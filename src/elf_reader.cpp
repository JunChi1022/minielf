#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include "elf_reader.h"

const std::map<uint8_t, const char *> bwMap = {
    // {1, "32 bits"},
    {2, "64 bits"},
};

const std::map<uint8_t, const char *> endianMap = {
    {0x1, "little endian"},
    // {0x2, "big endian"},
};

const std::map<uint16_t, const char *> typeMap = {
    {0x0, "NONE"},
    {0x1, "REL"},
    {0x2, "EXEC"},
    {0x3, "DYN"},
    {0x4, "CORE"},
};

const std::map<uint32_t, const char *> phTypeMap = {
    {0x0, "NULL"},
    {0x1, "LOAD"},
    {0x2, "DYNAMIC"},
    {0x3, "INTERP"},
    {0x4, "NOTE"},
    {0x6, "PHDR"},
    {0x7, "TLS"},
    {0x6474e550, "GNU_EH_FRAME"},
    {0x6474e551, "GNU_STACK"},
    {0x6474e552, "GNU_RELRO"},
    {0x6474e553, "GNU_PROPERTY"},
};

ElfReader::ElfReader(const char *file_name) {
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }

    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);
    buffer = (uint8_t *)malloc(fileSize + 1);
    size_t read_size = fread(buffer, 1, fileSize, fp);
    if (read_size != fileSize) {
        perror("fread");
    } else {
        buffer[read_size] = '\0';
    }
    fclose(fp);

    // parse program header
    ElfHeader_t *elfHeader = (ElfHeader_t *)buffer;
    for (int i = 0; i < elfHeader->phnum; ++i) {
      phHeaders.push_back((ElfProgramHeader_t *)(buffer + elfHeader->phoff +
                                                 i * elfHeader->phentsize));
    }
}

ElfReader::~ElfReader() {
    free(buffer);
    buffer = nullptr;
    fileSize = 0;
}

bool ElfReader::CheckElf() {
  ElfHeader_t *elf_header = (ElfHeader_t *)buffer;

  // check magic
  if (elf_header->magic[0] != 0x7f || elf_header->magic[1] != 'E' ||
      elf_header->magic[2] != 'L' || elf_header->magic[3] != 'F') {
    return false;
  }

  return true;
}

static std::string PrintElfFlags(uint32_t flags) {
    std::string res = "";
    if (flags & 0x4) {
        res += "R";
    }
    if (flags & 0x2) {
        res += "W";
    }
    if (flags & 0x1) {
        res += "X";
    }
    return res;
}

void ElfReader::PrintElfInfo() {
  // elf header
  std::cout << "ELF Header:" << std::endl;
  ElfHeader_t *elf_header = (ElfHeader_t *)buffer;
  std::cout << "Class:          " << bwMap.at(elf_header->bitWidth) << std::endl;
  std::cout << "Data:           " << endianMap.at(elf_header->endian) << std::endl;
  std::cout << "Type:           " << typeMap.at(elf_header->type) << std::endl;
  std::cout << "Entry Address:  0x" << std::hex << elf_header->entry << std::dec << std::endl;

  // program header
  std::cout << std::endl << "Program headers:" << std::endl;
  for (size_t i = 0; i < phHeaders.size(); i++) {
    std::cout << "[" << i << "]:" << std::endl;
    std::cout << "Type:   " << phTypeMap.at(phHeaders[i]->type) << "\t"
              << "Offset: " << std::hex << phHeaders[i]->offset << std::dec
              << "\t" << "VAddr:  " << std::hex << phHeaders[i]->vaddr
              << std::dec << "\t" << "PAddr:  " << std::hex
              << phHeaders[i]->paddr << std::dec << std::endl;
    std::cout << "FileSz: " << std::hex << phHeaders[i]->filesz << std::dec
              << "\t" << "MemSz:  " << std::hex << phHeaders[i]->memsz
              << std::dec << "\t"
              << "Flags:  " << PrintElfFlags(phHeaders[i]->flags) << "\t"
              << "Align:  " << std::hex << phHeaders[i]->align << std::dec
              << std::endl;
  }
}
