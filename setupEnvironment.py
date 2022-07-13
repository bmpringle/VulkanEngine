#! /usr/bin/env python3
import os
import subprocess
from executeCommand import execCmd

from platform import system

output, error = execCmd("source setup-env.sh", "./VulkanSDKMacOS" if system() == "Darwin" else "./VulkanSDKLinux")