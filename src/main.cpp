#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "elf_reader.h"
#include "cmdline.h"


int main(int argc, char *argv[]) {
    cmdline::parser cmdParser;
    cmdParser.add("info", 'i', "print header info for input elf");

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
        reader.PrintHeaderInfo();
    }

    return 0;
}