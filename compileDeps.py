#! /usr/bin/env python3
import os
import subprocess
from executeCommand import execCmd

from platform import system

if system() == "Linux" or system() == "Darwin":
    output, error = execCmd('git submodule update --init --recursive')
    output, error = execCmd('mkdir -p lib')

    try:
        output, error = execCmd('cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON .', cwdOverride = './glfw/')
        output, error = execCmd('make', cwdOverride = './glfw/')
        output, error = execCmd('mv ./src/libglfw3.a ../lib/', cwdOverride = './glfw/')
    except FileNotFoundError:
        print("Error, cmake needed to build glfw")
        #cmake needed for glfw

    output, error = execCmd('python3 compileDeps.py PIC=ON', cwdOverride = './StringToText/')
    output, error = execCmd('scons -j8 PIC=1', cwdOverride = './StringToText/')
    output, error = execCmd('cp -a ./bin/libStringToText.a ../lib/', cwdOverride = './StringToText/')

    output, error = execCmd('pip install scons-compiledb')
elif system() == "Windows":
    print("unsupported at this time")
    exit()

output, error = execCmd('mkdir -p shaders/output')

MACOS_VULKANSDK = "https://sdk.lunarg.com/sdk/download/1.2.198.1/mac/vulkansdk-macos-1.2.198.1.dmg"
MACOS_DMGNAME= "vulkansdk-macos-1.2.198.1.dmg"
LINUX_VULKANSDK = "https://sdk.lunarg.com/sdk/download/1.2.198.1/linux/vulkansdk-linux-x86_64-1.2.198.1.tar.gz"
LINUX_TARNAME = "vulkansdk-linux-x86_64-1.2.198.1.tar.gz"
WINDOWS_VULKANSDK = "https://sdk.lunarg.com/sdk/download/1.2.198.1/windows/VulkanSDK-1.2.198.1-Installer.exe"

if system() == "Darwin":
    output, error = execCmd("wget {}".format(MACOS_VULKANSDK))
    output, error = execCmd("hdiutil attach {}".format(MACOS_DMGNAME))
    output, error = execCmd("cp -rf /Volumes/{}/ ./".format(MACOS_DMGNAME.replace(".dmg", "")))
    output, error = execCmd("hdiutil detach /Volumes/{}/".format(MACOS_DMGNAME.replace(".dmg", "")))
    output, error = execCmd("rm {}".format(MACOS_DMGNAME))

    print("Install Vulkan SDK to ./VulkanSDKMacOS")

    output, error = execCmd("open InstallVulkan.app")
    
    output, error = execCmd("source setup-env.sh", "./VulkanSDKMacOS")

elif system() == "Linux":
    output, error = execCmd("wget {}".format(LINUX_VULKANSDK))
    output, error = execCmd("mkdir -p ./VulkanSDKLinux")
    output, error = execCmd("tar -xvzf {} -C {} --strip-components 1".format(LINUX_TARNAME, "./VulkanSDKLinux"))
    output, error = execCmd("rm {}".format(LINUX_TARNAME))

    output, error = execCmd("source setup-env.sh", "./VulkanSDKLinux")
elif system() == "Windows":
    output, error = execCmd("curl.exe -o WindowsInstallSDK.exe ()".format(WINDOWS_VULKANSDK))

    print("Install Vulkan SDK to ./VulkanSDKWindows")

    output, error = execCmd("WindowsInstallSDK.exe")
else:
    print("auto-install of vulkan sdk unsupported at this moment for platforms other than MacOS/Linux/Windows")
    print("\tyour system is: " + system())
    exit()
    