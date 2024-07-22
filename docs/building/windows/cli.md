Building from the Command Line on Windows
=========================================

Preparing your Windows build host
-------------------------------

### Microsoft Visual C++ compiler

You have two options for obtaining the MSVC compiler.

#### Option 1: Visual Studio

* Install Visual Studio and then C/C++ developer tools as described in [Bulding with Visual Studio on Windows](visualstudio.md).

#### Option 2: Standalone build tools

* Download and launch the installer for [Build Tools for Visual Studio](https://aka.ms/vs/17/release/vs_BuildTools.exe) from [Microsoft Visual Studio Downloads](https://visualstudio.microsoft.com/downloads/#remote-tools-for-visual-studio-2022).

* In the **[Workloads]** tab, check **Desktop development with C++ [ ]**.  You can review the individual components that will get installed in the right pane.
* Click **[Install]**.


### Developer Command Prompt

* Create a *Developer Command Prompt* shortcut in your taskbar:
   - Click on **[Start]**
   - Start typing `"Developer"`, until you see `"Developer Command Prompt for VS 2022"` among the choices.
   - Right-click on this item *or* click on the expander arrow to its right to get a submenu of additional choices
   - Click on **[Pin to taskbar]**

* Adjust the startup folder
   - Right-click on the new **Developer Command Prompt for VS 2022** shortcut, then again on its title (the first item in the pop-up menu), and finally left-click **Properties**
   - Next to **Start in:**, type in the location of your top-level working directory (e.g. `C:\Projects`).

* Try it out
  - Click on the new shortcut.
  - Verify that it opens up in the correct folder
  - Try the following commands:
     - `cmake`
     - `cl`


Building from the command line
------------------------------

From the Developer Command Prompt, navigate to the top-level project folder (`common-core`). From there:

* Generate Visual C++ build files using CMake:

  ```
  cmake -B out\build\windows-default
  ```

* Build the code

  ```
  cmake --build out\build\windows-default --parallel %NUMBER_OF_PROCESSORS%
  ```

* Install

  ```
  cmake --install out\build\windows-default --prefix C:\Apps\Common-Core --config Debug --strip
  ```
