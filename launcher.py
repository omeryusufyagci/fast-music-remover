import argparse
import atexit
import json
import os
import platform
import subprocess
import sys
import time
import webbrowser
from pathlib import Path

CONFIG_FILE = os.path.join("config.json")
venv_name = "virtual_env"  # For downloading python packages
venv_dir = Path.cwd() / venv_name  # Generate the path for the virtual environment


class DebugLevel:
    """Enum for debug levels"""

    NONE = 0
    APP = 1
    VERBOSE = 2

    _string_to_debug_level = {"none": NONE, "app": APP, "verbose": VERBOSE}


current_debug_level = DebugLevel.NONE


def run_command(command, cwd=None):
    """
    Executes a shell command.

    Args:
        command (str): The command to run.

    Raises:
        subprocess.CalledProcessError: If any command fails.
    """
    if current_debug_level == DebugLevel.VERBOSE:
        print(f"Executing command: {command}")
    subprocess.check_call(
        command.split(),
        cwd=cwd,
        stdout=None if current_debug_level == DebugLevel.VERBOSE else subprocess.DEVNULL,
        stderr=None if current_debug_level == DebugLevel.VERBOSE else subprocess.DEVNULL,
    )


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

    def ensure_dependency_installed(self, system, install=False):
        """
        Checks if the dependency is installed. If not, installs it.
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
                print(
                    f"{self.name} is not installed. Use --install to install dependencies automatically or install manually."
                )
                sys.exit(1)
            self.install_dependency(system)

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
            print(f"Error: {e}.\n\nInstall {self.name} manually.")
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
            print(f"Unspported sytem {system}. Please Install {self.name} manually.")
            sys.exit(1)

        return self._installers[system]()

    def _get_install_commands_linux(self):
        import distro

        distro = distro.id().lower()
        if distro not in self._linux_distro_map:
            print(f"Unsupported linux distro {distro}. Please Install {self.name} manually.")
            sys.exit(1)

        return [f"{self._linux_distro_map[distro]} {{self.package_name.get(system, self.name)}}"]

    def _get_install_commands_darwin(self):
        return [f"brew install {self.package_name.get('Darwin', self.name)}"]

    def _get_install_commands_windows(self):
        if not check_msys2_installed():
            install_msys2()
        return [f"pacman -S --needed --noconfirm {{self.package_name.get(system, self.name)}}"]


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


def install_msys2():
    try:
        global msys2_root_path
        installer_url = (
            "https://github.com/msys2/msys2-installer/releases/download/nightly-x86_64/msys2-base-x86_64-latest.sfx.exe"
        )
        installer_name = "msys2-installer.exe"

        msys2_root_path = "C:\\msys64"

        print("Downloading MSYS2 installer...")
        run_command(f"curl -L -o {installer_name} {installer_url}")

        print("Running MSYS2 installer...")
        run_command(f"{installer_name} -y -oC:\\")

        print("Updating MSYS2 packages...")
        run_command(f"{msys2_root_path}\\usr\\bin\\bash.exe -lc 'pacman -Syu --noconfirm'")

        print("Editing Environment Variables...")
        # Set it permanently for the current user
        commands = f"""
        $oldPath = [Environment]::GetEnvironmentVariable("Path", "User")
        $newPath = "{msys2_root_path}\\usr\\bin;{msys2_root_path}\\mingw64\\bin;{msys2_root_path}\\mingw32\\bin;" + $oldPath
        [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
        """
        # Run the PowerShell commands
        subprocess.check_call(["powershell", "-Command", commands])

        # Add MSYS2 paths to the PATH environment variable for the current session
        current_path = os.environ.get("PATH", "")
        new_path = (
            f"{msys2_root_path}\\usr\\bin;{msys2_root_path}\\mingw64\\bin;{msys2_root_path}\\mingw32\\bin;"
            + current_path
        )
        os.environ["PATH"] = new_path

        print("MSYS2 installed and updated successfully.")
        print("NOTE: Please restart your terminal before running this script again.")

    except subprocess.CalledProcessError as e:
        print(f"Error installing MSYS2: {e}")
        sys.exit(1)
    finally:
        if os.path.exists(installer_name):
            os.remove(installer_name)


def check_MediaProcessor(system):
    if system == "Windows":
        MediaProcessor_path = Path("MediaProcessor") / "build" / "MediaProcessor.exe"
    else:
        MediaProcessor_path = Path("MediaProcessor") / "build" / "MediaProcessor"
    return MediaProcessor_path.exists()


def generate_virtualenv():
    print(f"Creating virtual environment at: {venv_dir}")
    run_command(f"{sys.executable} -m venv {str(venv_dir)} --system-site-packages")
    print("Successfully generated Virtual environment.")


def install_python_dependencies(system):
    try:
        if not venv_dir.exists():
            generate_virtualenv()

        print("Installing Python dependencies...")

        if system == "Windows":
            # Check for both `Scripts` and `bin` folders.
            pip_path = venv_dir / "Scripts" / "pip.exe"
            if not pip_path.exists():
                pip_path = venv_dir / "bin" / "pip.exe"
            if not pip_path.exists():
                raise FileNotFoundError("pip not found in either Scripts or bin folder.")
        else:
            pip_path = venv_dir / "bin" / "pip"
            if not pip_path.exists():
                raise FileNotFoundError("pip not found in the bin folder.")

        run_command(f"{str(pip_path)} install -r requirements.txt")
        print("Python dependencies installed.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to install Python dependencies: {e}")
        sys.exit(1)


def build_cpp_dependencies():
    try:
        print("Building MediaProcessor...")
        build_dir = os.path.join("MediaProcessor", "build")
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)

        run_command("cmake -DCMAKE_BUILD_TYPE=Release ..", cwd=build_dir)
        run_command("cmake --build . --config Release", cwd=build_dir)

        print("MediaProcessor built successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to build MediaProcessor: {e}")
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
        print(f"{CONFIG_FILE} not found. Please create a default config.json file.")
        sys.exit(1)


def launch_web_application(system):
    try:
        if not venv_dir.exists():
            generate_virtualenv()

        if system == "Windows":
            python_path = str(venv_dir / "Scripts" / "python.exe")
        else:
            python_path = str(venv_dir / "bin" / "python")

        # Start the backend
        app_process = subprocess.Popen(
            [python_path, "app.py"],
            stdout=None if current_debug_level >= DebugLevel.APP else subprocess.DEVNULL,
            stderr=subprocess.PIPE,
        )
        atexit.register(app_process.terminate)

        # Give the process some time to initialize
        time.sleep(0.5)

        # Check if the process is still running
        if app_process.poll() is not None:
            error_output = app_process.stderr.read().decode("utf-8")
            print(f"Error starting the backend: {error_output}")
            sys.exit(1)

        webbrowser.open("http://127.0.0.1:8080")

        print("Web application running. Press Enter to stop.")
        input()  # Block until the user presses Enter

    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(description="Setup for MediaProcessor Application")
    parser.add_argument("--app", choices=["web", "none"], default="web", help="Specify launch mode")
    parser.add_argument("--install-dependencies", action="store_true", default=True, help="Install dependencies.")
    parser.add_argument("--rebuild", action="store_true", help="Rebuild MediaProcessor")
    parser.add_argument("--debug", choices=["verbose", "app"], default="none", help="Set the debug output level.")
    args = parser.parse_args()

    system = platform.system()
    print("Starting setup...")

    global current_debug_level
    current_debug_level = DebugLevel._string_to_debug_level[args.debug]

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
        print("Please specify how you would like to launch the application, like --app=web.")


if __name__ == "__main__":
    main()
