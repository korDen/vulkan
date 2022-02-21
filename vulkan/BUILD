cc_binary(
    name = "basic",
    srcs = ["basic/main.cpp"],
    linkopts = select({
        "@platforms//os:macos": ["-undefined error"],
        "//conditions:default": [],
    }),
    deps = [
        "@glfw//:glfw",
        "@glm//:glm",
        "@rules_vulkan//vulkan:vulkan_cc_library",
    ],
)
