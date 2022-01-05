#unfortunately, you will need to manually install the Vulkan SDK since I'm not sure how to automate that since the only 
#was to download it I could find was with their installer for MacOS. Just install it and put the path to it in the 
#VULKAN_HOME variable

import os
import subprocess
from platform import system

DBG = int(ARGUMENTS.get('DBG', 0))
ARM = int(ARGUMENTS.get('ARM', 1))
WARN = int(ARGUMENTS.get('WARN', 0))

TEST = int(ARGUMENTS.get('TEST', 0))

VALIDATION_LAYERS = int(ARGUMENTS.get('VALIDATION_LAYERS', 0))

env = Environment()

CXX='clang++'
GLFW_DIR='./glfw/'

#place path to Vulkan SDK here:
VULKAN_HOME = ""
if system() == "Darwin":
    VULKAN_HOME='./VulkanSDKMacOS'
if system() == "Linux":
    VULKAN_HOME='./VulkanSDKLinux'
if system() == "Windows":
    VULKAN_HOME='./VulkanSDKWindows'


LIBS=['pthread', 'vulkan.1', 'vulkan.1.2.182', 'libMoltenVK.dylib']
LINK='{} -framework OpenGL -framework Cocoa -framework IOKit'.format(CXX)

GLFW_INCLUDE=os.sep.join([GLFW_DIR,'include'])

VULKAN_INCLUDE=os.sep.join([VULKAN_HOME, 'macOS/include'])
MVK_INCLUDE=os.sep.join([VULKAN_HOME, 'MoltenVK/include'])

BLD = 'dbg' if DBG == 1 else 'rel'
OPT = 0 if DBG == 1 else 3

env.Append(CPPPATH = ['include']) 

CCFLAGS='-static -O{} -I {} -I {} -I {} -I {} -I {} -I {} -I {} -I {} -fPIC -Wall -Wpedantic {} -g -std=c++2a -DGLEW_STATIC {} '.format(OPT, './glm/', './include/Engine/', './', './include/', GLFW_INCLUDE, VULKAN_INCLUDE, 'StringToText/freetype/include/', MVK_INCLUDE, '-Werror' if WARN == 0 else '', '-DENABLE_VALIDATION_LAYERS' if VALIDATION_LAYERS == 1 else '')

LIBSSTATIC = Glob(os.sep.join(['lib', '*.a']))

VariantDir(os.sep.join(['obj', BLD]), "src", duplicate=0)

env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME'] = 1

sharedLibBuild = env.SharedLibrary(os.sep.join(['lib', BLD, 'libVulkanEngineLib.dylib']),
                    source=[Glob('{}/VKRenderer.cpp'.format(os.sep.join(['obj', BLD]))), Glob('{}/Engine/*.cpp'.format(os.sep.join(['obj', BLD]))), LIBSSTATIC],
                    CXX=CXX,
                    CCFLAGS=CCFLAGS,
                    LINK=LINK,
                    LIBS=LIBS)


CCFLAGS='-static -O{} -I {} -I {} -I {} -I {} -I {} -I {} -I {} -I {}  -Wall -Wpedantic {} -g -std=c++2a -DGLEW_STATIC {} '.format(OPT, './glm/', './include/Engine/', './', './include/', GLFW_INCLUDE, VULKAN_INCLUDE, 'StringToText/freetype/include/', MVK_INCLUDE, '-Werror' if WARN == 0 else '', '-DENABLE_VALIDATION_LAYERS' if VALIDATION_LAYERS == 1 else '')

LINK='{} -framework OpenGL -framework Cocoa -framework IOKit -L {}'.format(CXX, './lib/{}/'.format(BLD))
LIBS = ['libVulkanEngineLib.dylib']

testExecutableBuild = env.Program(os.sep.join(['bin', BLD, 'tstGame']),
                    source=[Glob('{}/main.cpp'.format(os.sep.join(['obj', BLD])))],
                    CXX=CXX,
                    CCFLAGS=CCFLAGS,
                    LINK=LINK,
                    LIBS=LIBS)

Depends(testExecutableBuild, sharedLibBuild)