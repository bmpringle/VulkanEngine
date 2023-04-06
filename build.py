import sys
import subprocess
import os

sys.path += [os.path.realpath(sys.path[0]) + "/scripts"]

from executeCommand import execCmd

class CompileData:
    APP_NAME = "TEST_APP"
    EXECUTABLE_FILE_PATH=f'{os.path.realpath(sys.path[0])}/bin/rel/tstGame'
    EXECUTABLE_NAME="tstGame"
    DEPLOY_FOLDER=f'{os.path.realpath(sys.path[0])}/deploy'

    ENGINE_BUILD_FOLDER=f"{os.path.realpath(sys.path[0])}"
    ENGINE_BUILD_CMD="scons -j8"
    ENGINE_CLEAN_CMD="scons -j8 -c; rm -rf deploy/*"

    OTHER_BUILD_CMD=""
    OTHER_CLEAN_CMD=""
    OTHER_BUILD_FOLDER=""

    BUILD_TYPE = "T"

def clean_project(data: CompileData):
    print(f'Cleaning Project ...')
    execCmd(data.ENGINE_CLEAN_CMD, cwdOverride = os.path.realpath(sys.path[0]), out = None)

    if data.BUILD_TYPE == "O":
        execCmd(data.OTHER_CLEAN_CMD, cwdOverride = data.OTHER_BUILD_FOLDER, out = None)
    print("Done Cleaning")

def build_project(data: CompileData):
   print(f'Building Project ...')
   execCmd(data.ENGINE_BUILD_CMD, cwdOverride = os.path.realpath(sys.path[0]), out = None)
   if data.BUILD_TYPE == "O":
        execCmd(data.OTHER_BUILD_CMD, cwdOverride = data.OTHER_BUILD_FOLDER, out = None)
   print("Done Building Project")

def build_dependencies(data: CompileData):
    print("Building Main Project Dependencies ...")
    execCmd(f'{sys.executable} {os.path.realpath(sys.path[0])}/scripts/compileDeps.py', cwdOverride = os.path.realpath(sys.path[0]), out = None)
    print("Done Building Main Project Dependencies")

def compile_shaders(data: CompileData):
    print("Compiling Shaders ...")
    execCmd(f'sh {os.path.realpath(sys.path[0])}/scripts/compileShaders.sh', cwdOverride = os.path.realpath(sys.path[0]), out = None)
    print("Done Compiling Shaders")

def post_compile_scripts(data: CompileData):
    print("Running Post-Compile Scripts ...")
    print("\nPlease note that install_name_tool will likely give errors on subsquent calls to this script. They are non-fatal and can be safely ignored.\n")
    execCmd(
        f'''
        export APP_NAME={data.APP_NAME};
        export EXECUTABLE_FILE_PATH={data.EXECUTABLE_FILE_PATH};
        export EXECUTABLE_NAME={data.EXECUTABLE_NAME};
        export DEPLOY_FOLDER={data.DEPLOY_FOLDER}; 
        sh {os.path.realpath(sys.path[0])}/scripts/postCompileCommands.sh
        ''', cwdOverride = os.path.realpath(sys.path[0]), out = None)
    print("Done Running Post-Compile Scripts")

def build_all(data: CompileData):
    clean_project(data)
    build_dependencies(data)
    build_project(data)
    compile_shaders(data)
    post_compile_scripts(data)

def set_test_project_env(data: CompileData):
    print("Setting Test Project Environment ...")
    print("Done Setting Test Project Environment")

def set_other_project_env(data: CompileData):
    print("Setting Other Project Environment ...")
    data.APP_NAME = input("Set app name:")
    data.EXECUTABLE_FILE_PATH = input("Set executable file path:")
    data.EXECUTABLE_NAME = input("Set executable name:")
    data.DEPLOY_FOLDER = input("Set deploy folder:")
    data.OTHER_BUILD_CMD = input("Set other project build command:")
    data.OTHER_CLEAN_CMD = input("Set other project clean command:")
    data.OTHER_BUILD_FOLDER = input("Set other project build folder:")

    data.BUILD_TYPE = "O"
    
    data.EXECUTABLE_FILE_PATH = os.path.abspath(data.EXECUTABLE_FILE_PATH)
    data.DEPLOY_FOLDER = os.path.abspath(data.DEPLOY_FOLDER)
    data.OTHER_BUILD_FOLDER = os.path.abspath(data.OTHER_BUILD_FOLDER)
    print("Done Setting Other Project Environment")
    
def main():
    data = CompileData()

    build_dictionary = {
        'C': clean_project,
        'B': build_project,
        'D': build_dependencies,
        'S': compile_shaders,
        'P': post_compile_scripts,
        'A': build_all
    }

    build_type_dictionary = {
        'T': set_test_project_env,
        'O': set_other_project_env
    }

    build_type = input(
        '''
        Build Type:
        Do you want to target the test project or do you want to target your own project using this engine?
        \tT - Test Project
        \tO - Other Project
        '''
    )

    if len(build_type) == 0:
        print("Error: no build type selected. Exiting Build Script ...")
        exit()

    if len(build_type) > 1:
        print("Warning: Only the first option passed will be used, all subsequent ones will be ignored")

    build_type_dictionary[build_type[0]](data)

    build_setting = input(
        '''
        Build Script Start:
        \tC - cleans main project
        \tB - builds main project
        \tD - builds main project dependencies'
        \tS - compiles main project shaders'
        \tP - runs post-compile and packaging scripts'
        \tA - runs all build commands in order'
        '''
    )

    if 'A' in build_setting:
        if len(build_setting) > 1:
            print("Since the \'A\' option was passed, all other options are ignored")

        build_dictionary["A"](data)
        return

    for option in build_setting:
        build_dictionary[option](data)

if __name__ == "__main__":
    main()
    
