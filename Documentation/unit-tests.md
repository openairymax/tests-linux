# 单元测试设计

> **子仓**：`tests-linux/`
> **组件**：`unit/`
> **可编译入口**：`unit/test_ipc_magic.c` → `test_ipc_magic`
> **[SC] 依赖**：`kernel/include/uapi/linux/airymax/ipc.h`
> **设计文档**：`docs/AirymaxOS/10-architecture/07-directory-structure.md` §4.8

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

License: GPL-2.0-only

---

## 1. 设计目标

单元测试验证 [SC] 共享契约头文件的契约一致性，确保用户态（agentrt）
与内核态（agentrt-linux）使用同一套常量、布局与类型。

| 测试 | 范围 | 状态 |
|------|------|------|
| `test_ipc_magic` | [SC] `ipc.h` magic + 消息头布局 + Badge | ✅ 0.1.1 |
| `test_error_codes` | [SC] `error.h` 故障码 | ⏳ 1.0.1 |
| `test_lsm_types` | [SC] `lsm_types.h` Agent 安全上下文 | ⏳ 1.0.1 |
| `test_memory_types` | [SC] `memory_types.h` 内存层级 | ⏳ 1.0.1 |
| `test_cognition_types` | [SC] `cognition_types.h` Q16.16 + 阶段 | ⏳ 1.0.1 |
| `test_badge_layout` | Badge 64-bit 深度测试 | ⏳ 1.0.1 |

## 2. test_ipc_magic 测试项

| 测试函数 | 断言数 | 验证内容 |
|----------|--------|----------|
| `test_ipc_magic_value` | 2 | `AIRY_IPC_MAGIC == 0x41524531u` 与字符表示 'ARE1' |
| `test_ipc_header_size` | 3 | `AIRY_IPC_HDR_SIZE == 128` 与 `sizeof(struct)` 一致 |
| `test_ipc_header_offset` | 10 | 关键字段偏移（magic/opcode/flags/trace_id/.../crc32） |
| `test_badge_layout` | 5 | `AIRY_BADGE_COMPILE` 与本地实现 + 分解对称性 + mask 完整性 |
| `test_badge_extremes` | 7 | 边界值（全零 / 最大值） |
| `test_ipc_opcodes` | 4 | `AIRY_IPC_OP_*` 常量 |
| `test_cap_perms` | 7 | `AIRY_CAP_PERM_*` 权限位 + 不重叠验证 |
| `test_ipc_flags` | 4 | `AIRY_IPC_FLAG_*` 常量 |
| `test_header_init` | 5 | 构造完整消息头并验证字段 |

总断言数：**47 项**

## 3. 测试输出示例

```
=== AirymaxOS [SC] ipc.h 契约测试 ===
编译时间戳: Jul 20 2026 23:42:00

[PASS] /home/spharx/SpharxWorks/airymaxhub/agentrt-linux/tests-linux/unit/test_ipc_magic.c:62 AIRY_IPC_MAGIC == 0x41524531u ('ARE1')
[PASS] /home/spharx/SpharxWorks/airymaxhub/agentrt-linux/tests-linux/unit/test_ipc_magic.c:73 AIRY_IPC_MAGIC 字符表示 == 'ARE1'
...

=== 测试结果: 47/47 通过 ===
[OK] test_ipc_magic 全部断言通过
```

## 4. CTest 集成

`tests-linux/CMakeLists.txt` 通过 `enable_testing()` 启用 CTest，
`unit/CMakeLists.txt` 通过 `add_test()` 注册测试：

```cmake
add_test(
    NAME test_ipc_magic
    COMMAND test_ipc_magic
)

set_tests_properties(test_ipc_magic
    PROPERTIES
        LABELS "unit;sc;ipc"
        DESCRIPTION "验证 [SC] ipc.h 的 AIRY_IPC_MAGIC 与消息头布局"
)
```

运行测试：

```bash
cd tests-linux/build
ctest --output-on-failure
```

或仅运行 `test_ipc_magic`：

```bash
ctest -R test_ipc_magic --output-on-failure
```

## 5. 测试标签

每个测试通过 `LABELS` 属性分类：

| 标签 | 含义 |
|------|------|
| `unit` | 单元测试 |
| `sc` | [SC] 契约测试 |
| `ipc` | A-IPC 相关 |
| `lsm` | LSM 相关 |
| `mem` | 内存相关 |
| `cog` | 认知相关 |

按标签过滤运行：

```bash
ctest -L sc --output-on-failure    # 仅运行 [SC] 契约测试
ctest -L unit --output-on-failure  # 仅运行单元测试
```

## 6. 测试宏

`test_ipc_magic.c` 定义了自定义测试宏 `AIRY_TEST`：

```c
#define AIRY_TEST(cond, msg)  ...
```

- 通过：打印 `[PASS]` 到 stdout
- 失败：打印 `[FAIL]` 到 stderr，立即返回 `EXIT_FAILURE`

后续版本将提取为公共头文件 `unit/airy_test.h`，供所有单元测试复用。

## 7. IRON-9 同源关系

| 层级 | 实体 | 说明 |
|------|------|------|
| `[SC]` | `ipc.h` 全部常量与结构 | 与 agentrt 共享 |
| `[IND]` | 测试实现本身 | agentrt-linux 专属 |

agentrt 用户态有对应的 `airytests` 工具（ecosystem/airytests），
两者通过 [SC] 共享契约，但测试实现完全独立。

## 8. 变更历史

| 版本 | 日期 | 变更 |
|------|------|------|
| 0.1.1 | 2026-07-20 | 初始最小可编译实现：test_ipc_magic（47 项断言） |
