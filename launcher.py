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
from importlib.metadata import PackageNotFoundError, version
from pathlib import Path

CONFIG_FILE = Path("config.json")
RUNTIME_CONFIG_FILE = Path("runtime_config.json")
VENV_NAME = "virtual_env"  # For downloading python packages
VENV_DIR = Path.cwd() / VENV_NAME  # Generate the path for the virtual environment
MEDIAPROCESSOR_PATH = Path("MediaProcessor") / "build"
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


def run_command(command: str, cwd=None):
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


def check_internet_connectivity(system: str) -> bool:
    """
    Check if the system has internet connectivity by pinging Google's public DNS server.
    """
    try:
        if system == "Windows":
            cmd = "ping -n 1 8.8.8.8"
        else:
            cmd = "ping -c 1 8.8.8.8"
        logging.debug("Checking internet connectivity... ")
        run_command(cmd)
        logging.debug("Internet connectivity OK")
        return True

    except Exception as e:
        logging.error("No internet connection detected.")
        logging.debug(f"Error: {e}")
        return False


def install_msys2():
    try:
        installer_url = (
            "https://github.com/msys2/msys2-installer/releases/download/nightly-x86_64/msys2-base-x86_64-latest.sfx.exe"
        )
        installer_name = "msys2-installer.exe"

        msys2_root_path = "C:\\msys64"

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
            f"Adding {msys2_root_path}\\usr\\bin, {msys2_root_path}\\mingw64\\bin, {msys2_root_path}\\mingw32\\bin to current enviornment."
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
        if Path(installer_name).exists():
            os.remove(installer_name)


def check_python_dependecies_installed():
    """
    check if the packages in the requirements.txt are installed
    """
    try:
        requirements = open("requirements.txt", "r").readlines()
        for req in requirements:
            req = req.strip()
            if not req or req.startswith("#"):  # Skip empty lines and comments
                continue
            operator = ">=" if ">=" in req else "=="

            package_name, required_version = req.split(operator)
            package_name = package_name.strip()
            installed_version = version(package_name)

            required_version = tuple(map(int, required_version.strip().split(".")))
            installed_version = tuple(map(int, installed_version.strip().split(".")))

            logging.debug(f"installed version of {package_name}: {installed_version}")
            if (operator == ">=" and installed_version >= required_version) or (
                operator == "==" and installed_version == required_version
            ):
                logging.debug(f"{package_name} is installed and meets the requirement.")
            else:
                logging.debug(
                    f"Version mismatch for {package_name}: "
                    f"installed {installed_version}, required {required_version}"
                )
                raise FileNotFoundError
    except (PackageNotFoundError, FileNotFoundError):
        logging.debug(f"{package_name} is not installed.")
        raise FileNotFoundError
    except Exception as e:
        logging.error(f"Error processing requirement {req}: {e}")
        sys.exit(1)


def ensure_virtualenv_exists():
    if VENV_DIR.exists():
        logging.debug("Virtual environment already exists.")
        return
    logging.info(f"Generating virtual environment at: {VENV_DIR}")
    run_command(f"{sys.executable} -m venv {str(VENV_DIR)} --system-site-packages")
    logging.info("Successfully generated Virtual environment.")


def get_virutalenv_folder() -> Path:
    """
    get the path of the virtual environment binaries folder
    """
    ensure_virtualenv_exists()
    for folder in ["bin", "Scripts"]:
        path = VENV_DIR / folder
        if path.exists():
            return path

    logging.error("Could not locate virtual environment folders.")
    sys.exit(1)


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

        Note: If a function is provided for check_cmd ensure it raise FileNotFoundError if the dependency is not installed.
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

    def ensure_installed(self, system):
        """
        Ensures the package is installed on the system.
        """
        if self.check_installed(system):
            return
        self.install_dependency(system)

    def check_installed(self, system) -> bool:
        """
        Checks if the dependency is installed.
        """
        logging.debug(f"Checking for {self.name} on {system}")
        commands = self._get_check_commands(system)
        try:
            for cmd in commands:
                if type(cmd) == str:
                    run_command(cmd)
                else:
                    cmd()
            return True
        except (FileNotFoundError, subprocess.CalledProcessError):
            logging.debug(f"{self.name} not Found.")
            return False
        except Exception as e:
            logging.error(f"While Checking Dependency: {e}", exc_info=True)
            sys.exit(1)

    def install_dependency(self, system: str):
        """
        Installs the dependency for the specified operating system.

        Args:
            system (str): The operating system type.
        """
        try:
            check_internet_connectivity(system)
            logging.debug(f"Installing {self.name} on {system}")
            commands = self._get_install_commands(system)
            for cmd in commands:
                if type(cmd) == str:
                    run_command(cmd)
                else:
                    cmd()
            logging.debug(f"Successfully installed {self.name}")
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
        MSYS2.ensure_installed("Windows")
        return [f"pacman -S --needed --noconfirm {self.package_name.get('Windows', self.name)}"]


MSYS2 = DependencyHandler(
    "MSYS2", check_cmd={"Windows": ["pacman --version"]}, install_cmd={"Windows": [install_msys2]}
)

# pkg-config should come before sndfie and nlohmann-json
required_dependencies = [
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
    DependencyHandler(
        "Python Dependencies",
        check_cmd={"all": [check_python_dependecies_installed]},
        install_cmd={
            "Windows": [f"{str(get_virutalenv_folder()/ 'pip.exe')} install -r requirements.txt"],
            "all": [f"{str(get_virutalenv_folder()/ 'pip')} install -r requirements.txt"],
        },
    ),
]


def ensure_MediaProcessor_build(system, re_build=False):
    """Ensure that the MediaProcessor is build. If rebuild is true, it will rebuild the MediaProcessor."""
    if not MEDIAPROCESSOR_PATH.exists():
        os.makedirs(MEDIAPROCESSOR_PATH)

    MediaProcessor_binary_path = MEDIAPROCESSOR_PATH / (
        "MediaProcessor.exe" if system == "Windows" else "MediaProcessor"
    )
    if MediaProcessor_binary_path.exists() and not re_build:
        logging.debug(f"{str(MediaProcessor_binary_path)} exists. Skipping re-build.")
        return
    try:
        logging.info("building MediaProcessor.")
        run_command("cmake -DCMAKE_BUILD_TYPE=Release ..", cwd=MEDIAPROCESSOR_PATH)
        run_command("cmake --build . --config Release", cwd=MEDIAPROCESSOR_PATH)
        logging.info("MediaProcessor built successfully.")
    except Exception as e:
        logging.error(f"Failed to build MediaProcessor: {e}", exc_info=True)
        sys.exit(1)


def ensure_runtime_config(system):
    """
    make a new runtime_config file with Platform independent settings.
    """
    try:
        with open(CONFIG_FILE, "r") as config_file:
            config = json.load(config_file)

        if system == "Windows":
            logging.info("Updating config file.")
            for key, value in config.items():
                if type(value) == str:
                    config[key] = value.replace("/", "\\")

        with open(RUNTIME_CONFIG_FILE, "w") as config_file:
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
        python_path = get_virutalenv_folder() / ("python.exe" if system == "Windows" else "python")

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

    system = platform.system()
    setup_logging(args.debug_level, args.debug_file)
    logging.info("Starting setup...")

    for dependency in required_dependencies:
        dependency.ensure_installed(system)

    ensure_MediaProcessor_build(system, args.rebuild)
    ensure_runtime_config(system)

    if args.app == "web":
        launch_web_application(system)
    else:
        logging.info("Please specify how you would like to launch the application, like --app=web.")


if __name__ == "__main__":
    main()
