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

import distro

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
        check_cmd (dict): { platform: command to check if the dependency is installed }.
        install_cmd (dict): { platform: command to install the dependency }.

        if package_name is not provided, it will be set to the name.

        if check_cmd is not provided
            package_name -version will be used to check if the dependency is installed

        if install cmd is not provided
            for debian based: apt-get
            for fedora/rhel: dnf
            for macos: brew
            for windwos: msys2
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
        if "all" in self.check_cmd:
            check_cmd = self.check_cmd["all"]
        elif system in self.check_cmd:
            system in self.check_cmd[system]
        else:
            check_cmd = self.package_name.get(system, self.name) + " -version"

        try:
            execute_command(check_cmd)
        except (FileNotFoundError, subprocess.CalledProcessError):
            if install:
                self.install_dependency(system)
            else:
                print(
                    f"{self.name} is not installed. Use --install to install dependencies automatically or install manually."
                )
                sys.exit(1)

    def install_dependency(self, system):
        """
        Installs the dependency for the specified operating system.

        Args:
            system (str): The operating system type.
        """
        if "all" in self.install_cmd:
            install_cmd = self.install_cmd["all"]
        elif system in self.install_cmd:
            install_cmd = self.install_cmd[system]
        else:
            if system == "Linux":
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
                install_cmd = ["pacman -Syu", f"pacman -S --needed {self.package_name.get(system, self.name)}"]
            else:
                print(f"Unsupported operating system: {system}. Please install {self.name} manually.")
                sys.exit(1)

        print(f"Installing {self.name}...")
        try:
            if type(install_cmd) == str:
                execute_command(install_cmd)
            else:
                for cmd in install_cmd:
                    execute_command(cmd)
            print(f"{self.name} installed successfully.")
        except subprocess.CalledProcessError as e:
            print(f"Error installing {self.name}: {e}")
            sys.exit(1)


dependencies = [
    Dependency("cmake"),
    Dependency("pkg-config"),
    Dependency("ffmpeg", {"Windows": "mingw-w64-x86_64-ffmpeg"}),
    Dependency(
        "sndfile",
        {"Windows": "mingw-w64-x86_64-libsndfile", "Linux": "libsndfile1-dev"},
        {"all": "pkg-config --exists sndfile"},
    ),
]


def check_msys2_installed():
    try:
        execute_command("pacman --version")
        return True
    except subprocess.CalledProcessError:
        return False


def install_msys2():
    try:
        installer_url = "https://repo.msys2.org/distrib/x86_64/msys2-x86_64-latest.exe"
        installer_name = "msys2-installer.exe"
        print("Downloading MSYS2 installer...")
        execute_command(f"curl -L -o {installer_name} {installer_url}")

        print("Running MSYS2 installer...")
        execute_command(f"start /wait {installer_name} --quiet-mode --root C:\\msys64")

        print("Updating MSYS2 packages...")
        execute_command(f"C:\\msys64\\usr\\bin\\bash.exe -lc pacman -Syu --noconfirm")

        print("MSYS2 installed and updated successfully.")

    except subprocess.CalledProcessError as e:
        print(f"Error installing MSYS2: {e}")
        sys.exit(1)
    finally:
        if os.path.exists(installer_name):
            os.remove(installer_name)


def check_MediaProcessor():
    MediaProcessor_path = Path("MediaProcessor") / "build" / "MediaProcessor"
    return MediaProcessor_path.exists()


def create_virtualenv():
    print(f"Creating virtual environment at: {venv_dir}")
    execute_command(f"{sys.executable} -m venv {str(venv_dir)} --system-site-packages")
    print("Virtual environment created successfully.")


def install_python_dependencies():
    try:
        if not venv_dir.exists():
            create_virtualenv()

        print("Installing pip dependencies...")
        execute_command(f"{str(venv_dir / 'bin' / 'pip')} install -r requirements.txt")
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


def launch_web_application():
    try:
        app_process = subprocess.Popen([str(venv_dir / "bin" / "python"), "app.py"])
        atexit.register(app_process.terminate)

        time.sleep(0.5)
        webbrowser.open("http://127.0.0.1:8080")

        print("Web application running. Press Enter to stop.")
        input()  # Block until the user presses Enter

    except subprocess.CalledProcessError as e:
        print(f"Failed to start the web application: {e}")
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
        install_python_dependencies()

    if not check_MediaProcessor() or args.rebuild:
        build_cpp_dependencies()

    update_config(system)

    if args.mode == "web":
        launch_web_application()
    else:
        print("Please specify an application mode like --mode=web")


if __name__ == "__main__":
    main()
