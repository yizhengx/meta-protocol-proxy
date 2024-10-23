load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Load the BSON library or MongoDB C++ Driver as an external dependency
http_archive(
    name = "mongo_cxx_driver",
    urls = ["https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.6/mongo-cxx-driver-r3.6.6.tar.gz"],
    strip_prefix = "mongo-cxx-driver-r3.6.6",
)

# compile proto
load("@envoy_api//bazel:api_build_system.bzl", "api_proto_package")

api_proto_package(
    deps = ["@com_github_cncf_udpa//udpa/annotations:pkg"],
)

envoy_cc_library(
    name = "config",
    visibility = ["//:__pkg__"],
    repository = "@envoy",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [
        ":pkg_cc_proto",
        ":codec_lib",
        "@envoy//envoy/registry",
        "//src/meta_protocol_proxy/codec:factory_lib",
    ],
)

envoy_cc_library(
    name = "codec_lib",
    repository = "@envoy",
    srcs = ["mongodb_codec.cc"],  # Source file for your codec
    hdrs = ["mongodb_codec.h"],   # Header file for your codec
    deps = [
        # List dependencies required by your codec here
        "@mongo_cxx_driver//:bsoncxx",
        "@envoy//envoy/buffer:buffer_interface",
        "@envoy//source/common/common:logger_lib",
        "@envoy//source/common/buffer:buffer_lib",
        "@envoy//source/common/common:assert_lib",
        "//src/meta_protocol_proxy/codec:codec_interface",
        ":protocol",
        # Any additional dependencies specific to your codec
    ],
)

envoy_cc_library(
    name = "protocol",
    repository = "@envoy",
    srcs = ["protocol.cc"],
    hdrs = ["protocol.h"],
    deps = [
        ":pkg_cc_proto",
        "@envoy//source/common/buffer:buffer_lib",
        "@envoy//source/common/common:minimal_logger_lib",
        "//src/meta_protocol_proxy/codec:codec_interface",
    ]
)
