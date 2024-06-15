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

void ElfReader::PrintHeaderInfo() {
  ElfHeader_t *elf_header = (ElfHeader_t *)buffer;
  std:: cout << "Class:          " << bwMap.at(elf_header->bitWidth) << std::endl;
  std:: cout << "Data:           " << endianMap.at(elf_header->endian) << std::endl;
  std:: cout << "Type:           " << typeMap.at(elf_header->type) << std::endl;
  std:: cout << "Entry Address:  0x" << std::hex << elf_header->entry << std::dec << std::endl;
}
