import sys
import subprocess

sys.path += ["./scripts"]

from executeCommand import execCmd

def clean_main_project():
    print("Cleaning Main Project ...")
    execCmd("scons -j8 -c", out = None)
    execCmd("rm -rf deploy/*")
    print("Done Cleaning")

def build_main_project():
   print("Building Main Project ...")
   execCmd("scons -j8", out = None)
   print("Done Building Main Project")

def build_dependencies():
    print("Building Main Project Dependencies ...")
    execCmd(f'{sys.executable} scripts/compileDeps.py', out = None)
    print("Done Building Main Project Dependencies")

def compile_shaders():
    print("Compiling Shaders ...")
    execCmd("sh scripts/compileShaders.sh", out = None)
    print("Done Compiling Shaders")

def post_compile_scripts():
    print("Running Post-Compile Scripts ...")
    print("\nPlease note that install_name_tool will likely give errors on subsquent calls to this script. They are non-fatal and can be safely ignored.\n")
    execCmd("sh scripts/postCompileCommands.sh", out = None)
    print("Done Running Post-Compile Scripts")

def build_all():
    clean_main_project()
    build_dependencies()
    build_main_project()
    compile_shaders()
    post_compile_scripts()

def main():
    build_setting = ""

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

    build_dictionary = {
        'C': clean_main_project,
        'B': build_main_project,
        'D': build_dependencies,
        'S': compile_shaders,
        'P': post_compile_scripts,
        'A': build_all

    }

    if 'A' in build_setting:
        if len(build_setting) > 1:
            print("Since the \'A\' option was passed, all other options are ignored")

        build_dictionary["A"]()
        return

    for option in build_setting:
        build_dictionary[option]()

if __name__ == "__main__":
    main()
    
