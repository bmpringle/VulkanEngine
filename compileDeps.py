#! /usr/bin/env python3
import os
import subprocess
from executeCommand import execCmd

from sys import platform

if platform == "linux" or platform == "darwin":
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
    output, error = execCmd('make library-POC -j8', cwdOverride = './StringToText/')
    output, error = execCmd('cp -a ./lib/. ../lib/', cwdOverride = './StringToText/')
elif platform == "win32":
    print("unsupported at this time")
    exit()

MACOS_VULKANSDK = "https://sdk.lunarg.com/sdk/download/1.2.189.0/mac/vulkansdk-macos-1.2.189.0.dmg"

if platform == "darwin":
    output, error = execCmd("wget {}".format(MACOS_VULKANSDK))
    output, error = execCmd("hdiutil attach {}".format("vulkansdk-macos-1.2.189.0.dmg"))
    output, error = execCmd("cp -rf /Volumes/{}/ ./".format("vulkansdk-macos-1.2.189.0"))
    output, error = execCmd("hdiutil detach /Volumes/{}/".format("vulkansdk-macos-1.2.189.0"))
    output, error = execCmd("rm {}".format("vulkansdk-macos-1.2.189.0.dmg"))

    print("Install Vulkan SDK to whatever directory you want, then in the Sconstruct file,\nset the VULKAN_HOME variable to be the path you installed the SDK to. The default is ../VulkanSDK in the Sconstruct file")

    output, error = execCmd("open InstallVulkan.app")
else:
    print("auto-install of vulkan sdk unsupported at this moment for non-darwin platforms")
    exit()