cc_binary(
    name = "basic",
    srcs = ["basic/main.cpp"],
    linkopts = select({
        "@platforms//os:macos": ["-undefined error", "-lglfw", "-lvulkan"],
        "//conditions:default": [],
    }),
    deps = [
        "@glfw//:glfw",
        "@glm//:glm",
        "@rules_vulkan//vulkan:vulkan_cc_library",
    ],
)

genrule(
	name = "vert_spv",
	srcs = ["tutorial/shader.vert"],
	outs = ["vert.spv"],
	cmd = "glslc $(location :tutorial/shader.vert) -o $(OUTS)",
)

genrule(
	name = "frag_spv",
	srcs = ["tutorial/shader.frag"],
	outs = ["frag.spv"],
	cmd = "glslc $(location :tutorial/shader.frag) -o $(OUTS)",
)

cc_binary(
    name = "tutorial",
    srcs = ["tutorial/main.cpp"],
    linkopts = select({
        "@platforms//os:macos": ["-undefined error", "-lglfw", "-lvulkan"],
        "//conditions:default": [],
    }),
    deps = [
        "@glfw//:glfw",
        "@glm//:glm",
        "@rules_vulkan//vulkan:vulkan_cc_library",
        "@bazel_tools//tools/cpp/runfiles",
    ],
    data = [
    	"vert.spv",
    	"frag.spv",
    ],
)
