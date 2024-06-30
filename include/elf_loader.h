#pragma once
#include "elf_reader.h"

class ElfLoader {
  ElfReader *reader;

  void *stack = nullptr;
public:
  ElfLoader(ElfReader * reader);
  ~ElfLoader() {}

  bool LoadSegment();
  bool AllocateStack();
  int Exec(int argc, char *argv[], char *envv[]);
};