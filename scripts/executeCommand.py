#! /usr/bin/env python3
import os
import subprocess

def execCmd(cmd, cwdOverride = './', out = subprocess.PIPE):
    process = subprocess.Popen(shell='true', cwd=cwdOverride, args=cmd, stdout=out)
    return process.communicate()