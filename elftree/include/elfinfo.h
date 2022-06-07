#ifndef __ELF_INFO_H__
#define __ELF_INFO_H__

#include <list>
#include <string>
#include <vector>

#include <elf.h>

#include "elf_arch.h"

enum class ElfArchType {
  ELF_ARCH_UNKNOWN = ELFCLASSNONE,
  ELF_ARCH_32BIT = ELFCLASS32,
  ELF_ARCH_64BIT = ELFCLASS64
};

class ElfInfo {
  private:
    int _fd;
    uint8_t *_mem;
    off_t _size;
    std::string _filePath;
    std::string _fileName;
    ElfArchType _archType;
    ElfLowInfo _elf;

    void loadMemoryMap(void);
    void loadElfInfo(void);
    bool isELF(const uint8_t *mem);
    Elf_Shdr* getSectionHeader(std::string sectionName) const;
    std::string getDump(unsigned int offset, unsigned int size) const;

  public:
    ElfInfo() = delete;
    explicit ElfInfo(const std::string& filePath);
    ~ElfInfo();

    std::string getFileName(void) const;
    ElfArchType getArchType(void) const;
    off_t getELFSize(void) const;
    std::list<std::string> getDependency(void) const;
    std::list<std::string> getSectionList(void) const;

    std::string getElfHeaderFormat(void) const;
    std::string getProgramHeaderFormat(void) const;
    std::string getSectionHeaderFormat(void) const;
    std::string getSectionDumpFormat(const std::string& sectionName) const;
    std::string getMemDumpFormat(const std::string& address, unsigned int size) const;
};

#endif /* __ELF_INFO_H__ */
