#include <stdio.h>
#include <iostream>
#include "elf_reader.h"
#include "cmdline.h"
#include "elf_loader.h"


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
    ElfLoader loader(&reader);

    if (!reader.CheckElf()) {
        std::cerr << "Unsupported Elf format" << std::endl;
        return -1;
    }

    if (cmdParser.exist("info")) {
        reader.PrintElfInfo();
    }

    if (cmdParser.exist("exec")) {
      // execuate the elf
      if (!loader.LoadSegment()) {
        std::cerr << "Failed to load segment" << std::endl;
        return -1;
      }
      if (!loader.AllocateStack()) {
        std::cerr << "Failed to allocate a stack" << std::endl;
        return -1;
      }
      char **exec = (char **)malloc(sizeof(char *) * cmdParser.rest().size());
      for (int i = cmdParser.rest().size() - 1; i < argc; ++i) {
        exec[i] = argv[i];
      }
      loader.Exec(cmdParser.rest().size(), exec, nullptr);
    }

    return 0;
}