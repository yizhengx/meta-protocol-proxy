cc_library(
    name = "bsoncxx",
    srcs = glob([
        "src/bsoncxx/*.cpp",          # Main source files
        "src/bsoncxx/builder/*.cpp",  # Include specific subdirectories as needed
        "src/bsoncxx/array/*.cpp",
        "src/bsoncxx/json/*.cpp",
        # Add other relevant source files here
    ]),
    hdrs = glob([
        "include/bsoncxx/*.hpp",            # Main header files
        "include/bsoncxx/builder/*.hpp",    # Builder headers
        "include/bsoncxx/array/*.hpp",      # Array headers
        "include/bsoncxx/json/*.hpp",       # JSON headers
        # Include other relevant header files
    ]),
    includes = [
        "include",  # This allows to use <bsoncxx/header.hpp> in your code
        "src",      # Optional: Include source path if needed
    ],
    visibility = ["//visibility:public"],
)
