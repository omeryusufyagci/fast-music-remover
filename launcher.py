import argparse
import json
import os
import platform
import subprocess
import sys
import webbrowser  # for opening web browser
from pathlib import Path

CONFIG_FILE = os.path.join("config.json")
venv_name = "myenv"  # For downloading python packages
venv_dir = Path.cwd() / venv_name  # Create the path for the virtual environment


def create_virtualenv():
    print(f"Creating virtual environment at: {venv_dir}")
    subprocess.check_call([sys.executable, "-m", "venv", str(venv_dir)])
    print("Virtual environment created successfully.")


def install_python_dependencies():
    try:
        if not venv_dir.exists():
            create_virtualenv()

        # Install dependencies in the virtual environment
        print("Installing pip dependencies...")
        subprocess.check_call(
            [str(venv_dir / "bin" / "pip"), "install", "-r", "requirements.txt"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
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
        subprocess.check_call(
            ["cmake", ".."],
            cwd=build_dir,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        subprocess.check_call(
            ["cmake", "--build", "."],
            cwd=build_dir,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        print("MediaProcessor built successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to build MediaProcessor: {e}")
        sys.exit(1)


def update_config():
    """Update config.json based on the platform."""
    try:
        with open(CONFIG_FILE, "r") as f:
            config = json.load(f)

        if platform.system() == "Windows":
            # Replace '/' in paths to '\\'
            for key, value in config.items():
                config[key] = value.replace("/", "\\")

        with open(CONFIG_FILE, "w") as f:
            json.dump(config, f, indent=4)

    except FileNotFoundError:
        print(f"{CONFIG_FILE} not found. Please create a default config.json file.")
        sys.exit(1)


def launch_web_application():
    try:
        subprocess.check_call([str(venv_dir / "bin" / "python"), "app.py"])
        # webbrowser.open("http://127.0.0.1:8080")

    except subprocess.CalledProcessError as e:
        print(f"Failed to start the web application: {e}")
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(description="Setup for MediaProcessor Application")
    parser.add_argument("--web", action="store_true", help="Launch the Web Application")
    args = parser.parse_args()

    print("Starting setup...")

    # Step 1: Install Python and C++ dependencies
    install_python_dependencies()
    build_cpp_dependencies()

    # Step 2: Update Configuration
    update_config()

    # Step 3: Start the application based on arguments
    if args.web:
        launch_web_application()
    else:
        print("Please specify an application mode like --web")


if __name__ == "__main__":
    main()
