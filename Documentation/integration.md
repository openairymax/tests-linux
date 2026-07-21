# 集成测试设计

> **子仓**：`tests-linux/`
> **组件**：`integration/`（规划中，本文件为设计占位）
> **[SC] 依赖**：`kernel/include/uapi/linux/airymax/*.h`
> **协同组件**：`kernel/`、`security/`、`memory/`、`cognition/`、`services/`
> **设计文档**：`docs/AirymaxOS/10-architecture/07-directory-structure.md` §4.8

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

License: GPL-2.0-only

---

## 1. 设计目标

集成测试验证内核模块与用户态组件的端到端协作，覆盖：

- LSM 注册与 Capability 检查（`security/airy_lsm/`）
- MemoryRovol L1 分配/释放（`memory/memoryrovol/`）
- CoreLoopThree kthread 启停（`cognition/coreloopthree/`）
- macro_d daemon 故障裁决（`services/daemons/macro_d/`）
- airymaxmon 状态读取（`system/monitoring/`）

集成测试需要 root 权限（加载内核模块）。

## 2. 测试矩阵

| 测试 | 入口 | 依赖 | 状态 |
|------|------|------|------|
| `itest_lsm_register` | `integration/itest_lsm_register.c` | `security/airy_lsm.ko` | ⏳ 1.0.1 |
| `itest_mr_l1_alloc` | `integration/itest_mr_l1_alloc.c` | `memory/airy_mr_l1.ko` | ⏳ 1.0.1 |
| `itest_clt_kthread` | `integration/itest_clt_kthread.c` | `cognition/airy_clt.ko` | ⏳ 1.0.1 |
| `itest_macro_d_eventfd` | `integration/itest_macro_d_eventfd.c` | `services/macro_d` + eventfd | ⏳ 1.0.1 |
| `itest_airymaxmon_status` | `integration/itest_airymaxmon_status.c` | `system/airymaxmon` | ⏳ 1.0.1 |
| `itest_agentctl_magic` | `integration/itest_agentctl_magic.c` | `cloudnative/agentctl` | ⏳ 1.0.1 |

## 3. 测试流程

每个集成测试遵循统一流程：

1. **前置条件检查**：内核模块是否加载，daemon 是否运行
2. **触发动作**：加载模块、启动 daemon、发送事件
3. **状态验证**：读取 `/proc/airy/status`，断言关键字段
4. **清理**：卸载模块、停止 daemon

示例（`itest_clt_kthread.c` 草案）：

```c
/* 1. 加载 airy_clt.ko */
system("insmod cognition/coreloopthree/airy_clt.ko");

/* 2. 等待 kthread 启动 */
sleep(2);

/* 3. 读取 /proc/airy/status 验证 loop_count > 0 */
FILE *fp = fopen("/proc/airy/status", "r");
/* ... 解析 loop_count ... */
assert(loop_count > 0);

/* 4. 卸载模块 */
system("rmmod airy_clt");
```

## 4. CTest 集成

集成测试通过 `add_test()` 注册，但需要 `ROOT` 权限标签：

```cmake
add_test(
    NAME itest_clt_kthread
    COMMAND sudo -E itest_clt_kthread
)
set_tests_properties(itest_clt_kthread
    PROPERTIES
        LABELS "integration;kernel;cognition"
)
```

CI 流水线将在特权容器中运行集成测试。

## 5. 测试环境

| 环境 | 用途 | 内核版本 |
|------|------|----------|
| 本地开发机 | 快速验证 | OLK 6.6 / 主线 6.6+ |
| CI 容器（特权） | 自动化集成测试 | openEuler 24.03 LTS |
| QEMU 虚拟机 | 内核崩溃恢复测试 | 主线 6.6+ |

## 6. 故障注入

集成测试使用以下故障注入机制：

| 故障类型 | 注入方式 | 验证目标 |
|----------|----------|----------|
| Capability 伪造 | 写入非法 badge 到 `/proc/airy/inject` | macro_d 触发 TERMINATE |
| IPC 超时 | 阻塞 kthread 唤醒 | macro_d 触发 DEGRADE |
| 内存泄漏 | 不调用 `airy_mr_l1_free()` | airymaxmon 显示 fault_count++ |

## 7. IRON-9 同源关系

| 层级 | 实体 | 说明 |
|------|------|------|
| `[SC]` | 全部 [SC] 头文件 | 跨内核/用户态共享 |
| `[IND]` | 集成测试实现 | agentrt-linux 专属 |

## 8. 当前实现状态

0.1.1 版本中集成测试仅作为设计占位：

- ✅ `unit/test_ipc_magic` 已实现（单元测试层）
- ⏳ `integration/` 子目录与全部 `itest_*` 尚未实现（1.0.1）
- ⏳ CI 特权容器环境尚未配置（1.0.1）

## 9. 变更历史

| 版本 | 日期 | 变更 |
|------|------|------|
| 0.1.1 | 2026-07-20 | 初始设计文档；测试矩阵与流程草案 |
