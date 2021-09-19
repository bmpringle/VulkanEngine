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