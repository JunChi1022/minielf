#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include "elf_reader.h"
#include "cmdline.h"

static int LoadSegment(ElfProgramHeader_t ph, uint8_t *buffer) {
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
    return -1;
  }

  memcpy((void *)ph.vaddr, (void *)(buffer + ph.offset), ph.memsz);

  int ret = mprotect((void *)vaddr, ph.memsz + padding, prot);
  if (ret != 0) {
    perror("mprotect");
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
    cmdline::parser cmdParser;
    cmdParser.add("info", 'i', "print infomation for input elf");
    cmdParser.add("exec", 'e', "execuate this elf");

    cmdParser.parse_check(argc, argv);

    if (cmdParser.rest().size() < 1) {
        std::cerr << "No input" << std::endl;
        return -1;
    }

    ElfReader reader(cmdParser.rest()[0].c_str());

    if (!reader.CheckElf()) {
        std::cerr << "Unsupported Elf format" << std::endl;
        return -1;
    }

    if (cmdParser.exist("info")) {
        reader.PrintElfInfo();
    }

    if (cmdParser.exist("exec")) {
      // execuate the elf
      if (reader.IsStaticExe()) {
        // for static linked exe, use the vaddr directly
        for (auto ph = reader.phbegin(); ph != reader.phend(); ph++) {
          if ((*ph)->type != 0x1) {
            // is not a load segment
            continue;
          }
          LoadSegment(**ph, reader.GetBuffer());
        }
        // jump to entry point
        ((void (*)())reader.GetEntryPoint())();
      } else {
        return -1;
      }
    }

    return 0;
}