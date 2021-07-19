#! /usr/bin/env python3
import os
import subprocess
from executeCommand import execCmd

output, error = execCmd('git submodule update --init --recursive')
output, error = execCmd('mkdir -p lib')

try:
    output, error = execCmd('cmake -DBUILD_SHARED_LIBS=OFF .', cwdOverride = './glfw/')
    output, error = execCmd('make', cwdOverride = './glfw/')
    output, error = execCmd('mv ./src/libglfw3.a ../lib/', cwdOverride = './glfw/')
except FileNotFoundError:
    print("Error, cmake needed to build glfw")
    #cmake needed for glfw