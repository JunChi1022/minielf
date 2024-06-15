#pragma once

#include <stdint.h>

typedef struct ElfHeader_s {
    uint8_t magic[4];
    uint8_t bitWidth;
    uint8_t endian;
    uint8_t version;
    uint8_t os_abi;
    uint64_t padding;
    uint16_t type;
    uint16_t machine;
    uint32_t version2;
    uint64_t entry;
    uint64_t phoff; // program header offset
    uint64_t shoff; // section header offset
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} ElfHeader_t;


class ElfReader {
    uint64_t fileSize = 0;
    uint8_t *buffer = nullptr;
public:
    ElfReader(const char *file_name);
    ~ElfReader();

    bool CheckElf();

    void PrintHeaderInfo();
};
