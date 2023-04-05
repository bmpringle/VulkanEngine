import sys
import subprocess
import os

sys.path += [os.path.realpath(sys.path[0]) + "/scripts"]

from executeCommand import execCmd

APP_NAME = "TEST_APP"
EXECUTABLE_FILE_PATH="./bin/rel/tstGame"
EXECUTABLE_NAME="tstGame"
DEPLOY_FOLDER="./deploy"
PROJECT_BUILD_FOLDER="./"
PROJECT_BUILD_CMD="scons -j8"
PROJECT_CLEAN_CMD="scons -j8 -c; rm -rf deploy/*"
BUILD_TYPE = "T"

def clean_all_projects():
    clean_project(os.path.realpath(sys.path[0]))
    if BUILD_TYPE == "O":
        clean_project(PROJECT_BUILD_FOLDER)

def clean_project(project_dir):
    print(f'Cleaning Project In Directory {project_dir}...')
    execCmd(PROJECT_CLEAN_CMD, cwdOverride = project_dir, out = None)
    print("Done Cleaning")

def build_all_projects():
    build_project(os.path.realpath(sys.path[0]))
    if BUILD_TYPE == "O":
        build_project(PROJECT_BUILD_FOLDER)

def build_project(project_dir):
   print(f'Building Project In Directory {project_dir}...')
   execCmd(PROJECT_BUILD_CMD, cwdOverride = project_dir, out = None)
   print("Done Building Project")

def build_dependencies():
    print("Building Main Project Dependencies ...")
    execCmd(f'{sys.executable} {os.path.realpath(sys.path[0])}/scripts/compileDeps.py', cwdOverride = os.path.realpath(sys.path[0]), out = None)
    print("Done Building Main Project Dependencies")

def compile_shaders():
    print("Compiling Shaders ...")
    execCmd(f'sh {os.path.realpath(sys.path[0])}/scripts/compileShaders.sh', cwdOverride = os.path.realpath(sys.path[0]), out = None)
    print("Done Compiling Shaders")

def post_compile_scripts():
    print("Running Post-Compile Scripts ...")
    print("\nPlease note that install_name_tool will likely give errors on subsquent calls to this script. They are non-fatal and can be safely ignored.\n")
    execCmd(
        f'''
        export APP_NAME={APP_NAME};
        export EXECUTABLE_FILE_PATH={EXECUTABLE_FILE_PATH};
        export EXECUTABLE_NAME={EXECUTABLE_NAME};
        export DEPLOY_FOLDER={DEPLOY_FOLDER}; 
        sh {os.path.realpath(sys.path[0])}/scripts/postCompileCommands.sh
        ''', cwdOverride = os.path.realpath(sys.path[0]), out = None)
    print("Done Running Post-Compile Scripts")

def build_all():
    clean_all_projects()
    build_dependencies()
    build_all_projects()
    compile_shaders()
    post_compile_scripts()

def set_test_project_env():
    print("Setting Test Project Environment ...")
    print("Done Setting Test Project Environment")

def set_other_project_env():
    print("Setting Other Project Environment ...")
    APP_NAME = input("Set app name:")
    EXECUTABLE_FILE_PATH = input("Set executable file path:")
    EXECUTABLE_NAME = input("Set executable name:")
    DEPLOY_FOLDER = input("Set deploy folder:")
    BUILD_TYPE = "O"
    print("Done Setting Other Project Environment")
    
def main():

    build_dictionary = {
        'C': clean_all_projects,
        'B': build_all_projects,
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

    build_type_dictionary[build_type[0]]()

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

        build_dictionary["A"]()
        return

    for option in build_setting:
        build_dictionary[option]()

if __name__ == "__main__":
    main()
    
