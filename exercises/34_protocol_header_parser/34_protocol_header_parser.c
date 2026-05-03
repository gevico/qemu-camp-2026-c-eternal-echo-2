#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
 * 14 紧凑网络协议头解析器（位域）
 * 要求：
 *  - 协议头格式：2 字节版本（4 位主版本 + 4 位次版本）、2 字节长度（16 位无符号）、1 字节标志（3 位保留 + 5
 * 位功能标志）。
 *  - 输入示例：\x00\x03\x00\x20\x00  →  version:0.3, length:32, flags:0x00
 *  - 考察点：位域定义、结构体对齐（#pragma pack(1)）、字节流解析
 */

/* 结构体对齐：按 1 字节对齐，确保与网络字节流逐字节一致 */
#pragma pack(push, 1)

/*
 * 原始协议头（与网络字节流逐字节对应，不直接使用位域跨字节）
 */
typedef struct {
    uint8_t ver_major;     /* 主版本号 */
    uint8_t ver_minor;     /* 次版本号 */
    uint16_t length_be;    /* 网络序长度 */
    uint8_t flags_raw;     /* 标志字节 */
} proto_header_raw_t;

/*
 * 位域定义的逻辑视图（使用位域表达语义，不强行与字节流直接映射）
 * 注意：位域在不同平台的位序实现可能不同，故此处不作为内存映射，仅用于展示语义并由解析代码赋值。
 */
typedef struct {
    unsigned int ver_major:4;   /* 主版本号 */
    unsigned int ver_minor:4;   /* 次版本号 */
    unsigned int reserved:3;    /* 保留位 */
    unsigned int flags:5;       /* 功能标志 */
    uint16_t length;            /* 长度 (主机序) */
} proto_header_bits_t;

#pragma pack(pop)

/*
 * 将网络序（大端）的 16 位数转换为主机序
 */
static uint16_t be16_to_cpu(uint16_t be) {
    return ((be & 0xFF) << 8) | ((be >> 8) & 0xFF);
}

int main(void) {
    /* 测试输入字节流：00 03 00 20 00 */
    const uint8_t stream[5] = {0x00, 0x03, 0x00, 0x20, 0x00};

    /* 将字节流复制到与其逐字节匹配的原始头结构体中 */
    proto_header_raw_t raw = {0};
    memcpy(&raw, stream, sizeof(raw));

    /* 解析版本号：题目定义”4 位主版本 + 4 位次版本”，每个版本占1字节 */
    proto_header_bits_t view;
    view.ver_major = raw.ver_major;
    view.ver_minor = raw.ver_minor;

    /* 解析长度：网络序 16 位 */
    uint16_t length = be16_to_cpu(raw.length_be);

    /* 使用位域结构体表达（非内存映射，仅用于说明位域解析规则） */
    view.reserved = (raw.flags_raw >> 5) & 0x07;
    view.flags = raw.flags_raw & 0x1F;
    view.length = length;

    /* 期望输出：version:0.3, length:32, flags:0x00 */
    printf("version:%u.%u, length:%u, flags:0x%02X\n", view.ver_major, view.ver_minor, view.length, view.flags & 0xFFu);

    return 0;
}
