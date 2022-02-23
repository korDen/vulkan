workspace(name = "vulkan")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

new_git_repository(
    name = "glfw",
    build_file = "glfw.BUILD",
    commit = "8d7e5cdb49a1a5247df612157ecffdd8e68923d2",
    remote = "https://github.com/glfw/glfw.git",
    shallow_since = "1551813720 +0100",
)

new_git_repository(
    name = "glm",
    build_file = "glm.BUILD",
    commit = "658d8960d081e0c9c312d49758c7ef919371b428",
    remote = "https://github.com/g-truc/glm.git",
    shallow_since = "1553016128 +0100",
)

new_git_repository(
    name = "shaderc",
    build_file = "shaderc.BUILD",
    commit = "1bbf43f210941ba69a2cd05cf3529063f1ff5bb9",
    remote = "https://github.com/google/shaderc.git",
    shallow_since = "1643823405 -0500",
)

git_repository(
    name = "glslang",
    commit = "d15deba1d8c7da7336945a80eccbf875ae884648",
    remote = "https://github.com/KhronosGroup/glslang.git",
    shallow_since = "1645053320 -0700",
)

git_repository(
    name = "spirv_tools",
    commit = "b1877de5cd776117050bd42f08d04b52bce16099",
    remote = "https://github.com/KhronosGroup/SPIRV-Tools.git",
    shallow_since = "1643227305 -0500",
)

git_repository(
    name = "spirv_headers",
    commit = "b42ba6d92faf6b4938e6f22ddd186dbdacc98d78",
    remote = "https://github.com/KhronosGroup/SPIRV-Headers.git",
    shallow_since = "1642610647 -0600",
)

git_repository(
    name = "rules_vulkan",
    commit = "7aa5bd55890c6018583d2b94f446deb33499e27f",
    remote = "https://github.com/jadarve/rules_vulkan.git",
    shallow_since = "1645375268 -0500",
)

load("@rules_vulkan//vulkan:repositories.bzl", "vulkan_repositories")

vulkan_repositories()

git_repository(
    name = "rules_rust",
    commit = "34cc2e9276b195eecc5c078f79b6c3ffc770da78",
    remote = "https://github.com/bazelbuild/rules_rust.git",
    shallow_since = "1645508790 -0800",
)

git_repository(
    name = "bazel_skylib",
    commit = "c4dfec1bf1316ab4e9fffbb382cf48259e0944fb",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    shallow_since = "1644598925 -0500",
)

git_repository(
    name = "build_bazel_rules_nodejs",
    commit = "45e97fc76a593b41e7372100545fee358c5a3bba",
    remote = "https://github.com/bazelbuild/rules_nodejs.git",
    shallow_since = "1644941997 -0800",
)

load("@rules_rust//rust:repositories.bzl", "rules_rust_dependencies", "rust_register_toolchains")

rules_rust_dependencies()

rust_register_toolchains()
