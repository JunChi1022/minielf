#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "elf_loader.h"

ElfLoader::ElfLoader(ElfReader *reader) : reader(reader) {}

static bool LoadSegmentStatic(ElfProgramHeader_t ph, uint8_t *buffer) {
  // map for all load segment
  int prot = 0;
  if (ph.flags & 0x1) {
    prot |= PROT_EXEC;
  }
  if (ph.flags & 0x2) {
    prot |= PROT_WRITE;
  }
  if (ph.flags & 0x4) {
    prot |= PROT_READ;
  }
  uint64_t vaddr = ph.vaddr;
  vaddr = vaddr & ~0xfff; // align to pagesize(0x1000)
  uint64_t padding = ph.vaddr - vaddr;
  vaddr = (uint64_t)mmap((void *)vaddr, ph.memsz + padding, PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
  if ((void *)vaddr == MAP_FAILED) {
    perror("mmap");
    return false;
  }

  memcpy((void *)ph.vaddr, (void *)(buffer + ph.offset), ph.memsz);

  int ret = mprotect((void *)vaddr, ph.memsz + padding, prot);
  if (ret != 0) {
    perror("mprotect");
    return false;
  }

  return true;
}

bool ElfLoader::LoadSegment() {
  if (reader->IsStaticExe()) {
    // for static linked exe, use the vaddr directly
    for (auto ph = reader->phbegin(); ph != reader->phend(); ph++) {
      if ((*ph)->type != 0x1) {
        // is not a load segment
        continue;
      }
      if (!LoadSegmentStatic(**ph, reader->GetBuffer())) {
        return false;
      }
    }
  }

  return true;
}

bool ElfLoader::AllocateStack() {
  // Just use 1024 * 1024 stack size
  stack = mmap(NULL, 1024 * 1024, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
  return stack != MAP_FAILED;
}

extern "C"
int prolog(void *entry, void *stack, int argc, char *argv[], char *envv[]);

int ElfLoader::Exec(int argc, char *argv[], char *envv[]) {
  prolog(reader->GetEntryPoint(), stack, argc, argv, envv);
  return 0;
}
