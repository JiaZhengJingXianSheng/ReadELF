// Create by LiuYuZhao
// 2021/11/28
#pragma once

#include <cstdint>
#include <string>

namespace ELF
{

    class ELF_reader
    {
    public:
        ELF_reader(const std::string &file_path);
        void loadFile(const std::string &file_path);
        void showHeader();
        void showSectionHeaders();
        void showSymbols();

    private:
        void load();
        void init(std::string file_path = std::string(),
                  int fd = -1,
                  std::size_t length = 0,
                  std::uint8_t *programMMap = nullptr);

        std::string Path;
        int fd;
        std::size_t length;
        std::uint8_t *programMMap;
    };

}
