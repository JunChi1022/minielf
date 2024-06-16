#pragma once

#include <stdint.h>
#include <vector>

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
    uint16_t phentsize; // size of program header entry
    uint16_t phnum; // number of program header
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} ElfHeader_t;

typedef struct ElfProgramHeader_s {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
} ElfProgramHeader_t;

class ElfReader {
    uint64_t fileSize = 0;
    uint8_t *buffer = nullptr;

    std::vector<ElfProgramHeader_t *> phHeaders;
public:
    ElfReader(const char *file_name);
    ~ElfReader();

    bool CheckElf();
    void PrintElfInfo();
};
