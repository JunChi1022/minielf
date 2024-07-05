#pragma once
#include <string>
#include "elf_reader.h"

class ElfLoader {
  ElfReader *reader;

  void *stack = nullptr;

  void Push64(uint64_t val);
  void PushString(std::string str);
public:
  ElfLoader(ElfReader * reader);
  ~ElfLoader() {}

  bool LoadSegment();
  bool AllocateStack();
  int Exec(int argc, char *argv[], char *envv[]);
};