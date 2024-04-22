load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
)

# compile proto
load("@envoy_api//bazel:api_build_system.bzl", "api_proto_package")

api_proto_package(
    deps = ["@com_github_cncf_udpa//udpa/annotations:pkg"],
)

envoy_cc_library(
    name = "config",
    visibility = ["//visibility:public"],
    repository = "@envoy",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [
        ":codec_lib",
        "@envoy//envoy/registry",
        "//src/meta_protocol_proxy/codec:factory_lib",
    ],
)

envoy_cc_library(
    name = "memcached_codec_lib",
    repository = "@envoy",
    srcs = ["memcached_codec.cc"],  # Source file for your codec
    hdrs = ["memcached_codec.h"],   # Header file for your codec
    deps = [
        # List dependencies required by your codec here
        "@envoy//envoy/buffer:buffer_interface",
        "@envoy//source/common/common:logger_lib",
        "@envoy//source/common/buffer:buffer_lib",
        "//src/meta_protocol_proxy/codec:codec_interface",
        # Any additional dependencies specific to your codec
    ],
)


envoy_cc_library(
    name = "codec_lib",
    repository = "@envoy",
    srcs = ["brpc_codec.cc"],
    hdrs = ["brpc_codec.h"],
    deps = [
        "@envoy//envoy/buffer:buffer_interface",
        "@envoy//source/common/common:logger_lib",
        "@envoy//source/common/buffer:buffer_lib",
        "//src/meta_protocol_proxy/codec:codec_interface",
        ":protocol",
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
    ]
)
