# ===========================================================================
# unit tests CMakeLists.txt
# 单元测试目录（[IND]）
# 决策 F1：真实可编译实现（非桩）
# ===========================================================================

# ---------------------------------------------------------------------------
# test_ipc_magic — 验证 [SC] ipc.h 中的 AIRY_IPC_MAGIC 与消息头布局
# ---------------------------------------------------------------------------
add_executable(test_ipc_magic
	test_ipc_magic.c
)

# [SC] 头文件引用已由顶层 CMakeLists.txt 通过 include_directories 配置

target_compile_definitions(test_ipc_magic
	PRIVATE
		_GNU_SOURCE
)

# 注册到 CTest
add_test(
	NAME test_ipc_magic
	COMMAND test_ipc_magic
)

# 设置测试属性
set_tests_properties(test_ipc_magic
	PROPERTIES
		LABELS "unit;sc;ipc"
		DESCRIPTION "验证 [SC] ipc.h 的 AIRY_IPC_MAGIC 与消息头布局"
)
