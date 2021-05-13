#ifndef __ELF_UTIL_H__
#define __ELF_UTIL_H__

#include <string>
#include <vector>

#include <elf.h>

enum class ElfArchType {
  ELF_ARCH_64BIT,
  ELF_ARCH_32BIT,
  ELF_ARCH_UNKNOWN
};

typedef struct {
  Elf64_Ehdr *ehdr;
  Elf64_Phdr *phdr;
  Elf64_Shdr *shdr;
} Elf64Info;

typedef struct {
  Elf32_Ehdr *ehdr;
  Elf32_Phdr *phdr;
  Elf32_Shdr *shdr;
} Elf32Info;

class ElfInfo {
  private:
    int _fd;
    uint8_t *_mem;
    std::string _fileName;
    ElfArchType _arch_type;
    Elf64Info _elf64;
    Elf32Info _elf32;

    void loadMemoryMap(void);
    void loadElfInfo(void);
    bool isELF(const uint8_t *mem);

  public:
    ElfInfo() = delete;
    ElfInfo(const std::string &fileName);
    ~ElfInfo();

    std::string getFileName(void);
    ElfArchType getArchType(void);
    std::vector<std::string> getDependency(void);
    std::string getAbsolutePath(void);
};

#endif /* __ELF_UTIL_H__ */
