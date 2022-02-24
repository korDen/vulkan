package(default_visibility = ["//visibility:public"])

cc_library(
	name = "windows",
	srcs = ["main.cpp"],
	linkopts = ["/SUBSYSTEM:WINDOWS"],
	deps = [
		"//vulkan/platform/app:Application",
		"@bazel_tools//tools/cpp/runfiles",
	],
)
