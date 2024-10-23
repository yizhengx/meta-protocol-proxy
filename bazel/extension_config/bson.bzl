load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Load the BSON library or MongoDB C++ Driver as an external dependency
http_archive(
    name = "mongo_cxx_driver",
    urls = ["https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.6/mongo-cxx-driver-r3.6.6.tar.gz"],
    strip_prefix = "mongo-cxx-driver-r3.6.6",
)