workspace(name = "vulkan")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

new_git_repository(
    name = "glfw",
    remote = "https://github.com/glfw/glfw.git",
    commit = "8d7e5cdb49a1a5247df612157ecffdd8e68923d2",
    build_file = "glfw.BUILD",
    shallow_since = "1551813720 +0100",
)

new_git_repository(
    name = "glm",
    remote = "https://github.com/g-truc/glm.git",
    commit = "658d8960d081e0c9c312d49758c7ef919371b428",
    shallow_since = "1553016128 +0100",
    build_file = "glm.BUILD",
)

git_repository(
    name = "rules_vulkan",
    remote = "https://github.com/jadarve/rules_vulkan.git",
    commit = "7aa5bd55890c6018583d2b94f446deb33499e27f",
    shallow_since = "1645375268 -0500"
)

load("@rules_vulkan//vulkan:repositories.bzl", "vulkan_repositories")
vulkan_repositories()
