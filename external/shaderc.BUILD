genrule(
    name = "gen_build_version",
    outs = ["build-version.inc"],
    cmd = "echo \\\"v2022.2-dev\\\" > $(location build-version.inc)",
)

cc_library(
    name = "libshaderc_util",
    srcs = glob(
        include = ["libshaderc_util/src/*.cc"],
        exclude = ["libshaderc_util/src/*_test.cc"],
    ),
    hdrs = glob(["libshaderc_util/include/libshaderc_util/*.h"]) + [
        "libshaderc_util/include/libshaderc_util/resources.inc",
        "build-version.inc",
    ],
    defines = [
        "AMD_EXTENSIONS",
        "ENABLE_HLSL=0",
        "ENABLE_OPT=0",
        "NV_EXTENSIONS",
    ],
    includes = [
        "libshaderc_util/include",
    ],
    deps = [
        "@spirv_tools//:spirv_tools",
        "@glslang//:SPIRV",
        "@spirv_tools//:spirv_tools_opt",
    ],
)

cc_library(
    name = "shaderc",
    srcs = [
        "libshaderc/src/shaderc.cc",
    ],
    hdrs = [
        "libshaderc/include/shaderc/env.h",
        "libshaderc/include/shaderc/shaderc.h",
        "libshaderc/include/shaderc/shaderc.hpp",
        "libshaderc/include/shaderc/status.h",
        "libshaderc/include/shaderc/visibility.h",
        "libshaderc/src/shaderc_private.h",
    ],
    includes = ["libshaderc/include"],
    deps = [
        ":libshaderc_util",
        "@spirv_headers//:spirv_cpp_headers",
    ],
    includes = ["libshaderc/include"],
)

cc_binary(
    name = "glslc",
    srcs = [
        "glslc/src/dependency_info.cc",
        "glslc/src/dependency_info.h",
        "glslc/src/file.cc",
        "glslc/src/file.h",
        "glslc/src/file_compiler.cc",
        "glslc/src/file_compiler.h",
        "glslc/src/file_includer.cc",
        "glslc/src/file_includer.h",
        "glslc/src/main.cc",
        "glslc/src/resource_parse.cc",
        "glslc/src/resource_parse.h",
        "glslc/src/shader_stage.cc",
        "glslc/src/shader_stage.h",
    ],
    includes = [
        "glslc/src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":libshaderc_util",
        ":shaderc",
        "@spirv_tools//:spirv_tools_opt",
    ],
)
