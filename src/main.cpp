// Create by LiuYuZhao
// 2021/11/28

#include "ELF_reader.h"
#include <string>
#include <string.h>
int main(int argc, char *argv[])
{
    using ELF::ELF_reader;
    std::string path = argv[2];
    ELF_reader s(path);
    if (!strcmp(argv[1], "-h"))
        s.showHeader();

    if (!strcmp(argv[1], "-S"))
        s.showSectionHeaders();

    if (!strcmp(argv[1], "-s"))
        s.showSymbols();
    return 0;
}
