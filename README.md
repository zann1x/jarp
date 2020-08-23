# 🌋 jarp

This is Just Another Rendering Project.
The whole purpose of it is to understand vulkan, rendering and all that beautiful stuff a little better and see where we're ending at.

## ✔️ Supported Platforms

- Windows 64-bit (MSVC)

## 🚀 Prerequisites

### Windows

- Visual Studio 2019 with workload "Desktop development with C++"
- CMake

## 👨‍💻 Getting up and running

### Windows

- Install the [Vulkan SDK 1.2.131](https://vulkan.lunarg.com/sdk/home#sdk/downloadConfirm/1.2.131.2/windows/VulkanSDK-1.2.131.2-Installer.exe) under `C:\VulkanSDK\1.2.131\`.
- Open a command line window in the folder `jarp/ThirdParty/assimp` and execute `cmake CMakeLists.txt`. Open the generated Visual Studio solution and build the project *assimp*.
  (Actually, only the needed files are commited here, so  this step should be unnecessary. It is included here just for future reference when we actually need to build that from scratch again.)
- Open the jarp project folder in Visual Studio, build the project and set the startup target to jarp.exe.

## 🔗 Dependencies

- assimp *5.0.1*
- glm *0.9.9.5*
- stb *@052dce1*
- volk *@453c4de*
- Vulkan SDK *1.2.131*

### Windows

- SDL2 *2.0.9*
