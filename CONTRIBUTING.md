# Contributing to Fast Music Remover

Thank you for considering contributing to this project! Your contributions are invaluable in providing a free and open tool that aims to address a wide set of use cases.

## How You Can Contribute

### 1. Issues:
You can quickly report most issues with the [issue templates](https://github.com/omeryusufyagci/fast-music-remover/issues/new/choose) for the following types:
* **Bug Report**: If you encounter unintended behavior, please report a bug to help us improve!
* **Feature Request**: This project is being built for you! If you see something that is missing, suggest a new feature!
* **Improvement Request**: Something can be improved? Written better, run faster? Suggest an improvement!

If these do not cover your specific case, you're welcome to open a custom issue.

### 2. Code Contributions:
* **Check the Open Issues**: Have a look at the [issues](https://github.com/omeryusufyagci/fast-music-remover/issues), especially those tagged `good first issue`, which are easier to get started with and help you understand the general workflow. 
* **Suggest Improvements**: If you see an area that can be extended or refactored, or if there's a feature you'd like to add, please open an issue or submit a pull request.

### 3. Testing
* **Manual Testing**: Run the software and report any bugs or unintended behavior you encounter. This is incredibly valuable, especially before releases.
* **Automated Testing**: Write new tests to improve code coverage or enhance the robustness of existing tests.

### 4. Documentation:
* If you find a piece of documentation missing or unclear, please consider improving it.

### 5. Outreach
* **Spread the Word**: Share `Fast Music Remover` with others who might benefit from it or want to contribute.
* **Discuss Features and Ideas**: Join the [discussions](https://github.com/omeryusufyagci/fast-music-remover/discussions) to talk about new features, feedback, or questions. It’s a great way to shape the direction of the project.

## How to Contribute

### Getting Started:

#### Step 1: Fork and Clone the Repository

Fork the repository and clone your fork:
```sh
git clone https://github.com/<your-username>/fast-music-remover.git
```
#### Step 2: Set Up Your Development Environment

After cloning the repository, make sure to install the necessary dependencies as described in the [Getting Started](README.md#getting-started) section of the README.

You can use Docker for a quick setup or install the prerequisites manually to run the server locally. For Python dependencies, use:

```sh
pip install -r requirements.txt
```
#### Step 3: Set Up Pre-Commit Hooks

To ensure code quality and maintain consistency across the project, set up pre-commit hooks. This will automatically format and lint your code before each commit.

1. Install `pre-commit`:
```sh
pip install pre-commit
```
2. Install the hooks:
```sh
pre-commit install
```

> **Note**: These hooks will automatically run `black` for the backend code in Python and clang-format for the `MediaProcessor/src/`, before every commit.

#### Step 4: Make Your Changes
Open a new branch for your changes and commit them:
```sh
git checkout -b feature/your-feature
git commit -m "Feature: Add feature for ..."
git push origin feature/your-feature
```
> **Note**: If your pull request is going to address an open issue, please use the `Generate Branch` option on the issue page to open your branch. This helps maintain traceability between issues and branches.

#### Step 5: Submit a Pull Request

Submit a pull request explaining the nature of the changes. 

### Pull Request Guidelines:
* **Keep it Modular**: Focus on a singular unit of work for each pull request, whether it's a new feature or a bug fix. This helps make the review process quicker and more efficient.
* **Write a Detailed Description**: Clearly describe what changes have been made and why they are necessary.
* **Reference Issues**: If your pull request addresses an open issue, link to it in the pull request description using `Closes #issue-number`.

### Style Guidelines:
- **Python**: Please use black for formatting Python code. This is automatically done if you have set up the pre-commit hooks.
- **C++**: Please follow the existing style for C++ files (refer to the .clang-format file). The pre-commit hooks will automatically run clang-format on `MediaProcessor/src`.
- **Keep Changes Modular**: Focus on singular units of work instead of larger pull requests, allowing for easier and faster progression.

### Commenting Guidelines:

#### General Rules:
- **Avoid Redundancy**: Do not add comments for variables, parameters or methods that are self-explanatory.
- **High-Level Overviews**: Add comments for public methods and classes to provide a brief overview of what they do. Include details if the method performs multiple steps or has a more complex workflow.
- **Minimal Private Method Comments**: Avoid commenting on private methods or member variables unless they perform non-obvious functions.
- **Doxygen Style for Public Methods**: Use Doxygen-style comments (e.g., `@brief`) for class-level and public method documentation.
  
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
Got questions, feedback or a feature request? Don't miss the [discussion](https://github.com/omeryusufyagci/fast-music-remover/discussions) to get in touch!