# Vulkan Engine
## Build
___
### MacOS
___
#### Dependencies (the ones that need to be manually installed):
    python@3.X
#### Build Steps:
    1. Clone this repository
    2. Run the python script compileDeps.py in the project root directory
    3. Run the compileShaders.sh shell script
    4. Run scons -j8, command line options are:
        - DBG -- turns off optimizations and puts the executable in ./bin/dbg/tstGame
        - ARM -- no-op (only compile for arm right now)
        - WARN -- turns of -Werror
        - TEST - no-op
### Linux
___
-todo
### Windows
___
-todo