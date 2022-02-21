cc_binary(
    name = "basic",
    srcs = ["basic/main.cpp"],
    linkopts = select({
        "@platforms//os:macos": ["-undefined error"],
        "//conditions:default": [],
    }) + [
        "-lglfw",
        "-lvulkan",
    ],
)
