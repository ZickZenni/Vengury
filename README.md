<h1 align="center">Vengury</h1>

A mod menu for Grand Theft Auto V.
Strictly for educational purposes.

It's based on BigBaseV2 and includes updated classes with [GTAClasses](https://github.com/Yimura/GTAV-Classes), later also adding Lua Scripting support.
This project isn't possible without the help of [YimMenu](https://github.com/YimMenu/YimMenu/)!

## Features
* ImGui–based user interface
* Unloading at runtime
* Log console
* Script fibers
* Fiber pool
* Access to YSC script globals
* scrProgram–based native hook

## Building
To build Vengury you need:
* Visual Studio 2022
* [Premake 5.0](https://premake.github.io/download.html) in your PATH

To set up the build environment, run the following commands in a terminal:
```dos
git clone https://github.com/ZickZenni/Vengury.git --recurse-submodules
cd Vengury
GenerateProjects.bat
```
Now, you will be able to open the solution, and simply build it in Visual Studio.
