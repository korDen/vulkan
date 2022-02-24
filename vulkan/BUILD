package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "basic",
    srcs = ["basic/main.cpp"],
    linkopts = select({
        "@platforms//os:macos": ["-undefined error"],
        "//conditions:default": [],
    }),
    deps = [
        "@glfw",
        "@glm",
        "@rules_vulkan//vulkan:vulkan_cc_library",
    ],
)

genrule(
    name = "vert_spv",
    srcs = ["tutorial/shader.vert"],
    outs = ["vert.spv"],
    cmd = "$(location @shaderc//:glslc) $(location :tutorial/shader.vert) -o $(OUTS)",
    tools = ["@shaderc//:glslc"],
)

genrule(
    name = "frag_spv",
    srcs = ["tutorial/shader.frag"],
    outs = ["frag.spv"],
    cmd = "$(location @shaderc//:glslc) $(location :tutorial/shader.frag) -o $(OUTS)",
    tools = ["@shaderc//:glslc"],
)

cc_binary(
    name = "tutorial",
    srcs = ["tutorial/main.cpp"],
    data = [
        "frag.spv",
        "vert.spv",
    ],
    linkopts = select({
        "@platforms//os:macos": ["-undefined error"],
        "//conditions:default": [],
    }),
    deps = [
        "@bazel_tools//tools/cpp/runfiles",
        "@glfw",
        "@rules_vulkan//vulkan:vulkan_cc_library",
    ],
)
