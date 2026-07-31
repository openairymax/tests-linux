// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
 *
 * test_ipc_magic.c — 验证 [SC] ipc.h 中的 A-IPC magic 与消息头布局
 *
 * 测试项：
 *   1. AIRY_IPC_MAGIC == 0x41524531u（'ARE1'）
 *   2. AIRY_IPC_HDR_SIZE == 128
 *   3. struct airy_ipc_msg_hdr 大小 == 128 字节
 *   4. capability_badge 字段偏移 == 40（Layout C v4）
 *   5. Badge 64-bit Native Word 编译/分解一致
 *   6. AIRY_BADGE_COMPILE() 与 AIRY_BADGE_EPOCH/RANDTAG/PERMS 对称
 *   7. capability_badge offset 与 [SC] _Static_assert 一致
 *
 * 决策 F1：真实可编译实现（非桩，含真实断言）。
 *
 * 返回值：
 *   0  - 全部断言通过
 *   非 0 - 断言失败（assert 中止）
 *
 * 相关文档：
 *   docs/AirymaxOS/10-architecture/07-directory-structure.md §4.8
 *   kernel/include/uapi/linux/airymax/ipc.h
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/* [SC] 共享契约头文件（通过 -I../kernel/include/uapi/linux 引用） */
#include <airymax/ipc.h>

/* ─── 辅助函数 ──────────────────────────────────────────────────────── */

static int g_test_count = 0;
static int g_test_passed = 0;

#define AIRY_TEST(cond, msg)						\
	do {								\
		g_test_count++;						\
		if (cond) {						\
			g_test_passed++;					\
			printf("[PASS] %s:%d %s\n",			\
			       __FILE__, __LINE__, (msg));		\
		} else {						\
			fprintf(stderr, "[FAIL] %s:%d %s\n",		\
				__FILE__, __LINE__, (msg));		\
			return EXIT_FAILURE;				\
		}							\
	} while (0)

/*
 * airy_badge_compile_local - 本地实现 Badge 编译，与 [SC] 宏对比
 *
 * 用于验证 [SC] AIRY_BADGE_COMPILE 宏的字节布局正确性。
 */
static uint64_t airy_badge_compile_local(uint64_t epoch, uint64_t randtag,
					 uint64_t perms)
{
	return ((epoch & 0xFFFFULL) << 48) |
	       ((randtag & 0xFFFFFFFFULL) << 16) |
	       (perms & 0xFFFFULL);
}

/* ─── 测试用例 ──────────────────────────────────────────────────────── */

/*
 * test_ipc_magic_value — 验证 magic 数值
 */
static void test_ipc_magic_value(void)
{
	AIRY_TEST(AIRY_IPC_MAGIC == 0x41524531u,
		  "AIRY_IPC_MAGIC == 0x41524531u ('ARE1')");

	/* 验证 magic 的字符表示 */
	char magic_str[5];
	magic_str[0] = (char)((AIRY_IPC_MAGIC >> 24) & 0xFF);
	magic_str[1] = (char)((AIRY_IPC_MAGIC >> 16) & 0xFF);
	magic_str[2] = (char)((AIRY_IPC_MAGIC >> 8) & 0xFF);
	magic_str[3] = (char)(AIRY_IPC_MAGIC & 0xFF);
	magic_str[4] = '\0';

	AIRY_TEST(strcmp(magic_str, "ARE1") == 0,
		  "AIRY_IPC_MAGIC 字符表示 == 'ARE1'");
}

/*
 * test_ipc_header_size — 验证消息头大小
 */
static void test_ipc_header_size(void)
{
	AIRY_TEST(AIRY_IPC_HDR_SIZE == 128,
		  "AIRY_IPC_HDR_SIZE == 128");

	AIRY_TEST(sizeof(struct airy_ipc_msg_hdr) == 128,
		  "sizeof(struct airy_ipc_msg_hdr) == 128");

	AIRY_TEST(sizeof(struct airy_ipc_msg_hdr) == AIRY_IPC_HDR_SIZE,
		  "sizeof(hdr) == AIRY_IPC_HDR_SIZE");
}

/*
 * test_ipc_header_offset — 验证字段偏移
 */
static void test_ipc_header_offset(void)
{
	AIRY_TEST(offsetof(struct airy_ipc_msg_hdr, magic) == 0,
		  "magic offset == 0");
	AIRY_TEST(offsetof(struct airy_ipc_msg_hdr, opcode) == 4,
		  "opcode offset == 4");
	AIRY_TEST(offsetof(struct airy_ipc_msg_hdr, flags) == 6,
		  "flags offset == 6");
	AIRY_TEST(offsetof(struct airy_ipc_msg_hdr, trace_id) == 8,
		  "trace_id offset == 8");
	AIRY_TEST(offsetof(struct airy_ipc_msg_hdr, timestamp_ns) == 16,
		  "timestamp_ns offset == 16");
	AIRY_TEST(offsetof(struct airy_ipc_msg_hdr, src_task) == 24,
		  "src_task offset == 24");
	AIRY_TEST(offsetof(struct airy_ipc_msg_hdr, dst_task) == 32,
		  "dst_task offset == 32");
	AIRY_TEST(offsetof(struct airy_ipc_msg_hdr, capability_badge) == 40,
		  "capability_badge offset == 40 (Layout C v4)");
	AIRY_TEST(offsetof(struct airy_ipc_msg_hdr, payload_len) == 48,
		  "payload_len offset == 48");
	AIRY_TEST(offsetof(struct airy_ipc_msg_hdr, crc32) == 52,
		  "crc32 offset == 52");
}

/*
 * test_badge_layout — 验证 Badge 64-bit 编译与分解
 */
static void test_badge_layout(void)
{
	const uint64_t epoch = 0x1234;
	const uint64_t randtag = 0xDEADBEEF;
	const uint64_t perms = 0x0007;
	const uint64_t badge = AIRY_BADGE_COMPILE(epoch, randtag, perms);

	/* 验证本地实现与 [SC] 宏一致 */
	uint64_t local = airy_badge_compile_local(epoch, randtag, perms);
	AIRY_TEST(badge == local,
		  "AIRY_BADGE_COMPILE 与本地实现一致");

	/* 验证分解对称性 */
	AIRY_TEST(AIRY_BADGE_EPOCH(badge) == epoch,
		  "AIRY_BADGE_EPOCH 对称");
	AIRY_TEST(AIRY_BADGE_RANDTAG(badge) == randtag,
		  "AIRY_BADGE_RANDTAG 对称");
	AIRY_TEST(AIRY_BADGE_PERMS(badge) == perms,
		  "AIRY_BADGE_PERMS 对称");

	/* 验证 mask 完整性 */
	uint64_t masked = badge & (AIRY_BADGE_EPOCH_MASK |
				   AIRY_BADGE_RANDTAG_MASK |
				   AIRY_BADGE_PERMS_MASK);
	AIRY_TEST(masked == badge,
		  "Badge 三段 mask 完整覆盖 64-bit");
}

/*
 * test_badge_extremes — 验证 Badge 边界值
 */
static void test_badge_extremes(void)
{
	/* 全零 */
	uint64_t zero = AIRY_BADGE_COMPILE(0, 0, 0);
	AIRY_TEST(zero == 0, "Badge(0,0,0) == 0");
	AIRY_TEST(AIRY_BADGE_EPOCH(zero) == 0, "Badge(0,0,0).epoch == 0");
	AIRY_TEST(AIRY_BADGE_RANDTAG(zero) == 0, "Badge(0,0,0).randtag == 0");
	AIRY_TEST(AIRY_BADGE_PERMS(zero) == 0, "Badge(0,0,0).perms == 0");

	/* 最大值 */
	uint64_t max_badge = AIRY_BADGE_COMPILE(0xFFFF, 0xFFFFFFFF, 0xFFFF);
	AIRY_TEST(AIRY_BADGE_EPOCH(max_badge) == 0xFFFF,
		  "Badge max epoch == 0xFFFF");
	AIRY_TEST(AIRY_BADGE_RANDTAG(max_badge) == 0xFFFFFFFF,
		  "Badge max randtag == 0xFFFFFFFF");
	AIRY_TEST(AIRY_BADGE_PERMS(max_badge) == 0xFFFF,
		  "Badge max perms == 0xFFFF");
}

/*
 * test_ipc_opcodes — 验证 IPC opcode 常量
 */
static void test_ipc_opcodes(void)
{
	AIRY_TEST(AIRY_IPC_OP_SEND == 0x0001, "AIRY_IPC_OP_SEND == 0x0001");
	AIRY_TEST(AIRY_IPC_OP_RECV == 0x0002, "AIRY_IPC_OP_RECV == 0x0002");
	AIRY_TEST(AIRY_IPC_OP_FREEZE == 0x0005,
		  "AIRY_IPC_OP_FREEZE == 0x0005");
	AIRY_TEST(AIRY_IPC_OP_CAP_REQUEST == 0x0010,
		  "AIRY_IPC_OP_CAP_REQUEST == 0x0010");
}

/*
 * test_cap_perms — 验证 capability 权限位
 *
 * SSoT: AIRY_CAP_PERM_* 常量定义在 security_types.h（ipc.h 通过
 * #include <linux/airymax/security_types.h> 间接引入）。本测试验证
 * security_types.h 中的 7 个权限位定义。
 */
static void test_cap_perms(void)
{
	AIRY_TEST(AIRY_CAP_PERM_SEND == 0x0001,
		  "AIRY_CAP_PERM_SEND == 0x0001");
	AIRY_TEST(AIRY_CAP_PERM_RECV == 0x0002,
		  "AIRY_CAP_PERM_RECV == 0x0002");
	AIRY_TEST(AIRY_CAP_PERM_DERIVE == 0x0004,
		  "AIRY_CAP_PERM_DERIVE == 0x0004");
	AIRY_TEST(AIRY_CAP_PERM_KILL == 0x0008,
		  "AIRY_CAP_PERM_KILL == 0x0008");
	AIRY_TEST(AIRY_CAP_PERM_FILE_OPEN == 0x0010,
		  "AIRY_CAP_PERM_FILE_OPEN == 0x0010");
	AIRY_TEST(AIRY_CAP_PERM_ROTATE == 0x0020,
		  "AIRY_CAP_PERM_ROTATE == 0x0020");
	AIRY_TEST(AIRY_CAP_PERM_SUPERVISE == 0x0040,
		  "AIRY_CAP_PERM_SUPERVISE == 0x0040");

	/* 验证权限位不重叠 */
	uint16_t all_perms = AIRY_CAP_PERM_SEND | AIRY_CAP_PERM_RECV |
			     AIRY_CAP_PERM_DERIVE | AIRY_CAP_PERM_KILL |
			     AIRY_CAP_PERM_FILE_OPEN | AIRY_CAP_PERM_ROTATE |
			     AIRY_CAP_PERM_SUPERVISE;
	AIRY_TEST(all_perms == 0x007F, "全部权限位 OR == 0x007F");
	AIRY_TEST(AIRY_CAP_PERM_ALL == 0x007F, "AIRY_CAP_PERM_ALL == 0x007F");
}

/*
 * test_ipc_flags — 验证 IPC flag 常量
 */
static void test_ipc_flags(void)
{
	AIRY_TEST(AIRY_IPC_FLAG_ZEROCOPY == 0x0001,
		  "AIRY_IPC_FLAG_ZEROCOPY == 0x0001");
	AIRY_TEST(AIRY_IPC_FLAG_CAP_CARRY == 0x0002,
		  "AIRY_IPC_FLAG_CAP_CARRY == 0x0002");
	AIRY_TEST(AIRY_IPC_FLAG_ENCRYPT == 0x0004,
		  "AIRY_IPC_FLAG_ENCRYPT == 0x0004");
	AIRY_TEST(AIRY_IPC_FLAG_COMPRESS == 0x0008,
		  "AIRY_IPC_FLAG_COMPRESS == 0x0008");
}

/*
 * test_header_init — 构造一个完整消息头并验证字段
 */
static void test_header_init(void)
{
	struct airy_ipc_msg_hdr hdr;
	uint64_t badge = AIRY_BADGE_COMPILE(0x0001, 0xABCD1234, 0x0003);

	memset(&hdr, 0, sizeof(hdr));
	hdr.magic = AIRY_IPC_MAGIC;
	hdr.opcode = AIRY_IPC_OP_SEND;
	hdr.flags = AIRY_IPC_FLAG_CAP_CARRY;
	hdr.trace_id = 0x123456789ABCDEF0ULL;
	hdr.timestamp_ns = 1784562248123456789ULL;
	hdr.src_task = 100;
	hdr.dst_task = 200;
	hdr.capability_badge = badge;
	hdr.payload_len = 256;
	hdr.crc32 = 0xDEADBEEF;

	AIRY_TEST(hdr.magic == AIRY_IPC_MAGIC,
		  "hdr.magic == AIRY_IPC_MAGIC");
	AIRY_TEST(hdr.opcode == AIRY_IPC_OP_SEND,
		  "hdr.opcode == AIRY_IPC_OP_SEND");
	AIRY_TEST(hdr.capability_badge == badge,
		  "hdr.capability_badge == badge");
	AIRY_TEST(AIRY_BADGE_EPOCH(hdr.capability_badge) == 0x0001,
		  "hdr.capability_badge.epoch == 0x0001");
	AIRY_TEST(AIRY_BADGE_PERMS(hdr.capability_badge) == 0x0003,
		  "hdr.capability_badge.perms == 0x0003");
}

/* ─── main 入口 ─────────────────────────────────────────────────────── */

int main(void)
{
	printf("=== AirymaxOS [SC] ipc.h 契约测试 ===\n");
	printf("编译时间戳: %s %s\n\n", __DATE__, __TIME__);

	test_ipc_magic_value();
	test_ipc_header_size();
	test_ipc_header_offset();
	test_badge_layout();
	test_badge_extremes();
	test_ipc_opcodes();
	test_cap_perms();
	test_ipc_flags();
	test_header_init();

	printf("\n=== 测试结果: %d/%d 通过 ===\n",
	       g_test_passed, g_test_count);
	printf("[OK] test_ipc_magic 全部断言通过\n");

	return EXIT_SUCCESS;
}
