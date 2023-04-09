import os
import subprocess
from platform import system
import scons_compiledb
import distutils.ccompiler

DBG = int(ARGUMENTS.get('DBG', 0))
ARM = int(ARGUMENTS.get('ARM', 1))
WARN = int(ARGUMENTS.get('WARN', 0))
SANITIZE_MEM = int(ARGUMENTS.get('SANITIZE_MEM', 0))
TEST = int(ARGUMENTS.get('TEST', 0))
RELEASE = int(ARGUMENTS.get('RELEASE', 0))

VALIDATION_LAYERS = int(ARGUMENTS.get('VALIDATION_LAYERS', 0))

env = Environment(RPATH = './')

scons_compiledb.enable(env)

CXX='clang++'
GLFW_DIR='./glfw/'

VULKAN_HOME = ""
if system() == "Darwin":
    VULKAN_HOME='./VulkanSDKMacOS'
if system() == "Linux":
    VULKAN_HOME='./VulkanSDKLinux'
if system() == "Windows":
    VULKAN_HOME='./VulkanSDKWindows'


LIBS= []
LINK=''
VULKAN_INCLUDE='.'
MVK_INCLUDE='.'

if system() == 'Darwin':
    LIBS = [['pthread', 'vulkan', 'libMoltenVK.dylib']]
    LINK = (f'{CXX} {"-fsanitize=address" if SANITIZE_MEM == 1 else ""} -framework OpenGL -framework Cocoa '
            f'-framework IOKit -L {os.sep.join([VULKAN_HOME, "macOS", "lib"])}')
    VULKAN_INCLUDE=os.sep.join([VULKAN_HOME, 'macOS/include'])
    MVK_INCLUDE=os.sep.join([VULKAN_HOME, 'MoltenVK/include'])
elif system() == 'Linux':
    LIBS = [['pthread', 'vulkan']]
    LINK = (f'{CXX} {"-fsanitize=address" if SANITIZE_MEM == 1 else ""} '
           f'-L {os.sep.join([VULKAN_HOME, "macOS", "lib"])}')
    VULKAN_INCLUDE = os.sep.join([VULKAN_HOME, 'x86_64/include'])

GLFW_INCLUDE=os.sep.join([GLFW_DIR,'include'])

BLD = 'dbg' if DBG == 1 else 'rel'
OPT = 0 if DBG == 1 else 3

env.Append(CPPPATH = ['include']) 

CCFLAGS = (f"-static -O{OPT} -I ./glm/ -I ./include/Engine/ -I ./ -I ./include/ "
           f"-I {GLFW_INCLUDE} -I {VULKAN_INCLUDE} -I StringToText/freetype/include/ -I {MVK_INCLUDE} "
           f"-I ./StringToText/include/ -I ./assimp/include {'-Werror' if WARN == 0 else ''} "
           f"-fPIC -Wall -Wpedantic {'-DRELEASE' if RELEASE == 1 else ''} -g -std=c++2a -DGLEW_STATIC "
           f"{'-DENABLE_VALIDATION_LAYERS' if VALIDATION_LAYERS == 1 else ''} "
           f"{'-fsanitize=address' if SANITIZE_MEM == 1 else ''}")

LIBSSTATIC = Glob(os.sep.join(['lib', '*.a']))

VariantDir(os.sep.join(['obj', BLD]), "src", duplicate=0)

env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME'] = 1

shared_lib_extension = (".dylib" if system() == "Darwin" else ".so" if system() == "Linux" else ".dll")
shared_lib_name = os.sep.join(['bin', BLD, 'libVulkanEngineLib' + shared_lib_extension])

sharedLibBuild = env.SharedLibrary(shared_lib_name,
    source = [
        Glob(f'{os.sep.join(["obj", BLD])}/*.cpp', exclude=[f'{os.sep.join(["obj", BLD])}/main.cpp']), 
        Glob(f'{os.sep.join(["obj", BLD])}/Engine/*.cpp'), LIBSSTATIC],
    CXX = CXX,
    CCFLAGS = CCFLAGS,
    LINK = LINK,
    LIBS = LIBS)

CCFLAGS.replace(" -fPIC", "")

LIBS = ['VulkanEngineLib']

if system() == 'Darwin':
    LINK=f'{CXX} -framework OpenGL -framework Cocoa -framework IOKit -L {f"./bin/{BLD}/"}'
elif system() == 'Linux':
    LINK=f'{CXX} -L {f"./bin/{BLD}/"} -L {os.sep.join([VULKAN_HOME, "lib"])}'

testExecutableBuild = env.Program(os.sep.join(['bin', BLD, 'tstGame']),
                    source=[Glob(f'{os.sep.join(["obj", BLD])}/main.cpp')],
                    CXX=CXX,
                    CCFLAGS=CCFLAGS,
                    LINK=LINK,
                    LIBS=LIBS)

Depends(testExecutableBuild, sharedLibBuild)

env.CompileDb()