#! /usr/bin/env python3
import os
import subprocess
from executeCommand import execCmd
import sys

nextArgIsVertexOutput = False
nextArgIsFragmentOutput = False
nextArgIsVertexInput = False
nextArgIsFragmnetInput = False

vertexInput = ''
fragmentInput = ''

vertexOutput = ''
fragmentOutput = ''

for i, arg in enumerate(sys.argv):
    if i == 0:
        continue
    if(arg == '-vo'):
        nextArgIsVertexOutput = True
        nextArgIsVertexInput = False
        nextArgIsFragmnetInput = False
        nextArgIsFragmentOutput = False
    elif(arg == '-fo'):
        nextArgIsVertexOutput = False
        nextArgIsVertexInput = False
        nextArgIsFragmnetInput = False
        nextArgIsFragmentOutput = True
    elif(arg == '-v'):
        nextArgIsVertexInput = True
        nextArgIsVertexOutput = False
        nextArgIsFragmnetInput = False
        nextArgIsFragmentOutput = False
    elif(arg == '-f'):
        nextArgIsFragmentInput = True
        nextArgIsVertexOutput = False
        nextArgIsVertexInput = False
        nextArgIsFragmentOutput = False
    else:
        if(nextArgIsVertexOutput):
            vertexOutput = arg
        elif(nextArgIsFragmentOutput):
            fragmentOutput = arg
        elif(nextArgIsVertexInput):
            vertexInput = arg
        elif(nextArgIsFragmentInput):
            fragmentInput = arg
        else:
            print('you passed an invalid argument, ' + arg + ', ignoring it')

        nextArgIsVertexOutput = False
        nextArgIsVertexInput = False
        nextArgIsFragmnetInput = False
        nextArgIsFragmentOutput = False

if(vertexInput == ''):
    print('vertexInput not set, exiting\nnote: vertexInput can be set with -v')
if(vertexOutput == ''):
    print('vertexOutput not set, exiting\nnote: vertexOutput can be set with -vo')
if(fragmentInput == ''):
    print('fragmentInput not set, exiting\nnote: fragmentInput can be set with -f')
if(fragmentOutput == ''):
    print('fragmentOutput not set, exiting\nnote: fragmentOutput can be set with -fo')



VulkanSDKDir = '../VulkanSDK/'

output, error = execCmd(VulkanSDKDir + 'macOS/bin/glslc ' + vertexInput + ' -o ' + vertexOutput)
output, error = execCmd(VulkanSDKDir + 'macOS/bin/glslc ' + fragmentInput + ' -o ' + fragmentOutput)