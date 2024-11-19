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
venv_dir = Path.cwd() / venv_name  # Create the path for the virtual environment


def execute_command(command, cwd=None):
    """
    Executes a shell command.

    Args:
        commands (str): The command to execute.

    Raises:
        subprocess.CalledProcessError: If any command fails.
    """
    subprocess.check_call(command.split(), cwd=cwd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


class Dependency:
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
            for debian based: apt-get
            for fedora/rhel: dnf
            for macos: brew
            for windows: msys2
    """

    def __init__(self, name, package_name={}, check_cmd={}, install_cmd={}) -> None:
        self.name = name
        self.package_name = package_name
        self.check_cmd = check_cmd
        self.install_cmd = install_cmd

    def check_dependency(self, system, install):
        """
        Checks if the dependency is installed. If not, installs it if `install` is True.

        Args:
            system (str): The operating system type.
            install (bool): Whether to install the dependency if not found.
        """
        if system in self.check_cmd:
            check_cmd = self.check_cmd[system]
        elif "all" in self.check_cmd:
            check_cmd = self.check_cmd["all"]
        else:
            check_cmd = [self.package_name.get(system, self.name) + " --version"]

        try:
            for cmd in check_cmd:
                if type(cmd) == str:
                    execute_command(cmd)
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
        if system in self.install_cmd:
            install_cmd = self.install_cmd[system]
        elif "all" in self.install_cmd:
            install_cmd = self.install_cmd["all"]
        else:
            if system == "Linux":
                import distro

                linux_distro = distro.id().lower()
                if linux_distro in ["ubuntu", "debian", "kali"]:
                    install_cmd = [
                        "sudo apt-get update",
                        f"sudo apt-get install -y {self.package_name.get(system, self.name)}",
                    ]
                elif linux_distro in ["fedora", "centos", "rhel"]:
                    install_cmd = [f"sudo dnf install -y {self.package_name.get(system, self.name)}"]
                else:
                    print(f"Unsupported Linux distribution. Please install {self.name} manually.")
                    sys.exit(1)
            elif system == "Darwin":
                install_cmd = [f"brew install {self.package_name.get(system, self.name)}"]
            elif system == "Windows":
                if not check_msys2_installed():
                    install_msys2()
                install_cmd = [f"pacman -S --needed --noconfirm {self.package_name.get(system, self.name)}"]
            else:
                print(f"Unsupported operating system: {system}. Please install {self.name} manually.")
                sys.exit(1)

        print(f"Installing {self.name}...")
        try:
            for cmd in install_cmd:
                if type(cmd) == str:
                    execute_command(cmd)
                else:
                    cmd()

            print(f"{self.name} installed successfully.")
        except subprocess.CalledProcessError as e:
            print(f"Error installing {self.name}: {e}")
            sys.exit(1)


# pkg-config should come before sndfie and nlohmann-json
dependencies = [
    Dependency("cmake", {"Windows": "mingw-w64-x86_64-cmake"}, {"all": ["cmake --version"]}),
    Dependency(
        "g++",
        {"Windows": "mingw-w64-x86_64-gcc", "Darwin": "gcc"},
        {"all": ["g++ --version"]},
        {"Windows": ["pacman -S --needed --noconfirm base-devel mingw-w64-x86_64-toolchain"]},
    ),
    Dependency("pkg-config"),
    Dependency("ffmpeg", {"Windows": "mingw-w64-x86_64-ffmpeg"}, {"all": ["ffmpeg -version"]}),
    Dependency(
        "libsndfile",
        {"Windows": "mingw-w64-x86_64-libsndfile", "Linux": "libsndfile1-dev"},
        {"all": ["pkg-config --exists sndfile"]},
    ),
    Dependency(
        "nlohmann-json",
        {"Windows": "mingw-w64-x86_64-nlohmann-json"},
        {"all": ["pkg-config --exists nlohmann_json"]},
    ),
]


def check_msys2_installed():
    try:
        execute_command("pacman --version")
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
        execute_command(f"curl -L -o {installer_name} {installer_url}")

        print("Running MSYS2 installer...")
        execute_command(f"{installer_name} -y -oC:\\")

        print("Updating MSYS2 packages...")
        execute_command(f"{msys2_root_path}\\usr\\bin\\bash.exe -lc 'pacman -Syu --noconfirm'")

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


def create_virtualenv():
    print(f"Creating virtual environment at: {venv_dir}")
    execute_command(f"{sys.executable} -m venv {str(venv_dir)} --system-site-packages")
    print("Virtual environment created successfully.")


def install_python_dependencies(system):
    try:
        if not venv_dir.exists():
            create_virtualenv()

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

        execute_command(f"{str(pip_path)} install -r requirements.txt")
        print("Python dependencies installed.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to install Python dependencies: {e}")
        sys.exit(1)


def build_cpp_dependencies(system):
    try:
        print("Building MediaProcessor...")
        build_dir = os.path.join("MediaProcessor", "build")
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)

        execute_command("cmake -DCMAKE_BUILD_TYPE=Release ..", cwd=build_dir)
        execute_command("cmake --build . --config Release", cwd=build_dir)

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
            config["ffmpeg_path"] = "ffmpeg"
        elif system == "Darwin":
            config["ffmpeg_path"] = "/usr/local/bin/ffmpeg"
        elif system == "Linux":
            config["ffmpeg_path"] = "/usr/bin/ffmpeg"

        with open(CONFIG_FILE, "w") as config_file:
            json.dump(config, config_file, indent=4)

    except FileNotFoundError:
        print(f"{CONFIG_FILE} not found. Please create a default config.json file.")
        sys.exit(1)


def launch_web_application(system):
    try:
        if not venv_dir.exists():
            create_virtualenv()

        if system == "Windows":
            python_path = str(venv_dir / "Scripts" / "python.exe")
        else:
            python_path = str(venv_dir / "bin" / "python")

        # Start the backend and capture errors
        app_process = subprocess.Popen([python_path, "app.py"], stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
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
    parser.add_argument("--mode", choices=["web"], help="Specify mode to launch")
    parser.add_argument("--install", action="store_true", help="Install dependencies if not found")
    parser.add_argument("--rebuild", action="store_true", help="Rebuild MediaProcessor")
    args = parser.parse_args()

    system = platform.system()
    print("Starting setup...")

    for dependency in dependencies:
        dependency.check_dependency(system, args.install)

    if args.install:
        install_python_dependencies(system)

    if args.rebuild or not check_MediaProcessor(system):
        build_cpp_dependencies(system)

    update_config(system)

    if args.mode == "web":
        launch_web_application(system)
    else:
        print("Please specify an application mode like --mode=web")


if __name__ == "__main__":
    main()
