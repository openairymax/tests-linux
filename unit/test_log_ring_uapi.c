// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
 *
 * test_log_ring_uapi.c — 验证 [SC] log_types.h 与 log_ring.h 的日志 ABI
 *
 * 日志守护与内核生产者隔着 mmap 共享区协作：记录的固定 128 字节、共享头
 * 的索引偏移、几何查询与通知 ioctl 任一漂移，都会让两侧对同一块内存给出
 * 不同解释。这里把内核侧已编译进去的布局在用户态再钉一遍。
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <linux/airymax/log_ring.h>
#include <linux/airymax/log_types.h>

#include "airy_test.h"

static void test_log_magic(void)
{
	char text[5];

	text[0] = (char)((AIRY_LOG_MAGIC >> 24) & 0xFF);
	text[1] = (char)((AIRY_LOG_MAGIC >> 16) & 0xFF);
	text[2] = (char)((AIRY_LOG_MAGIC >> 8) & 0xFF);
	text[3] = (char)(AIRY_LOG_MAGIC & 0xFF);
	text[4] = '\0';

	AIRY_TEST(AIRY_LOG_MAGIC == 0x414C4F47u,
		  "AIRY_LOG_MAGIC == 0x414C4F47 ('ALOG')");
	AIRY_TEST(strcmp(text, "ALOG") == 0,
		  "AIRY_LOG_MAGIC 字符表示 == 'ALOG'");
	AIRY_TEST(AIRY_LOG_RECORD_SIZE == 128,
		  "AIRY_LOG_RECORD_SIZE == 128");
}

static void test_log_record_layout(void)
{
	AIRY_TEST(sizeof(struct airy_log_record) == 128,
		  "sizeof(airy_log_record) == 128");
	AIRY_TEST(sizeof(struct airy_log_record) == AIRY_LOG_RECORD_SIZE,
		  "sizeof(airy_log_record) == AIRY_LOG_RECORD_SIZE");
	AIRY_TEST(_Alignof(struct airy_log_record) == 64,
		  "airy_log_record 按 64 字节对齐");

	AIRY_TEST(offsetof(struct airy_log_record, magic) == 0,
		  "record.magic offset == 0");
	AIRY_TEST(offsetof(struct airy_log_record, level) == 4,
		  "record.level offset == 4");
	AIRY_TEST(offsetof(struct airy_log_record, facility) == 6,
		  "record.facility offset == 6");
	AIRY_TEST(offsetof(struct airy_log_record, timestamp_ns) == 8,
		  "record.timestamp_ns offset == 8");
	AIRY_TEST(offsetof(struct airy_log_record, caller_id) == 16,
		  "record.caller_id offset == 16");
	AIRY_TEST(offsetof(struct airy_log_record, payload_len) == 20,
		  "record.payload_len offset == 20");
	AIRY_TEST(offsetof(struct airy_log_record, payload) == 24,
		  "record.payload offset == 24");
	AIRY_TEST(offsetof(struct airy_log_record, reserved) == 120,
		  "record.reserved offset == 120");
	AIRY_TEST(sizeof(((struct airy_log_record *)0)->payload) == 96,
		  "record.payload 容量 == 96");
}

static void test_log_levels_and_facilities(void)
{
	AIRY_TEST(AIRY_LOG_DEBUG == 0, "AIRY_LOG_DEBUG == 0");
	AIRY_TEST(AIRY_LOG_INFO == 1, "AIRY_LOG_INFO == 1");
	AIRY_TEST(AIRY_LOG_WARN == 2, "AIRY_LOG_WARN == 2");
	AIRY_TEST(AIRY_LOG_ERROR == 3, "AIRY_LOG_ERROR == 3");
	AIRY_TEST(AIRY_LOG_FATAL == 4, "AIRY_LOG_FATAL == 4");
	AIRY_TEST(AIRY_LOG_LEVEL_MAX == 5, "AIRY_LOG_LEVEL_MAX == 5");

	AIRY_TEST(AIRY_LOG_FAC_KERN == 0x0001, "FAC_KERN == 0x0001");
	AIRY_TEST(AIRY_LOG_FAC_USER == 0x0002, "FAC_USER == 0x0002");
	AIRY_TEST(AIRY_LOG_FAC_DAEMON == 0x0003, "FAC_DAEMON == 0x0003");
	AIRY_TEST(AIRY_LOG_FAC_SECURITY == 0x0004, "FAC_SECURITY == 0x0004");
	AIRY_TEST(AIRY_LOG_FAC_SCHED == 0x0005, "FAC_SCHED == 0x0005");
	AIRY_TEST(AIRY_LOG_FAC_IPC == 0x0006, "FAC_IPC == 0x0006");
	AIRY_TEST(AIRY_LOG_FAC_MEMORY == 0x0007, "FAC_MEMORY == 0x0007");
}

static void test_log_ring_header(void)
{
	AIRY_TEST(sizeof(struct airy_log_ring_header) == 40,
		  "sizeof(airy_log_ring_header) == 40");
	AIRY_TEST(offsetof(struct airy_log_ring_header, head) == 0,
		  "ring.head offset == 0");
	AIRY_TEST(offsetof(struct airy_log_ring_header, committed_head) == 8,
		  "ring.committed_head offset == 8");
	AIRY_TEST(offsetof(struct airy_log_ring_header, tail) == 16,
		  "ring.tail offset == 16");
	AIRY_TEST(offsetof(struct airy_log_ring_header, capacity) == 24,
		  "ring.capacity offset == 24");
	AIRY_TEST(offsetof(struct airy_log_ring_header, record_size) == 28,
		  "ring.record_size offset == 28");
	AIRY_TEST(offsetof(struct airy_log_ring_header, frozen) == 32,
		  "ring.frozen offset == 32");
	AIRY_TEST(offsetof(struct airy_log_ring_header, pad) == 36,
		  "ring.pad offset == 36");
}

static void test_log_geom(void)
{
	AIRY_TEST(sizeof(struct airy_log_geom) == 24,
		  "sizeof(airy_log_geom) == 24");
	AIRY_TEST(offsetof(struct airy_log_geom, abi) == 0,
		  "geom.abi offset == 0");
	AIRY_TEST(offsetof(struct airy_log_geom, capacity) == 4,
		  "geom.capacity offset == 4");
	AIRY_TEST(offsetof(struct airy_log_geom, record_size) == 8,
		  "geom.record_size offset == 8");
	AIRY_TEST(offsetof(struct airy_log_geom, hdr_size) == 12,
		  "geom.hdr_size offset == 12");
	AIRY_TEST(offsetof(struct airy_log_geom, ring_bytes) == 16,
		  "geom.ring_bytes offset == 16（8 字节对齐）");
}

static void test_log_device_abi(void)
{
	AIRY_TEST(AIRY_LOG_RING_ABI == 1, "AIRY_LOG_RING_ABI == 1");
	AIRY_TEST(strcmp(AIRY_LOG_DEV_PATH, "/dev/airy_log") == 0,
		  "AIRY_LOG_DEV_PATH == /dev/airy_log");
	AIRY_TEST(strcmp(AIRY_LOG_DEV_NAME, "airy_log") == 0,
		  "AIRY_LOG_DEV_NAME == airy_log");
}

static void test_log_ioctl_values(void)
{
	AIRY_TEST(AIRY_LOG_IOC_MAGIC == 'A', "AIRY_LOG_IOC_MAGIC == 'A'");
	AIRY_TEST(AIRY_LOG_IOC_EFD == 0x40044140u,
		  "AIRY_LOG_IOC_EFD == 0x40044140");
	AIRY_TEST(AIRY_LOG_IOC_GEOM == 0x80184141u,
		  "AIRY_LOG_IOC_GEOM == 0x80184141");
	AIRY_TEST(AIRY_LOG_IOC_FREEZE == 0x4142u,
		  "AIRY_LOG_IOC_FREEZE == 0x4142");
}

static void test_log_record_roundtrip(void)
{
	struct airy_log_record rec;
	const char *msg = "airymax log ring";

	memset(&rec, 0, sizeof(rec));
	rec.magic = AIRY_LOG_MAGIC;
	rec.level = AIRY_LOG_WARN;
	rec.facility = AIRY_LOG_FAC_DAEMON;
	rec.timestamp_ns = 1784562248123456789ULL;
	rec.caller_id = 4242;
	rec.payload_len = (uint32_t)strlen(msg);
	memcpy(rec.payload, msg, rec.payload_len);

	AIRY_TEST(rec.magic == AIRY_LOG_MAGIC, "记录 magic 可回读");
	AIRY_TEST(rec.level == AIRY_LOG_WARN, "记录 level 可回读");
	AIRY_TEST(rec.payload_len <= 96, "payload_len 落在 payload 容量内");
	AIRY_TEST(memcmp(rec.payload, msg, rec.payload_len) == 0,
		  "记录 payload 可回读");
}

int main(void)
{
	printf("=== AirymaxOS [SC] 日志环 ABI 契约测试 ===\n");

	test_log_magic();
	test_log_record_layout();
	test_log_levels_and_facilities();
	test_log_ring_header();
	test_log_geom();
	test_log_device_abi();
	test_log_ioctl_values();
	test_log_record_roundtrip();

	airy_test_report("test_log_ring_uapi");
	return EXIT_SUCCESS;
}
