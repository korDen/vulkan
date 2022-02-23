load("@build_bazel_rules_apple//apple:versioning.bzl", "apple_bundle_version")
load("@build_bazel_rules_apple//apple:macos.bzl", "macos_application")

apple_bundle_version(
    name = "version",
    build_version = "1.0",
)

cc_library(
    name = "app",
    srcs = ["app.cpp"],
    deps = [
        "//vulkan/platform/app:Application",
        "@rules_vulkan//vulkan:vulkan_cc_library",
    ],
)

cc_binary(
    name = "windows_app",
    deps = [
        ":app",
        "//vulkan/platform/windows",
    ],
)

macos_application(
    name = "macos_app",
    bundle_id = "vulkan.app.mac",
    infoplists = ["//vulkan/platform/macos:info.plist"],
    minimum_os_version = "10.12",
    version = ":version",
    deps = [
        ":app",
        "//vulkan/platform/macos",
    ],
)
