package(default_visibility = ["//visibility:public"])

exports_files(["info.plist"])

objc_library(
    name = "macos",
    srcs = [
        "ApplicationControllerImpl.mm",
        "AppDelegate.mm",
        "MetalWindow.mm",
        "main.mm",
    ],
    hdrs = [
        "ApplicationControllerImpl.h",
        "AppDelegate.h",
        "MetalWindow.h",
    ],
    sdk_frameworks = [
        "AppKit",
        "QuartzCore",
        "MetalKit",
        "Metal",
    ],
    deps = [
        "//vulkan/platform/app:Application",
        "@rules_vulkan//vulkan:vulkan_cc_library",
        "@bazel_tools//tools/cpp/runfiles",
    ],
    defines = ["PLATFORM_MACOS_USE_METAL"],
)
