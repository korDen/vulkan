package(default_visibility = ["//visibility:public"])

exports_files(["info.plist"])

objc_library(
    name = "macos",
    srcs = [
        "AppDelegate.mm",
        "Window.mm",
        "main.mm",
    ],
    hdrs = [
        "AppDelegate.h",
        "Window.h",
    ],
    sdk_frameworks = [
        "AppKit",
    ],
    deps = ["//vulkan/platform/app:Application"],
)
