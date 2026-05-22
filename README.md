# Social Network Analyzer

A C++ and SFML-based graphical application that visualizes a social network, allows user interaction, and provides friend recommendations based on mutual connections.

## Dependencies

This project requires a specific development environment to be set up on your machine.

1.  **MinGW C++ Compiler**: A 64-bit version of MinGW (like MinGW-w64).
2.  **SFML Library**: The version of SFML that matches your MinGW compiler.

## Setup Instructions

To compile and run this project, you must have the dependencies installed and placed in specific locations that match the project's build configuration.

1.  **Install MinGW**:
    *   Download and install a 64-bit MinGW toolchain.
    *   Ensure it is installed or copied to `C:\MinGW`.
    *   **Important**: Add `C:\MinGW\bin` to your system's `PATH` environment variable. This allows you to run `g++` from any terminal.

2.  **Install SFML**:
    *   Download the SFML version that corresponds to your MinGW compiler.
    *   Unzip or install it to `C:\SFML`.

3.  **Clone the Repository**:
    *   Clone this project from GitHub to a location on your computer.
    ```bash
    git clone <your-repo-url> "C:\Projects\Social Network Suggestion System"
    ```

4.  **Copy Required DLLs**:
    *   Create a new folder named `bin` in the root of your project folder.
    *   After cloning, the project will not run until it has the necessary `.dll` files.
    *   Navigate to your SFML installation's binary folder: `C:\SFML\bin`.
    *   Copy all the `.dll` files from this folder.
    *   Paste them directly into the newly created `bin` folder.

## Building and Running

Once the setup is complete, you can build and run the application in two ways:

### 1. Using Visual Studio Code (Recommended)

The project includes VS Code configuration files (`.vscode/tasks.json` and `.vscode/launch.json`).

*   Open the project folder in VS Code.
*   Simply press **F5**. This will automatically trigger the build task and launch the application with the debugger attached.

### 2. Using the Command Line

*   Open a command prompt (`cmd`) in the root of the project folder.
*   Run the following command to compile the project:
    ```cmd
    g++ -std=c++17 Main.cpp App_Code/*.cpp -I"C:\SFML\include" -L"C:\SFML\lib" -lsfml-graphics -lsfml-window -lsfml-system -o bin\SNA_Run.exe
    ```
*   Once it compiles successfully, run the executable:
    ```cmd
    bin\SNA_Run.exe
    ```