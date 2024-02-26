Building with Visual Studio Code on Windows
===========================================

Visual Studio Code is a lightweight code editor and alternative to the full Visual Studio suite. Its main advantages over Visual Studio are:

- Lighter, quicker
- Avialable also for Linux
- Integrates with third-party toolchains

However it does not supply any toolchains on its own. Here we'll use the Microsoft C++ compiler tools provided with Visual Studio.


Preparing your Windows build host
-------------------------------

### Microsoft Visual C++ compiler and Developer Command Prompt

Follow preparation instructions as described in [Building from the command line on Windows](cli.md).

### Visual Studio Code

* Download and install Code from [here](https://code.visualstudio.com/download).

* **Important**: To access the MSVC++ toolchain from Code, you must launch it by typing `code` from within the *Developer Command Prompt* you installed above.

### CMake and C++ Support

* From within Code, click on the **[Extensions]** button on the left side bar

* Search for and install `"C/C++ Extension Pack"` by Microsoft. This will additionally install the following extensions:
   * `C/C++`
   * `C/C++ Themes`
   * `CMake`
   * `CMake Tools`

* Configure the CMake extension:
  * Click on the cog wheel at the bottom of the left side bar, and select **Settings**
  * Navigate to **Extensions → CMake Tools**
  * Edit the following values
     - **CMake: Build Directory**: `${workspaceFolder}/out/build/windows-default`
     - **CMake: Configure on Open**: Choose whether you want CMake to automatically generate build files each time you open a project.  Uncheck this for quicker startup.
     - **CMake: Parallel Jobs**: Enter the number of virtual processor cores on your computer. (You can find this number by typing `echo %NUMBER_OF_PROCESSORS%` in the Developer Command Prompt).
       - **Note**: There is also an option earlier in the list titled **CMake - Ctest: Parallel jobs**.  You can edit this as well, but the pertinent one here is **CMake: Parallel Jobs**.


Building from VS Code
---------------------

### Open the project folder

- Choose **Open Folder** from the **Code Explorer** or **File** menu, or press **[Ctrl]+[K]**, **[Ctrl]+[O]**
- Navigate to the top-level project folder (e.g., `C:\Projects\common-core`), and click **[Select Folder]**


Questions? Comments? Concerns?
------------------------------

Contact:
    Tor Slettnes  
    Email: [tor@slett.net](mailto:tor@slett.net)  
    Phone: [+1-925-8888-TOR](tel:+19258888867)
