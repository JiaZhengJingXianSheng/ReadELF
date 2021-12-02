// Create by LiuYuZhao
// 2021/11/28

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "ELF_reader.h"

namespace ELF
{

    ELF_reader::ELF_reader(const std::string &file_path)
        : Path(file_path)
    {
        load();
    }

    void ELF_reader::loadFile(const std::string &path_name)
    {
        Path = path_name;
        load();
    }

    void ELF_reader::showHeader()
    {
        const Elf64_Ehdr *header;
        header = reinterpret_cast<Elf64_Ehdr *>(programMMap);

        printf("Magic:\t\t\t\t\t");
        for (int i = 0; i < 16; ++i)
        {
            printf("%02x ", header->e_ident[i]);
        }
        printf("\n");

        printf("类型:\t\t\t\t\t");
        switch (header->e_ident[4])
        {
        case 0:
            printf("无效\n");
            break;
        case 1:
            printf("ELF32\n");
            break;
        case 2:
            printf("Elf64\n");
            break;
        default:
            printf("错误\n");
            break;
        }

        printf("数据存储方式:\t\t\t\t");
        switch (header->e_ident[5])
        {
        case 0:
            printf("未知格式\n");
            break;
        case 1:
            printf("二进制补码 小端存储\n");
            break;
        case 2:
            printf("二进制补码 大端存储\n");
            break;
        default:
            printf("错误\n");
            break;
        }

        printf("版本:\t\t\t\t\t");
        printf("%d\n", (int)header->e_ident[8]);

        printf("运行平台:\t\t\t\t");
        switch (header->e_ident[EI_OSABI])
        {
        case 0:
            printf("UNIX System V ABI\n");
            break;
        case 1:
            printf("HP-UX ABI\n");
            break;
        case 2:
            printf("NetBSD ABI\n");
            break;
        case 6:
            printf("Solaris ABI\n");
            break;
        case 8:
            printf("IRIX ABI\n");
            break;
        case 9:
            printf("FreeBSD ABI\n");
            break;
        case 10:
            printf("TRU64 UNIX ABI\n");
            break;

        default:
            printf("未知 ABI\n");
            break;
        }

        printf("类型:\t\t\t\t\t");
        switch (header->e_type)
        {
        case 0:
            printf("未知\n");
            break;
        case 1:
            printf("可重定位文件\n");
            break;
        case 2:
            printf("可执行文件\n");
            break;
        case 3:
            printf("共享文件\n");
            break;
        case 4:
            printf("core文件\n");
            break;
        default:
            printf("错误\n");
            break;
        }

        printf("设备:\t\t\t\t\t");
        switch (header->e_machine)
        {
        case 0:
            printf("未知\n");
            break;
        case 3:
            printf("Intel 80386\n");
            break;
        case 7:
            printf("Intel 80860\n");
            break;
        case 20:
            printf("PowerPC\n");
            break;
        case 21:
            printf("PowerPC 64-bit\n");
            break;
        case 40:
            printf("Advanced RISC Machines\n");
            break;
        case 50:
            printf("Intel Itanium\n");
            break;
        case 62:
            printf("AMD x86-64\n");
            break;
        default:
            printf("错误\n");
            break;
        }

        printf("版本:\t\t\t\t\t");
        switch (header->e_ident[6])
        {
        case 0:
            printf("无效版本\n");
            break;
        case 1:
            printf("通用版本\n");
            break;
        default:
            printf("错误\n");
            break;
        }

        printf("入口地址:\t\t\t\t");
        printf("0x%lx\n", header->e_entry);

        printf("程序偏移:\t\t\t\t");
        printf("%ld (bytes)\n", header->e_phoff);

        printf("段偏移:\t\t\t\t\t");
        printf("%ld (bytes)\n", header->e_shoff);

        printf("标志位:\t\t\t\t\t");
        printf("0x%x\n", header->e_flags);

        printf("文件头大小:\t\t\t\t");
        printf("%d (bytes)\n", header->e_ehsize);

        printf("程序头大小:\t\t\t\t");
        printf("%d (bytes)\n", header->e_phentsize);

        printf("程序头数量:\t\t\t\t");
        printf("%d\n", header->e_phnum);

        printf("段表描述符大小:\t\t\t\t");
        printf("%d (bytes)\n", header->e_shentsize);

        printf("段表描述符数量:\t\t\t\t");
        printf("%d\n", header->e_shnum);

        printf("段表字符串表下标:\t\t\t");
        switch (header->e_shstrndx)
        {
        case 0:
            printf("未知\n");
            break;
        default:
            printf("%u\n", header->e_shstrndx);
            break;
        }
    }

    void ELF_reader::showSectionHeaders()
    {
        const Elf64_Ehdr *header;
        const Elf64_Shdr *sTable;
        const char *sSTable; // section string table
        size_t sSTableIndex;
        Elf64_Xword sNumber;

        header = reinterpret_cast<Elf64_Ehdr *>(programMMap);
        sTable = reinterpret_cast<Elf64_Shdr *>(programMMap + header->e_shoff);

        sSTableIndex = header->e_shstrndx;
        sSTable = reinterpret_cast<char *>(&programMMap[sTable[sSTableIndex].sh_offset]);

        sNumber = reinterpret_cast<Elf64_Shdr *>(&programMMap[header->e_shoff])->sh_size;
        if (sNumber == 0)
        {
            sNumber = header->e_shnum;
        }

        printf("There are %ld section header%s, starting at offset 0x%lx:\n\n", sNumber,
               sNumber > 1 ? "s" : "", header->e_shoff);
        printf("[Nr]\t名称\t\t\t类型\t\t地址\t\t\t偏移地址\t大小\t\t\tEntry尺寸\t\t标志位\t索引值\t信息\t对齐长度\n");
        for (decltype(sNumber) i = 0; i < sNumber; ++i)
        {
            printf("[%2lu]\t", i);

            printf("%-16.16s\t", sSTable + sTable[i].sh_name);

            switch (sTable[i].sh_type)
            {

            case 0:
                printf("NULL\t\t");
                break;

            case 1:
                printf("PROGBITS\t");
                break;

            case 2:
                printf("SYMTAB\t\t");
                break;
            case 3:
                printf("STRTAB\t\t");
                break;
            case 4:
                printf("RELA\t\t");
                break;
            case 5:
                printf("HASH\t\t");
                break;
            case 6:
                printf("DYNSYM\t\t");
                break;

            case 7:
                printf("NOTE\t\t");
                break;

            case 8:
                printf("NOBITS\t\t");
                break;

            case 9:
                printf("REL\t\t");
                break;

            case 10:
                printf("SHLIB\t\t");
                break;

            case 11:
                printf("DYNSYM\t\t");
                break;

            case 14:
                printf("INIT_ARRAY\t");
                break;

            case 15:
                printf("FINIT_ARRAY\t");
                break;

            case 16:
                printf("PREINIT_ARRAY\t");
                break;

            case 17:
                printf("GROUP\t\t");
                break;

            case 18:
                printf("SYMTAB_SHNDX\t");
                break;

            default:
                printf("Unknown\t\t");
                break;
            }

            printf("%016lx\t", sTable[i].sh_addr);
            printf("%08lx\t", sTable[i].sh_offset);
            printf("%016lx\t", sTable[i].sh_size);
            printf("%016lx\t", sTable[i].sh_entsize);
            printf("0x%02lx\t", sTable[i].sh_flags);
            printf("%4d\t", sTable[i].sh_link);
            printf("%4d\t", sTable[i].sh_info);
            printf("%4lu\n", sTable[i].sh_addralign);
        }
        printf("Key to Flags:\n"
               "  W (write), A (alloc), X (execute), M (merge), S (strings), l (large)\n"
               "  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)\n"
               "  O (extra OS processing required) o (OS specific), p (processor specific)\n");
    }

    void ELF_reader::showSymbols()
    {
        const Elf64_Ehdr *header;
        const Elf64_Shdr *sTable; // section table
        const Elf64_Sym *symbolTable;
        const char *symbolSTable; // symbol string table
        const char *sSTable;      // section string table
        Elf64_Xword sectionNumber;
        std::size_t symbolENumber; // symbol entry number

        header = reinterpret_cast<Elf64_Ehdr *>(programMMap);
        sTable = reinterpret_cast<Elf64_Shdr *>(programMMap + header->e_shoff);
        sSTable = reinterpret_cast<char *>(programMMap + sTable[header->e_shstrndx].sh_offset);
        sectionNumber = reinterpret_cast<Elf64_Shdr *>(&programMMap[header->e_shoff])->sh_size;
        if (sectionNumber == 0)
        {
            sectionNumber = header->e_shnum;
        }

        for (decltype(sectionNumber) i = 0; i < sectionNumber; ++i)
        {
            if (sTable[i].sh_type == SHT_SYMTAB || sTable[i].sh_type == SHT_DYNSYM)
            {
                symbolTable = reinterpret_cast<Elf64_Sym *>(&programMMap[sTable[i].sh_offset]);
                symbolENumber = sTable[i].sh_size / sTable[i].sh_entsize;
                symbolSTable = reinterpret_cast<char *>(&programMMap[sTable[i + 1].sh_offset]);

                printf("Symbol table '%s' contain %lu %s:\n", &sSTable[sTable[i].sh_name], symbolENumber, "entries");
                printf("Num:\tValue\t\t\tSize\tType\t\tBind\t\tVis\t\tNdx\tName\n");
                for (decltype(symbolENumber) i = 0; i < symbolENumber; ++i)
                {
                    printf("%lu\t%016lx\t%lu\t", i, symbolTable[i].st_value, symbolTable[i].st_size);
                    switch (ELF64_ST_TYPE(symbolTable[i].st_info))
                    {
                    case 0:
                        printf("NOTYPE\t\t");
                        break;
                    case 1:
                        printf("OBJECT\t\t");
                        break;
                    case 2:
                        printf("FUNC\t\t");
                        break;
                    case 3:
                        printf("SECTION\t\t");
                        break;
                    case 4:
                        printf("FILE\t\t");
                        break;
                    case 5:
                        printf("COMMON\t\t");
                        break;
                    case 6:
                        printf("TLS\t\t");
                        break;
                    default:
                        printf("未知\t\t");
                        break;
                    }

                    switch (ELF64_ST_BIND(symbolTable[i].st_info))
                    {
                    case 0:
                        printf("LOCAL\t\t");
                        break;
                    case 1:
                        printf("GLOBAL\t\t");
                        break;
                    case 2:
                        printf("WEAK\t\t");
                        break;
                    default:
                        printf("未知\t\t");
                    }

                    switch (ELF64_ST_VISIBILITY(symbolTable[i].st_other))
                    {
                    case 0:
                        printf("DEFAULT\t\t");
                        break;
                    case 1:
                        printf("INTERNAL\t");
                        break;
                    case 2:
                        printf("HIDDEN\t\t");
                        break;
                    case 3:
                        printf("PROTECTED\t");
                        break;
                    default:
                        printf("未知\t\t");
                        break;
                    }

                    switch (symbolTable[i].st_shndx)
                    {
                    case SHN_ABS:
                        printf("ABS\t");
                        break;
                    case SHN_COMMON:
                        printf("COM\t");
                        break;
                    case SHN_UNDEF:
                        printf("UND\t");
                        break;
                    default:
                        printf("%3d\t", symbolTable[i].st_shndx);
                        break;
                    }

                    printf("%.25s\n", &symbolSTable[symbolTable[i].st_name]);
                }
            }
        }
    }

    void ELF_reader::load()
    {
        void *mmap_res;
        struct stat st;

        fd = open(Path.c_str(), O_RDONLY);
        fstat(fd, &st);

        length = static_cast<std::size_t>(st.st_size);

        mmap_res = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);

        programMMap = static_cast<std::uint8_t *>(mmap_res);
    }

    void ELF_reader::init(std::string Path, int fd, std::size_t length, std::uint8_t *programMMap)
    {
        this->Path = std::move(Path);
        this->fd = fd;
        this->length = length;
        this->programMMap = programMMap;
    }

}