#! /usr/bin/env python3
import os
import subprocess

def execCmd(cmd, cwdOverride = './'):
    process = subprocess.Popen(shell='true', cwd=cwdOverride, args=cmd, stdout=subprocess.PIPE)
    return process.communicate()

output, error = execCmd('git submodule update --init --recursive')
output, error = execCmd('mkdir -p lib')

try:
    output, error = execCmd('cmake -DBUILD_SHARED_LIBS=OFF .', cwdOverride = './glfw/')
    output, error = execCmd('make', cwdOverride = './glfw/')
    output, error = execCmd('mv ./src/libglfw3.a ../lib/', cwdOverride = './glfw/')
except FileNotFoundError:
    print("Error, cmake needed to build glfw")
    #cmake needed for glfw