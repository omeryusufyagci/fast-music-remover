# Contributing to Fast Music Remover

Thank you for your interest in `Fast Music Remover`! Your contributions are invaluable in advancing this free and open tool, designed to address real-world challenges around internet media.

Please read this page carefully to get up to speed with our development practices, and don't hesitate to get in touch if you have questions!

## Contribution Channels

### 1. Issues:
You can quickly report most issues with our [issue templates](https://github.com/omeryusufyagci/fast-music-remover/issues/new/choose).
If these do not cover your specific case, you're welcome to open a custom issue.

### 2. Code Contributions:
Have a look at our [open issues](https://github.com/omeryusufyagci/fast-music-remover/issues), especially those tagged `good first issue`, which should be easier to get started with and help you understand the general workflow. 

To start off, open a new issue it's not already covered by an existing one, and provide a description. 
A dedicated branch will be opened by a maintainer and you should target your PR for that branch, e.g. `100-fix-broken-build-for-amd64-linux`.
Please also take a look at our [Commit Guidelines](#commit-guidelines), and adhere to both for consistency. 

### 3. Testing
* We're using Google Test for our [processing engine](https://github.com/omeryusufyagci/fast-music-remover/tree/main/MediaProcessor) and have coverage around the most critical functionality. 
  However, our coverage isn't as great for utilities and other less-critical parts. Improvements in these areas would be welcome additions!
* We're still missing tests for the [Python backend](https://github.com/omeryusufyagci/fast-music-remover/blob/main/app.py), and the backend itself is long overdue for a refactor to reorganize it better. If you're interested in this, please get in touch!

### 4. Documentation:
* If you find any part of the documentation missing, unclear or outdated, please let us know, and consider improving it by submitting a PR.

### 5. Outreach
We're building this with one thing in mind: to provide an **accessible solution** to a **shared problem**. 

If you know others who might benefit from or contribute to `Fast Music Remover`, **help us by sharing the project with them**. 
Join the [discussions](https://github.com/omeryusufyagci/fast-music-remover/discussions) here or over at [discord](https://discord.gg/xje3PQTEYp) to talk about ideas, give feedback, and ask questions. 

Your input helps shape the project’s direction, so don't hesitate to get in touch!

## Contribution Guidelines

### Getting Started:

#### Step 1: Fork and Clone the Repository

Fork the repository and clone your fork:
```sh
git clone https://github.com/<your-username>/fast-music-remover.git
```
#### Step 2: Set Up Your Development Environment

After cloning the repository, make sure to install the dependencies as described in the [Getting Started](README.md#getting-started) section of the README. 
Ensure the `config.json` file at the root is properly configured for your system before continuing.
> [!NOTE]
> If you notice anything incorrect or outdated in the docs, please let us know!

Install the Python dependencies with:

```sh
pip install -r requirements.txt
```
#### Step 3: Set Up Pre-Commit Hooks

We use pre-commit hooks to maintain consistency across the project. This will automatically format your code before each commit.
CI will reject poorly formatted code as well as code that won't build. So, please install these hooks to avoid wasting CI resources.

1. Install `pre-commit`:
```sh
pip install pre-commit
```
2. Install the hooks:
```sh
pre-commit install
```
> [!TIP]
> These hooks will automatically run `black` for the backend code in Python and `clang-format` for the `MediaProcessor/src/`, at every commit. 
> 
> For a better experience, enable 'Format on Save' on your IDE, and point to the `.clang-format` file for C++.

#### Step 4: Make Your Changes

As described above, make your changes on the dedicated issue branch (if you don't see one, ask the maintainers to open one for you). 

Here's an example
```sh
git fetch origin
git switch 123-fix-certain-bug-in-core
git add -u
git commit -m "Core: Fix bug in ..."
git push origin 123-fix-certain-bug-in-core
```
> [!IMPORTANT]
> See our [Commit Guidelines](#commit-guidelines) for details on how to write commit messages.

#### Step 5: Submit a Pull Request

Submit a pull request explaining the nature of the changes.

### Pull Request Guidelines:
* **Keep it Modular**: Focus on a singular work units for each PR. This helps keep the review process efficient.
* **Provide Context**: Clearly describe the nature and justification of the changes. If already justified elsewhere on Github, e.g. issues, linking is sufficient.
* **Reference Issues**: Link the underlying issue in your pull request description using `Closes #<issue-number>`.

### Commit Guidelines
Please ensure commit hygeine and use the following specifiers:
* `CI: <commit message>` for changes under the ``.github/workflow` files
* `Docs: <commit message>` for updates to the documentation
* `Core: <commit message>` for changes in `MediaProcessor`
* `Backend: <commit message>` for changes in the backend
* `Frontend: <commit message>` for changes in the frontend
* `Tooling: <commit message>` for changes in any tools we have, such as the project launcher.

Please adhere to the following practices:
* Start your commit with the appropriate specifier as listed above.
* Write your commit messages in imperative form, e.g. `Core: Fix bug in..`, instead of `Core: Fixed bug in..`
* Avoid long headers. Instead give a brief explanation, add a blank line then provide further context where needed. Here the imperative form is not required, and it may be good to explain briefly the nature and justification of the changes.

Here's an example:

```sh
CI: Update image release workflow with multi-arch support.

This updates the GHCR image release workflow to provide the `latest` 
image with multi-architecture support for amd64 and arm64.
```

### Style Guidelines:
- **C++**: Please follow the existing style for C++ files (refer to the .clang-format file).
- **Python**: Please use `black` for formatting Python code.
> [!NOTE]
> These are done automatically if you installed the precommit hooks as described [above](#step-3-set-up-pre-commit-hooks).

### Commenting Guidelines:

#### General Rules:
- **Avoid Redundancy**: Do not add comments for variables, parameters or functions that are self-explanatory. When commenting, focus on why instead of what.
- **High-Level Overviews**: Add comments for public member functions and classes to provide a brief overview of what they do. Include details if the workflow may not be immediately obvious to the reader.
- **Doxygen-style for Public API**: Use Doxygen-style comments (e.g., `@brief`) for public API documentation.
- **Clearly Note Exceptions**: If it's possible for a function to throw, clearly document this in the public API with a `@throws` tag.
  
Here's an example for a public member function:
```cpp
/**
 * @brief Merges the audio and video files into a single output file.
 * 
 * @return true if the merge is successful, false otherwise.
 */
bool mergeMedia();
```

### Got Questions?
Got questions, feedback or a feature request? Don't miss the [discussion](https://github.com/omeryusufyagci/fast-music-remover/discussions), and the [discord server](https://discord.gg/xje3PQTEYp) to get in touch.