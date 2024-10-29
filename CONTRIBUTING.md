# Contributing to Fast Music Remover

Thank you for your interest in `Fast Music Remover`! Your contributions are invaluable in providing a free and open tool that is centered around tackling practical problems of internet media.

## Contribution Channels

### 1. Issues:
You can quickly report most issues with the [issue templates](https://github.com/omeryusufyagci/fast-music-remover/issues/new/choose) for the following types:
* **Bug Report**: Report any unintended behavior, such as crashes, errors, etc.
* **Feature Request**: Let us know what's missing and take part in shaping the direction of the project.
* **Improvement Request**: Did you find some code that is inefficient, docs not clear, or anything in general that can be improved? Provide your feedback with an Improvement Request.

If these do not cover your specific case, you're welcome to open a custom issue.

### 2. Code Contributions:
Have a look at the [issues](https://github.com/omeryusufyagci/fast-music-remover/issues), especially those tagged `good first issue`, which are easier to get started with and help you understand the general workflow. 

For code contributions, please first open an issue and provide a description. Open a dedicated branch at the issue page, and only then submit your PR targeting this dedicated branch. 

### 3. Testing

As a new and rapidly growing project, we rely on tests to maintain our momentum as the codebase expands. Contributions that add new tests are highly appreciated, as they help ensure stability and prevent regressions. Additionally, testing the application on your system and reporting any issues you encounter is invaluable to us. Every bit of feedback helps!

### 4. Documentation:
* If you find any part of the documentation missing or unclear, please consider contributing improvements. 

### 5. Outreach
If you know others who might benefit from `Fast Music Remover` or want to contribute, help us by sharing the project with them. Join the [discussions](https://github.com/omeryusufyagci/fast-music-remover/discussions) to talk about ideas, give feedback, or ask questions—your input helps shape the project’s direction!

## Contribution Guidelines

### Getting Started:

#### Step 1: Fork and Clone the Repository

Fork the repository and clone your fork:
```sh
git clone https://github.com/<your-username>/fast-music-remover.git
```
#### Step 2: Set Up Your Development Environment

After cloning the repository, make sure to install the necessary dependencies as described in the [Getting Started](README.md#getting-started) section of the README. Ensure the `config.json` file at the root is properly configured for your system before continuing.

Install the Python dependencies with:

```sh
pip install -r requirements.txt
```
#### Step 3: Set Up Pre-Commit Hooks

We use pre-commit hooks to ensure code quality and maintain consistency across the project. This will automatically format your code before each commit.

1. Install `pre-commit`:
```sh
pip install pre-commit
```
2. Install the hooks:
```sh
pre-commit install
```

> **Note**: These hooks will automatically run `black` for the backend code in Python and clang-format for the `MediaProcessor/src/`, before every commit. For a better experience, enable 'Format on Save' on your IDE, and point to the `.clang-format` file for C++.

#### Step 4: Make Your Changes

As described above, make your changes on the dedicated issue branch, that you've generated via the GitHub Web UI. Here's an example
```sh
git fetch origin
git switch 123-fix-certain-bug-in-core
git add -u
git commit -m "Core: Fix bug where ..."
git push origin 123-fix-certain-bug-in-core
```
Please ensure commit hygeine and use the following prefixes:
* For changes in the MediaProcessor: `Core: <commit message>`
* For changes in the backend: `Backend: <commit message>`
* For changes in the backend: `Frontend: <commit message>`

Please adhere to the following practices:
* Write your commit messages in imperative form, e.g. "Fix bug where..", instead of "Fixed bug where.."
* Avoid long commit message bodies. Instead give a brief message, add 2 blank lines then provide further context where needed. Here the imperative form is not required.

#### Step 5: Submit a Pull Request

Submit a pull request explaining the nature of the changes. 

### Pull Request Guidelines:
* **Keep it Modular**: Focus on a singular unit of work for each pull request. This helps make the review process quicker and more efficient. If you're in doubt, feel free to clarify over at the discussions section or at the Discord server.
* **Write a Useful Description**: Clearly describe what changes have been made and why they are necessary.
* **Reference Issues**: Link the underlying issue in your pull request description using `Closes #issue-number`.

### Style Guidelines:
- **Python**: Please use black for formatting Python code. This is automatically done if you have set up the pre-commit hooks.
- **C++**: Please follow the existing style for C++ files (refer to the .clang-format file). The pre-commit hooks will automatically run clang-format on `MediaProcessor/src`.

### Commenting Guidelines:

#### General Rules:
- **Avoid Redundancy**: Do not add comments for variables, parameters or functions that are self-explanatory.
- **High-Level Overviews**: Add comments for public member functions and classes to provide a brief overview of what they do. Include details if the method performs multiple steps or has a more complex workflow.
- **Minimal Private Method Comments**: Avoid commenting on private methods or member variables unless they perform non-obvious functions.
- **Doxygen Style for Public API**: Use Doxygen-style comments (e.g., `@brief`) for public API documentation.
- **Clearly Note Exceptions**: If it's possible for a function to throw, clearly document this in the public API with a `@throws` tag.
  
#### Comment Style Example:
```cpp
/**
 * @brief Merges the audio and video files into a single output file.
 * 
 * @return true if the merge is successful, false otherwise.
 */
bool mergeMedia();
```

#### Class Comments:
Provide a brief description of what the class is responsible for. For abstract classes or interfaces, a minimal comment indicating its purpose is sufficient.

### Got Questions?
Got questions, feedback or a feature request? Don't miss the [discussion](https://github.com/omeryusufyagci/fast-music-remover/discussions), and the [discord server](https://discord.gg/xje3PQTEYp) to get in touch!