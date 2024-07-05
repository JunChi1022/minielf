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
  if (stack == MAP_FAILED) {
    return false;
  }
  stack = (void *)((uint8_t *)stack + 1024 * 1024);
  return true;
}

void ElfLoader::Push64(uint64_t val) {
  uint64_t *sp = (uint64_t *)stack;
  *(--sp) = val;
  stack = (void *)sp;
}

void ElfLoader::PushString(std::string str) {
  char *sp = (char *)stack;
  for (int i = 0; i < str.size(); i++) {
    *sp = str[i];
    sp--;
  }
  stack = (void *)sp;
}

static int GetEnvc(char *envv[]) {
  int c = 0;
  while (envv[c] != NULL) {
    c++;
  }
  return c;
}

extern "C"
int prolog(void *entry, void *stack, int argc, char *argv[], char *envv[]);

int ElfLoader::Exec(int argc, char *argv[], char *envv[]) {
  // initialize the stack accoding to the systemv abi documentation
  Push64(0);
  PushString(argv[0]);
  std::vector<uint64_t> envps;
  for (int i = GetEnvc(envv) - 1; i >= 0; --i) {
    PushString(envv[i]);
    envps.push_back((uint64_t)stack);
  }
  std::vector<uint64_t> argvps;
  for (int i = argc - 1; i >= 0; --i) {
    PushString(argv[i]);
    argvps.push_back((uint64_t)stack);
  }
  // align the stack (16 bytes)
  stack = (void *)((uint64_t)stack & ~0x7);

  PushString("x86_64");
  uint64_t pPlatform = (uint64_t)stack;
  stack = (void *)((uint64_t)stack & ~0x7);

  // Auxiliary Vector
  Push64(0); Push64(0); // AT_NULL
  Push64(0x1000); Push64(6); // AT_PAGESZ
  Push64(0); Push64(8); // AT_FLAGS
  Push64((uint64_t)reader->GetEntryPoint()); Push64(9); // AT_ENTRY
  Push64(pPlatform); Push64(15); // AT_PLATFORM
  Push64(1 << 0                  // fpu
         | 1 << 4                // rdtsc
         | 1 << 8                // cmpxchg8
         | 1 << 11               // sep (sysenter & sysexit)
         | 1 << 15               // cmov
         | 1 << 19               // clflush (seems to be with SSE2)
         | 1 << 23               // mmx
         | 1 << 24               // fxsr (fxsave, fxrestore)
         | 1 << 25               // SSE
         | 1 << 26               // SSE2
         | 1 << 28               // hyper threading
         | 1 << 30               // ia64
  );
  Push64(16); // AT_HWCAP


  // nil
  Push64(0);
  // envs
  for (auto envp = envps.rbegin(); envp != envps.rend(); envp++) {
    Push64(*envp);
  }
  // nil
  Push64(0);
  // args
  for (auto argvp = argvps.rbegin(); argvp != argvps.rend(); argvp++) {
    Push64(*argvp);
  }
  // argc
  Push64(argc);

  prolog(reader->GetEntryPoint(), stack, argc, argv, envv);
  return 0;
}
