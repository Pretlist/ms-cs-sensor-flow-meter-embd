list(APPEND shared_linker_flags
    "-fPIC"
    "-fpic"
    "-pipe"
    "-shared"
    "-Wl,--fatal-warnings"
    # Issue with external build for memory sanitizers
    "-Wl,-z,defs"
    "-Wl,-z,noexecstack"
    "-Wl,-z,now"
    "-Wl,-z,relro"
    "-fthreadsafe-statics"
)

list(APPEND static_linker_flags
    "-fPIC"
    "-fpic"
    "-pipe"
    "-Wl,--fatal-warnings"
    # Issue with external build for memory sanitizers
    "-Wl,-z,defs"
    "-Wl,-z,noexecstack"
    "-Wl,-z,now"
    "-Wl,-z,relro"
    "-fthreadsafe-statics"
    "-static"
    "--static"
)

list(APPEND exe_linker_flags
    "-fPIE"
    "-fpie"
    "-pipe"
    "-fuse-ld=lld"
    "-Wl,--fatal-warnings"
    # Issue with external build for memory sanitizers
    "-Wl,-z,defs"
    "-Wl,-z,noexecstack"
    "-Wl,-z,now"
    "-Wl,-z,relro"
    "-fthreadsafe-statics"
)
