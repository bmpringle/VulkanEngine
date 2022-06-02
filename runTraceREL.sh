scons -j8
rm -rf traces/out.trace
xcrun xctrace record --template 'Time Profiler' --output 'traces/out.trace' --launch ./bin/rel/tstGame