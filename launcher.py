import argparse
import atexit
import json
import logging
import logging.config
import os
import platform
import subprocess
import sys
import threading
import time
import webbrowser
from pathlib import Path

CONFIG_FILE = Path("config.json")
VENV_NAME = "virtual_env"  # For downloading python packages
VENV_DIR = Path.cwd() / VENV_NAME  # Generate the path for the virtual environment
DEFAULT_CONFIG = {
    "version": 1,
    "formatters": {
        "detailed": {"format": "%(asctime)s [%(levelname)s] [%(funcName)s(), %(lineno)d]: %(message)s"},
        "simple": {"format": "[%(levelname)s] %(message)s"},
    },
    "handlers": {
        "console": {
            "class": "logging.StreamHandler",
            "formatter": "simple",
            "stream": "ext://sys.stdout",
        },
    },
    "root": {"level": "ERROR", "handlers": ["console"]},
}


def setup_logging(log_level, log_file=False):
    DEFAULT_CONFIG["root"]["level"] = log_level
    if log_file:
        DEFAULT_CONFIG["handlers"]["file"] = {
            "class": "logging.handlers.RotatingFileHandler",
            "formatter": "detailed",
            "filename": f"logfile_{time.strftime('%Y%m%d_%H%M%S')}.log",
            "maxBytes": 1024 * 1024 * 5,  # 5MB
            "backupCount": 3,
        }
        DEFAULT_CONFIG["root"]["handlers"].append("file")

    logging.config.dictConfig(DEFAULT_CONFIG)


def run_command(command, cwd=None):
    """
    Executes a shell command.

    Args:
        command (str): The command to run.
        cwd (str, optional): The working directory. Defaults to None.

    Raises:
        subprocess.CalledProcessError: If any command fails.
    """
    logging.debug(f"Executing command: {command}")
    process = subprocess.Popen(
        command.split(),
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    stdout, stderr = process.communicate()  # wait for process to terminate
    if stdout:
        logging.debug(f"Command output: {stdout}")
    if stderr:
        logging.error(f"Command error: {stderr}")

    if process.returncode != 0:
        raise subprocess.CalledProcessError(process.returncode, command)


class DependencyHandler:
    """
    Represents a dependency with methods for checking and installing it.

    Attributes:
        name (str): The name of the dependency.
        package_name (dict): { platform: package_name }.
        check_cmd (dict): { platform: list of command or function to check if the dependency is installed }.
        install_cmd (dict): { platform: list of command or function to install the dependency }.

        if package_name is not provided, it will be set to the name.

        if check_cmd is not provided
            package_name --version will be used to check if the dependency is installed

        if install cmd is not provided
            for linux: package manager based on distro
            for macos: brew
            for windows: msys2
    """

    def __init__(self, name, package_name=None, check_cmd=None, install_cmd=None):
        self.name = name
        self.package_name = package_name or {}
        self.check_cmd = check_cmd or {}
        self.install_cmd = install_cmd or {}
        self._installers = {
            "Linux": self._get_install_commands_linux,
            "Darwin": self._get_install_commands_darwin,
            "Windows": self._get_install_commands_windows,
        }
        # Mapping of Linux distributions to package managers
        self._linux_distro_map = {
            "ubuntu": "sudo apt-get install -y",
            "debian": "sudo apt-get install -y",
            "kali": "sudo apt-get install -y ",
            "pop": "sudo apt-get install -y ",
            "elementary": "sudo apt-get install -y ",
            "mint": "sudo apt-get install -y",
            "fedora": "sudo dnf install -y",
            "rhel": "sudo dnf install -y",
            "centos": "sudo dnf install -y",
            "rocky": "sudo dnf install -y",
            "alma": "sudo dnf install -y",
            "arch": "sudo pacman -S --needed --noconfirm",
            "manjaro": "sudo pacman -S --needed --noconfirm",
            "endeavouros": "sudo pacman -S --needed --noconfirm",
            "garuda": "sudo pacman -S --needed --noconfirm",
            "opensuse": "sudo zypper install -y",
            "suse": "sudo zypper install -y",
            "alpine": "sudo apk add",
            "solus": "sudo eopkg install -y",
            "void": "sudo xbps-install -y",
            "clearlinux": "sudo swupd bundle-add",
        }

    def ensure_dependency_installed(self, system, install=True):
        """
        Checks if the dependency is installed. If not installs it if install arg is True.
        """
        commands = self._get_check_commands(system)
        try:
            for cmd in commands:
                if type(cmd) == str:
                    run_command(cmd)
                else:
                    cmd()

        except (FileNotFoundError, subprocess.CalledProcessError):
            if not install:
                logging.info(f"{self.name} is not installed.")
                sys.exit(1)
            self.install_dependency(system)

        except Exception as e:
            logging.error(f"While Checking Dependency: {e}", exc_info=True)
            sys.exit(1)

    def install_dependency(self, system):
        """
        Installs the dependency for the specified operating system.

        Args:
            system (str): The operating system type.
        """
        try:
            commands = self._get_install_commands(system)
            for cmd in commands:
                if type(cmd) == str:
                    run_command(cmd)
                else:
                    cmd()
        except Exception as e:
            logging.error(f"While Installing: {e}.", exc_info=True)
            sys.exit(1)

    def _get_check_commands(self, system):
        if system in self.check_cmd:
            return self.check_cmd[system]
        if "all" in self.check_cmd:
            return self.check_cmd["all"]
        return [f"{self.package_name.get(system, self.name)} --version"]

    def _get_install_commands(self, system):
        if system in self.install_cmd:
            return self.install_cmd[system]
        if "all" in self.install_cmd:
            return self.install_cmd["all"]
        if system not in self._installers:
            logging.error(f"Unspported sytem {system}.")
            sys.exit(1)

        return self._installers[system]()

    def _get_install_commands_linux(self):
        import distro

        distro = distro.id().lower()
        if distro not in self._linux_distro_map:
            logging.error(f"Unsupported linux distro {distro}.")
            sys.exit(1)

        return [f"{self._linux_distro_map[distro]} {self.package_name.get('Linux', self.name)}"]

    def _get_install_commands_darwin(self):
        return [f"brew install {self.package_name.get('Darwin', self.name)}"]

    def _get_install_commands_windows(self):
        if not check_msys2_installed():
            install_msys2()
        return [f"pacman -S --needed --noconfirm {self.package_name.get('Windows', self.name)}"]


# pkg-config should come before sndfie and nlohmann-json
dependencies = [
    DependencyHandler("cmake", {"Windows": "mingw-w64-x86_64-cmake"}, {"all": ["cmake --version"]}),
    DependencyHandler(
        "g++",
        {"Windows": "mingw-w64-x86_64-gcc", "Darwin": "gcc"},
        {"all": ["g++ --version"]},
        {"Windows": ["pacman -S --needed --noconfirm base-devel mingw-w64-x86_64-toolchain"]},
    ),
    DependencyHandler("pkg-config"),
    DependencyHandler("ffmpeg", {"Windows": "mingw-w64-x86_64-ffmpeg"}, {"all": ["ffmpeg -version"]}),
    DependencyHandler(
        "libsndfile",
        {"Windows": "mingw-w64-x86_64-libsndfile", "Linux": "libsndfile1-dev"},
        {"all": ["pkg-config --exists sndfile"]},
    ),
    DependencyHandler(
        "nlohmann-json",
        {"Windows": "mingw-w64-x86_64-nlohmann-json", "Linux": "nlohmann-json3-dev"},
        {"all": ["pkg-config --exists nlohmann_json"]},
    ),
]


def check_msys2_installed():
    try:
        run_command("pacman --version")
        return True
    except (FileNotFoundError, subprocess.CalledProcessError):
        return False


def check_internet_connectivity():
    """For Windows check internet connectivity"""
    try:
        logging.debug("Checking internet connectivity... ")
        # Ping Google's public DNS server
        run_command("ping -n 1 8.8.8.8")
        logging.debug("Internet connectivity OK")
    except subprocess.CalledProcessError:
        logging.error("No internet connection detected.")
        sys.exit(1)


def install_msys2():
    try:
        installer_url = (
            "https://github.com/msys2/msys2-installer/releases/download/nightly-x86_64/msys2-base-x86_64-latest.sfx.exe"
        )
        installer_name = "msys2-installer.exe"

        msys2_root_path = "C:\\msys64"

        check_internet_connectivity()

        logging.info("Downloading MSYS2 installer...")
        logging.debug(f"Installer URL: {installer_url}")
        run_command(f"curl -L -o {installer_name} {installer_url}")

        logging.info("Running MSYS2 installer...")
        logging.debug(f"Installing MSYS2 at {msys2_root_path}")
        run_command(f"{installer_name} -y -oC:\\")

        logging.info("Updating MSYS2 packages...")
        run_command(f"{msys2_root_path}\\usr\\bin\\bash.exe -lc 'pacman -Syu --noconfirm'")

        logging.info("Editing Environment Variables...")
        logging.debug(
            f"Adding {msys2_root_path}\\usr\\bin, {msys2_root_path}\\mingw64\\bin, {msys2_root_path}\\mingw32\\bin to PATH"
        )
        # Set it permanently for the current user
        commands = f"""
        $oldPath = [Environment]::GetEnvironmentVariable("Path", "User")
        $newPath = "{msys2_root_path}\\usr\\bin;{msys2_root_path}\\mingw64\\bin;{msys2_root_path}\\mingw32\\bin;" + $oldPath
        [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
        """
        # Run the PowerShell commands
        subprocess.check_call(["powershell", "-Command", commands])

        # Add MSYS2 paths to the PATH environment variable for the current session
        logging.debug(
            f"Addiing {msys2_root_path}\\usr\\bin, {msys2_root_path}\\mingw64\\bin, {msys2_root_path}\\mingw32\\bin to current enviornment."
        )
        current_path = os.environ.get("PATH", "")
        new_path = (
            f"{msys2_root_path}\\usr\\bin;{msys2_root_path}\\mingw64\\bin;{msys2_root_path}\\mingw32\\bin;"
            + current_path
        )
        os.environ["PATH"] = new_path

        logging.info("MSYS2 installed and updated successfully.")
        logging.info("NOTE: Please restart your terminal before running this script again.")

    except subprocess.CalledProcessError as e:
        logging.error(f"Error installing MSYS2: {e}", exc_info=True)
        sys.exit(1)
    finally:
        if Path.exists(installer_name):
            os.remove(installer_name)


def check_MediaProcessor(system):
    if system == "Windows":
        MediaProcessor_path = Path("MediaProcessor") / "build" / "MediaProcessor.exe"
    else:
        MediaProcessor_path = Path("MediaProcessor") / "build" / "MediaProcessor"
    return MediaProcessor_path.exists()


def generate_virtualenv():
    logging.info(f"Generating virtual environment at: {VENV_DIR}")
    run_command(f"{sys.executable} -m venv {str(VENV_DIR)} --system-site-packages")
    logging.info("Successfully generated Virtual environment.")


def install_python_dependencies(system):
    try:
        if not VENV_DIR.exists():
            generate_virtualenv()

        logging.info("Installing Python dependencies...")

        if system == "Windows":
            # Check for both `Scripts` and `bin` folders.
            pip_scripts_path = VENV_DIR / "Scripts" / "pip.exe"
            pip_bin_path = VENV_DIR / "bin" / "pip.exe"

            if not pip_scripts_path.exists() and not pip_bin_path.exists():
                logging.error(f"pip not found. Searched paths: [{pip_scripts_path}] and [{pip_bin_path}]")
                sys.exit(1)

            if pip_bin_path.exists():
                pip_path = pip_bin_path
            else:
                pip_path = pip_scripts_path

        else:
            pip_path = VENV_DIR / "bin" / "pip"
            if not pip_path.exists():
                logging.error(f"pip not found. Searched paths: [{pip_path}]")
                sys.exit(1)

        run_command(f"{str(pip_path)} install -r requirements.txt")
        logging.info("Python dependencies installed.")
    except Exception as e:
        logging.error(f"Failed to install Python dependencies: {e}", exc_info=True)
        sys.exit(1)


def build_cpp_dependencies():
    try:
        logging.info("Building MediaProcessor...")
        build_dir = Path.join("MediaProcessor", "build")
        if not Path.exists(build_dir):
            os.makedirs(build_dir)

        run_command("cmake -DCMAKE_BUILD_TYPE=Release ..", cwd=build_dir)
        run_command("cmake --build . --config Release", cwd=build_dir)

        logging.info("MediaProcessor built successfully.")
    except Exception as e:
        logging.error(f"Failed to build MediaProcessor: {e}", exc_info=True)
        sys.exit(1)


def update_config(system):
    try:
        with open(CONFIG_FILE, "r") as config_file:
            config = json.load(config_file)

        if system == "Windows":
            for key, value in config.items():
                if type(value) == str:
                    config[key] = value.replace("/", "\\")

        with open(CONFIG_FILE, "w") as config_file:
            json.dump(config, config_file, indent=4)

    except FileNotFoundError:
        logging.error(f"{CONFIG_FILE} not found. Please create a default config.json file.")
        sys.exit(1)
    except Exception as e:
        logging.error(f"Failed to update config: {e}", exc_info=True)
        sys.exit(1)


def log_stream(stream, log_function):
    """Logs output from a stream."""
    for line in iter(stream.readline, ""):
        log_function(line.strip())
    stream.close()


def launch_web_application(system):
    try:
        if not VENV_DIR.exists():
            generate_virtualenv()

        if system == "Windows":
            python_path = str(VENV_DIR / "Scripts" / "python.exe")
        else:
            python_path = str(VENV_DIR / "bin" / "python")

        # Start the backend
        app_process = subprocess.Popen(
            [python_path, "app.py"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        atexit.register(app_process.terminate)

        # Threads to handle stdout and stderr asynchronously
        threading.Thread(target=log_stream, args=(app_process.stdout, logging.debug), daemon=True).start()
        threading.Thread(target=log_stream, args=(app_process.stderr, logging.debug), daemon=True).start()

        # Give the process some time to initialize
        time.sleep(0.5)

        # Check if the process is still running
        if app_process.poll() is not None:
            error_output = app_process.stderr.read()
            logging.error(f"Error starting the backend: {error_output}")
            sys.exit(1)

        webbrowser.open("http://127.0.0.1:8080")

        logging.info("Web application running. Press Enter to stop.")
        input()  # Block until the user presses Enter

    except Exception as e:
        logging.error(f"An error occurred: {e}", exc_info=True)
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(description="Setup for MediaProcessor Application")
    parser.add_argument("--app", choices=["web", "none"], default="web", help="Specify launch mode")
    parser.add_argument("--install-dependencies", action="store_true", default=True, help="Install dependencies.")
    parser.add_argument("--rebuild", action="store_true", help="Rebuild MediaProcessor")
    parser.add_argument(
        "--debug-level", choices=["DEBUG", "INFO", "ERROR"], default="INFO", help="Set the debug output level."
    )
    parser.add_argument("--debug-file", action="store_true", help="Set the debug file.")
    args = parser.parse_args()

    """
    Initialise logging
    check dependency
    """

    system = platform.system()
    setup_logging(args.debug_level, args.debug_file)
    logging.info("Starting setup...")

    for dependency in dependencies:
        dependency.ensure_dependency_installed(system, args.install_dependencies)

    if args.install_dependencies:
        install_python_dependencies(system)

    if args.rebuild or not check_MediaProcessor(system):
        build_cpp_dependencies()

    update_config(system)

    if args.app == "web":
        launch_web_application(system)
    else:
        logging.info("Please specify how you would like to launch the application, like --app=web.")


if __name__ == "__main__":
    main()
