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

typedef struct ElfSectionHeader_s {
    uint32_t name;
    uint32_t type;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addrAlign;
    uint64_t entSize;
} ElfSectionHeader_t;

typedef struct ElfSymbol_s {
    uint32_t name;
    uint8_t info;
    uint8_t other;
    uint16_t shndx; // in which section
    uint64_t value; // value or address associated with the symbol
    uint64_t size;
} ElfSymbol_t;

class ElfReader {
    uint64_t fileSize = 0;
    uint8_t *buffer = nullptr;

    ElfSectionHeader_t *strTab = nullptr;
    ElfSectionHeader_t *sybTab = nullptr;

    std::vector<ElfProgramHeader_t *> phHeaders;

    std::vector<ElfSymbol_t *> syms;
public:
    ElfReader(const char *file_name);
    ~ElfReader();

    bool CheckElf();
    bool IsStaticExe();
    void PrintElfInfo();
    std::vector<ElfProgramHeader_t *>::const_iterator phbegin() const {
      return phHeaders.cbegin();
    }
    std::vector<ElfProgramHeader_t *>::const_iterator phend() const {
      return phHeaders.cend();
    }

    void *GetEntryPoint() {
        ElfHeader_t *elf_header = (ElfHeader_t *)buffer;
      return (void *)elf_header->entry;
    }

    uint8_t *GetBuffer() {
        return buffer;
    }
};
