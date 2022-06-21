import os
import subprocess
from platform import system
import scons_compiledb
import distutils.ccompiler

DBG = int(ARGUMENTS.get('DBG', 0))
LIB_DBG = int(ARGUMENTS.get('LIB_DBG', 0))
ARM = int(ARGUMENTS.get('ARM', 1))
WARN = int(ARGUMENTS.get('WARN', 0))
SANITIZE_MEM = int(ARGUMENTS.get('SANITIZE_MEM', 0))
TEST = int(ARGUMENTS.get('TEST', 0))

VALIDATION_LAYERS = int(ARGUMENTS.get('VALIDATION_LAYERS', 0))

env = Environment()

scons_compiledb.enable(env);

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
VULKAN_INCLUDE=''
MVK_INCLUDE=''

if system() == 'Darwin':
    LIBS = [['pthread', 'vulkan', 'libMoltenVK.dylib']]
    LINK = '{} {} -framework OpenGL -framework Cocoa -framework IOKit -L {}'.format(CXX, '-fsanitize=address' if SANITIZE_MEM == 1 else '', os.sep.join([VULKAN_HOME, 'lib']))
    VULKAN_INCLUDE=os.sep.join([VULKAN_HOME, 'macOS/include'])
    MVK_INCLUDE=os.sep.join([VULKAN_HOME, 'MoltenVK/include'])
elif system() == 'Linux':
    LIBS = [['pthread', 'vulkan']]
    LINK = '{} {}'.format(CXX, '-fsanitize=address -L {}' if SANITIZE_MEM == 1 else '', os.sep.join([VULKAN_HOME, 'lib']))
    VULKAN_INCLUDE = os.sep.join([VULKAN_HOME, 'x86_64/include'])

GLFW_INCLUDE=os.sep.join([GLFW_DIR,'include'])

BLD = 'dbg' if DBG == 1 else 'rel'
OPT = 0 if DBG == 1 else 3

env.Append(CPPPATH = ['include']) 

CCFLAGS='-static -O{} -I {} -I {} -I {} -I {} -I {} -I {} -I {} -I {} -fPIC -Wall -Wpedantic {} -g -std=c++2a -DGLEW_STATIC {} {}'.format(OPT, './glm/', './include/Engine/', './', './include/', GLFW_INCLUDE, VULKAN_INCLUDE, 'StringToText/freetype/include/', MVK_INCLUDE, '-Werror' if WARN == 0 else '', '-DENABLE_VALIDATION_LAYERS' if VALIDATION_LAYERS == 1 else '', '-fsanitize=address' if SANITIZE_MEM == 1 else '')

LIBSSTATIC = Glob(os.sep.join(['lib', '*.a']))

VariantDir(os.sep.join(['obj', BLD]), "src", duplicate=0)

env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME'] = 1

sharedLibBuild = env.SharedLibrary(os.sep.join(['lib', BLD, 'libVulkanEngineLib' + distutils.ccompiler.new_compiler().shared_lib_extension]),
                    source=[Glob('{}/VKRenderer.cpp'.format(os.sep.join(['obj', BLD]))), Glob('{}/Engine/*.cpp'.format(os.sep.join(['obj', BLD]))), LIBSSTATIC],
                    CXX=CXX,
                    CCFLAGS=CCFLAGS,
                    LINK=LINK,
                    LIBS=LIBS)


CCFLAGS='-static -O{} -I {} -I {} -I {} -I {} -I {} -I {} -I {} -I {}  -Wall -Wpedantic {} -g -std=c++2a -DGLEW_STATIC {} '.format(OPT, './glm/', './include/Engine/', './', './include/', GLFW_INCLUDE, VULKAN_INCLUDE, 'StringToText/freetype/include/', MVK_INCLUDE, '-Werror' if WARN == 0 else '', '-DENABLE_VALIDATION_LAYERS' if VALIDATION_LAYERS == 1 else '')

LIBS = ['VulkanEngineLib']

if system() == 'Darwin':
    LINK='{} -framework OpenGL -framework Cocoa -framework IOKit -L {} -L {}'.format(CXX, './lib/{}/'.format(BLD), os.sep.join([VULKAN_HOME, 'lib']))
elif system() == 'Linux':
    LINK='{} -L {} -L {}'.format(CXX, './lib/{}/'.format(BLD), os.sep.join([VULKAN_HOME, 'lib']))

testExecutableBuild = env.Program(os.sep.join(['bin', BLD, 'tstGame']),
                    source=[Glob('{}/main.cpp'.format(os.sep.join(['obj', BLD])))],
                    CXX=CXX,
                    CCFLAGS=CCFLAGS,
                    LINK=LINK,
                    LIBS=LIBS)

Depends(testExecutableBuild, sharedLibBuild)

env.CompileDb()