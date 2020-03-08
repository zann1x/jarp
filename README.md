# 🌋 jarp

This is Just Another Rendering Project.
The whole purpose of it is to understand vulkan, rendering and all that beautiful stuff a little better and see where we're ending at.

## ✔️ Supported Platforms

- Windows 64-bit
- Ubuntu 18.04

## 🚀 Prerequisites

### Windows

- Visual Studio 2019 with workload "Desktop development with C++" is installed

### Linux

```bash
$ sudo apt install build-essential
```

## 👨‍💻 Getting up and running

### Windows

- Install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#sdk/downloadConfirm/1.2.131.1/windows/VulkanSDK-1.2.131.1-Installer.exe) under `C:\VulkanSDK`.
- Execute `GenerateProjectFiles.bat`.

### Linux

- Install SDL2. 

```bash
$ sudo apt install libsdl2-dev
```

- Install the Vulkan SDK.

```bash
# Vulkan SDK installation
$ wget -qO - http://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo apt-key add -
$ sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.2.131-bionic.list http://packages.lunarg.com/vulkan/1.2.131/lunarg-vulkan-1.2.131-bionic.list
$ sudo apt update
$ sudo apt install vulkan-sdk

# Debugging symbols installation
$ sudo apt install libvulkan1-dbgsym vulkan-tools-dbgsym
```

- Execute `GenerateProjectFiles.sh` to create the Makefiles.

## 🔗 Dependencies

- glm *0.9.9.5*
- stb *@052dce1*
- volk *@453c4de*
- Vulkan SDK *1.2.131*

### Windows

- SDL2 *2.0.9*

### Linux

- SDL2 *2.0.8*
