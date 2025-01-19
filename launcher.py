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
from typing import Optional

PROCESSING_ENGINE_PATH = Path("MediaProcessor") / "build"
LOG_LOCK = threading.Lock()


class ConfigManager:
    """
    Manages Config files and logging Configuration.
    """

    CONFIG_FILE_PATH = Path("config.json")
    RUNTIME_CONFIG_FILE_PATH = Path("runtime_config.json")
    DEFAULT_LOGGING_CONFIG = {
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

    def __init__(self, system: str, log_level: str, log_file: bool = False) -> None:
        self.setup_runtime_config(system)
        self.setup_logging(log_level, log_file)

    def update_config_for_windows(self, configuration):
        for key, value in configuration.items():
            if type(value) == str:
                configuration[key] = value.replace("/", "\\")  # change / to \ for windows path compatibility

    def setup_runtime_config(self, system: str):
        logging.debug(f"Setting up runtime config for {system}")
        try:
            with open(ConfigManager.CONFIG_FILE_PATH, "r") as config_file:
                config = json.load(config_file)

            if system == "Windows":
                self.update_config_for_windows(config)

            with open(ConfigManager.RUNTIME_CONFIG_FILE_PATH, "w") as config_file:
                json.dump(config, config_file, indent=4)

        except FileNotFoundError:
            logging.error(
                f"{ConfigManager.CONFIG_FILE_PATH} not found. Please check a config file exists in project root."
            )
            sys.exit(1)
        except Exception as e:
            logging.error(f"Failed to update config: {e}", exc_info=True)
            sys.exit(1)

    def setup_logging(self, log_level: str, log_file=False):
        ConfigManager.DEFAULT_LOGGING_CONFIG["root"]["level"] = log_level
        if log_file:
            ConfigManager.DEFAULT_LOGGING_CONFIG["handlers"]["file"] = {
                "class": "logging.handlers.RotatingFileHandler",
                "formatter": "detailed",
                "filename": f"logfile_{time.strftime('%Y%m%d_%H%M%S')}.log",
                "maxBytes": 1024 * 1024 * 5,  # 5MB
                "backupCount": 3,
            }
            ConfigManager.DEFAULT_LOGGING_CONFIG["root"]["handlers"].append("file")

        logging.config.dictConfig(ConfigManager.DEFAULT_LOGGING_CONFIG)


class Utils:
    VENV_NAME = "virtual_env"  # For downloading python packages
    VENV_DIR_PATH = Path.cwd() / VENV_NAME  # Generate the path for the virtual environment

    @staticmethod
    def run_command(command: str, cwd: Optional[Path] = None):
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

    @staticmethod
    def check_internet_connectivity(system) -> bool:
        try:
            cmd = "ping -n 1 8.8.8.8" if system == "Windows" else "ping -c 1 8.8.8.8"
            Utils.run_command(cmd)
            logging.debug("Internet connectivity OK")
            return True
        except Exception as e:
            logging.error("No internet connection detected.")
            logging.debug(f"Error: {e}")
            return False

    @staticmethod
    def ensure_venv_exists():
        if Utils.VENV_DIR_PATH.exists():
            logging.debug(f"Virtual environment already exists at {Utils.VENV_DIR_PATH}.")
            return
        logging.info(f"Generating virtual environment at: {Utils.VENV_DIR_PATH}")
        Utils.run_command(f"{sys.executable} -m venv {str(Utils.VENV_DIR_PATH)} --system-site-packages")
        logging.info("Successfully generated Virtual environment.")

    @staticmethod
    def get_venv_binaries_path() -> Path:
        """
        get the path of the virtual environment binaries path
        """
        Utils.ensure_venv_exists()
        for directory_name in ["bin", "Scripts"]:
            path = Utils.VENV_DIR_PATH / directory_name
            logging.debug(f"Searching VENV Path: {path}")
            if path.exists():
                return path

        logging.error("Could not locate virtual environment folders.")
        sys.exit(1)


def install_msys2():
    """
    Installs MSYS2 and Update Path enviornment variable for windows platform
    """
    try:
        installer_url = "https://repo.msys2.org/distrib/x86_64/msys2-base-x86_64-20241208.sfx.exe"
        installer_name = "msys2-installer.exe"

        msys2_root_path = "C:\\msys64"

        logging.info("Downloading MSYS2 installer...")
        logging.debug(f"Installer URL: {installer_url}")
        Utils.run_command(f"curl -L -o {installer_name} {installer_url}")

        logging.info("Running MSYS2 installer...")
        logging.debug(f"Installing MSYS2 at {msys2_root_path}")
        Utils.run_command(f"{installer_name} -y -oC:\\")

        logging.info("Updating MSYS2 packages...")
        Utils.run_command(f"{msys2_root_path}\\usr\\bin\\bash.exe -lc 'pacman -Syu --noconfirm'")

        logging.info("Editing Environment Variables...")
        logging.debug(
            f"Adding {msys2_root_path}\\usr\\bin, {msys2_root_path}\\mingw64\\bin, {msys2_root_path}\\mingw32\\bin to PATH for current user."
        )
        # Set it permanently for the current user
        commands = f"""
        $oldPath = [Environment]::GetEnvironmentVariable("Path", "User")
        $newPath = "{msys2_root_path}\\usr\\bin;{msys2_root_path}\\mingw64\\bin;{msys2_root_path}\\mingw32\\bin;" + $oldPath
        [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
        """
        # Run the PowerShell commands
        subprocess.check_call(["powershell", "-Command", commands])
        logging.info("Added MSYS2 to PATH environment variable Permanently for current user.")

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


def validate_python_dependencies():
    """
    check if the packages in the requirements.txt are installed

    Raises:
        FileNotFoundError : if the requirements are not installed.

    Note:
        Currenly Supports `==` and `>=` operators only in the requirements.txt.
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

            logging.debug(f"Installed version of {package_name}: {installed_version}")
            if (operator == ">=" and installed_version >= required_version) or (
                operator == "==" and installed_version == required_version
            ):
                logging.debug(f"{package_name} is installed and meets the requirement.")
            else:
                logging.debug(
                    f"Version mismatch for {package_name}: "
                    f"installed {installed_version}, required {required_version}, operator {operator}"
                )
                raise FileNotFoundError
    except (PackageNotFoundError, FileNotFoundError):
        logging.debug(f"{package_name} is not installed.")
        raise FileNotFoundError
    except Exception as e:
        logging.error(f"Error processing requirement {req}: {e}")
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

    Note:
        If a function is provided for check_cmd ensure it raise FileNotFoundError if the dependency is not installed.
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
                    Utils.run_command(cmd)
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
        if not Utils.check_internet_connectivity(system):
            logging.error("Please Connect to a Internet Connection.")
            sys.exit(1)
        try:
            logging.debug(f"Installing {self.name} on {system}")
            commands = self._get_install_commands(system)
            for cmd in commands:
                if type(cmd) == str:
                    Utils.run_command(cmd)
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


def log_stream(stream, log_function):
    """Logs output from a stream."""
    for line in iter(stream.readline, ""):
        with LOG_LOCK:
            log_function(line.strip())
    stream.close()


def build_processing_engine(system, re_build=False):
    """Ensure that the MediaProcessor is build. If rebuild is true, it will rebuild the MediaProcessor."""
    if not PROCESSING_ENGINE_PATH.exists():
        os.makedirs(PROCESSING_ENGINE_PATH)

    MediaProcessor_binary_path = PROCESSING_ENGINE_PATH / (
        "MediaProcessor.exe" if system == "Windows" else "MediaProcessor"
    )
    if MediaProcessor_binary_path.exists() and not re_build:
        logging.debug(f"{str(MediaProcessor_binary_path)} exists. Skipping re-build.")
        return
    try:
        logging.info("building Processing Engine.")
        Utils.run_command("cmake -DCMAKE_BUILD_TYPE=Release ..", cwd=PROCESSING_ENGINE_PATH)
        Utils.run_command("cmake --build . --config Release", cwd=PROCESSING_ENGINE_PATH)
        logging.info("Processing Engine built successfully.")
    except Exception as e:
        logging.error(f"Failed to build MediaProcessor: {e}", exc_info=True)
        sys.exit(1)


class WebApplication:
    def __init__(self, system: str, log_level: str, log_file: bool = False):

        # FIXME: ConfigManager is not initialized yet so logging is not setup.
        #        Following log statements to be swapped once this is fixed.
        # logging.info("Setting up Web Application. This will resolve all dependencies, and may take a few minutes.")
        # logging.info("Run with `--log-level DEBUG` for more detailed output.")
        print("Setting up Web Application. This will resolve all dependencies, and may take a few minutes.")
        print("Run with `--log-level DEBUG` for more detailed output.")

        self.dependencies = [
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
                check_cmd={"all": [validate_python_dependencies]},
                install_cmd={
                    "Windows": [f"{str(Utils.get_venv_binaries_path()/ 'pip.exe')} install -r requirements.txt"],
                    "all": [f"{str(Utils.get_venv_binaries_path()/ 'pip')} install -r requirements.txt"],
                },
            ),
        ]
        self.system = system
        self.DEAFULT_URL = "http://127.0.0.1"
        self.DEAFULT_PORT = 8080
        self.timeout = 0.5
        self.setup(log_level, log_file)

    def setup(self, log_level: str, log_file: bool):
        """
        Installs the dependencies and Setup Configuration for the web application.
        """

        """ FIXME: Common dependencies shouldn't be resolved here. 
                   This should be exclusive to the WebApplication class dependencies.
        """

        self.config = ConfigManager(self.system, log_level, log_file)
        for dependency in self.dependencies:
            dependency.ensure_installed(self.system)

    def run(self, port: Optional[int] = None):
        try:
            python_path = Utils.get_venv_binaries_path() / ("python.exe" if self.system == "Windows" else "python")

            # Start the backend
            logging.debug("Starting backend")
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
            time.sleep(self.timeout)

            # Check if the process is still running
            if app_process.poll() is not None:
                error_output = app_process.stderr.read()
                logging.error(f"Error starting the backend: {error_output}")
                sys.exit(1)

            url = f"{self.DEAFULT_URL}:{port if port else self.DEAFULT_PORT}"
            logging.debug(f"Web application running on {url}.")
            webbrowser.open(url)

            logging.info("Web application running. Press Enter to stop.")
            input()  # Block until the user presses Enter

        except Exception as e:
            logging.error(f"An error occurred: {e}", exc_info=True)
            sys.exit(1)


Applications = {
    "web": WebApplication,
}


def main():
    parser = argparse.ArgumentParser(description="Setup for MediaProcessor Application.")
    parser.add_argument("--app", choices=["web", "none"], default="web", help="Specify launch mode (default=web).")
    parser.add_argument("--install-only", action="store_true", default=False, help="Install dependencies only.")
    parser.add_argument("--rebuild", action="store_true", help="Rebuild MediaProcessor")
    parser.add_argument(
        "--log-level", choices=["DEBUG", "INFO", "ERROR"], default="INFO", help="Set the logging level (default=INFO)."
    )
    parser.add_argument("--log-file", action="store_true", help="Outputs log in a log file.")
    args = parser.parse_args()

    """ FIXME: Control flow should be more transparent.
               It's unclear where dependencies are resolved (currently delegated to WebApplication).
               Similar to building the core, resolving common dependencies should be explicit.
               Consider adding init/cleanup functions to fix these without cluttering main.
    """

    system = platform.system()
    if args.app == "none":
        print("Please specify a launch option, e.g. `--app=web`. Start with `--help` to see all options.")
        sys.exit(0)

    # FIXME: App starts before ConfigManager is initialized leading to limited intermediary logging capability.
    # To be handled separately likely in the to-be-added init function.
    app = Applications[args.app](system, args.log_level, args.log_file)
    build_processing_engine(system, args.rebuild)
    if args.install_only:
        sys.exit(0)

    app.run()


if __name__ == "__main__":
    main()
