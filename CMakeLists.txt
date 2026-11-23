# ===========================================================================
# agent-linux (AirymaxOS) tests-linux submodule CMakeLists.txt
# 构建入口：unit + integration + fuzz + perf 测试
# 决策 F1：最小可编译骨架（非桩）
# ===========================================================================

cmake_minimum_required(VERSION 3.20)

project(airy_tests
	VERSION 0.1.1
	LANGUAGES C
	DESCRIPTION "AirymaxOS tests-linux submodule — unit + integration + fuzz + perf")

# ---------------------------------------------------------------------------
# C 标准：C11（用户态测试）
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# [SC] 头文件引用（OS-IRON-014 / 决策 G1）
# 唯一物理宿主：kernel/include/uapi/linux/airymax/，禁止物理副本。
#
# AIRY_HDR_ROOT 指向 make headers_install 的导出树（其 include/ 根），
# 按 <linux/airymax/foo.h> 引用契约头。用户态测试只能用导出后的 UAPI：
# 源码树的 kernel/include/uapi 里 types.h 依赖 __EXPORTED_HEADERS__，
# stddef.h 会去找只存在于内核内部的 linux/compiler_types.h，直接引用
# 编不过；而把 kernel/include 放进搜索路径又会让内核内部 linux/types.h
# 与 libc 的 loff_t/uint64_t/blkcnt_t 冲突。
#
# 生成导出树（产物不落源码区）：
#   make -C ../kernel O=<build>/kobj headers_install INSTALL_HDR_PATH=<build>/hdr
#   cmake -S . -B <build> -DAIRY_HDR_ROOT=<build>/hdr/include
# ---------------------------------------------------------------------------
if(NOT AIRY_HDR_ROOT)
	message(FATAL_ERROR
		"AIRY_HDR_ROOT 未设置。用户态测试必须针对导出后的 UAPI 头构建："
		"先执行 make -C ../kernel O=<build>/kobj headers_install "
		"INSTALL_HDR_PATH=<build>/hdr，再传 -DAIRY_HDR_ROOT=<build>/hdr/include。")
endif()
include_directories(${AIRY_HDR_ROOT})

# ---------------------------------------------------------------------------
# 编译选项（OS-STD-FMT-001/002）
# 测试编译允许 -Wno-unused（部分测试可能不使用所有参数）
# ---------------------------------------------------------------------------
add_compile_options(
	-Wall
	-Wextra
	-Werror
	-Wno-unused-parameter
	-Wno-unused-function
	-fstack-protector-strong
	-D_FORTIFY_SOURCE=2
)

# ---------------------------------------------------------------------------
# 启用 CTest 测试运行器
# ---------------------------------------------------------------------------
enable_testing()

# ---------------------------------------------------------------------------
# 子目录
# ---------------------------------------------------------------------------
add_subdirectory(unit)

# ---------------------------------------------------------------------------
# 自定义目标：运行全部测试
# ---------------------------------------------------------------------------
add_custom_target(check
	COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
	DEPENDS test_ipc_magic test_log_ring_uapi test_superv_uapi test_error_codes
	COMMENT "运行 AirymaxOS 全部测试"
)
