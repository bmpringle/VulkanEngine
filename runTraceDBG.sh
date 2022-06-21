scons -j8 DBG=1
rm -rf traces/out.trace
xcrun xctrace record --template 'Time Profiler' --template 'Metal System Trace' --output 'traces/out.trace' --launch ./bin/dbg/tstGame