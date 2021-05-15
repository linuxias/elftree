#include <string>
#include <stdexcept>
#include <list>
#include <iostream>

#include <boost/filesystem.hpp>

#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <elf.h>
#include <unistd.h>
#include <errno.h>

#include "elf_util.h"

bool ElfInfo::isELF(const uint8_t *mem)
{
#define FIRST_MAGIC_STRING 0x7f
#define ELF_STRING "ELF"

    if (mem[0] != FIRST_MAGIC_STRING && strcmp((char *)(&mem[1]), ELF_STRING)) {
      fprintf(stderr, "Not an ELF");
      return false;
    }

    return true;
}

void ElfInfo::loadMemoryMap(void)
{
	struct stat st;
	errno = 0;
	if (fstat(_fd, &st) < 0) {
		perror("fstat");
		exit(-1);
	}

	_mem = static_cast<uint8_t *>(mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, _fd, 0));
	if (_mem == MAP_FAILED) {
		perror("mmap");
		exit(-1);
	}
}

void ElfInfo::loadElfInfo(void)
{
  char buf[EI_NIDENT];
  ssize_t size = read(_fd, buf, EI_NIDENT);
  if (size == 0 || size < EI_NIDENT) {
    perror("read");
    exit(-1);
  }

  if (buf[EI_CLASS] == ELFCLASS32) {
    _elf32.ehdr = (Elf32_Ehdr *)_mem;
    _elf32.phdr = (Elf32_Phdr *)&(_mem[_elf32.ehdr->e_phoff]);
    _elf32.shdr = (Elf32_Shdr *)&(_mem[_elf32.ehdr->e_shoff]);
  } else if (buf[EI_CLASS] == ELFCLASS64) {
    _elf64.ehdr = (Elf64_Ehdr *)_mem;
    _elf64.phdr = (Elf64_Phdr *)&(_mem[_elf64.ehdr->e_phoff]);
    _elf64.shdr = (Elf64_Shdr *)&(_mem[_elf64.ehdr->e_shoff]);
  } else {
    /* TODO : Process unknown class exception*/
  }
  _arch_type = (ElfArchType)buf[EI_CLASS];
}

ElfInfo::ElfInfo(const std::string filePath) :
  _filePath(filePath), _arch_type(ElfArchType::ELF_ARCH_UNKNOWN)
{
  errno = 0;
  _fd = open(_filePath.c_str(), O_RDONLY);
  if (errno < 0 || _fd == -1) {
    perror("open");
    exit(-1);
  }

  boost::filesystem::path p(filePath);
  _dirPath = p.parent_path().string();
  _fileName = p.filename().string();

  loadMemoryMap();
  if (isELF(_mem) == false) {
    throw std::invalid_argument("This file is not ELF");
  }

  loadElfInfo();
}

std::list<std::string> ElfInfo::getDependency(void) {
  std::list<std::string> deps;
  int i;
  unsigned char *string_table;
  Elf64_Ehdr *ehdr = _elf64.ehdr;
  Elf64_Shdr *shdr = _elf64.shdr;

  string_table = &_mem[shdr[ehdr->e_shstrndx].sh_offset];
  unsigned char *dynstr= NULL;
  for (i = 0; i < ehdr->e_shnum; i++) {
    if (strcmp((char *)&string_table[shdr[i].sh_name], ".dynstr") == 0) {
      dynstr = (unsigned char *)&_mem[shdr[i].sh_offset];
    }
  }

  for (i = 0; i < ehdr->e_shnum; i++) {
    if (strcmp((char *)&string_table[shdr[i].sh_name], ".dynamic") == 0) {
      unsigned char *addr = &_mem[shdr[i].sh_offset];
      int count = shdr[i].sh_size / sizeof(Elf64_Dyn);
      for (int j = 0; j < count; j++) {
        Elf64_Dyn *dyn = (Elf64_Dyn *)&addr[j * sizeof(Elf64_Dyn)];
        if (dyn->d_tag == DT_NEEDED) {
          deps.push_back(std::string((char *)&dynstr[dyn->d_un.d_val]));
        }
      }
    }
  }

  return deps;
}

ElfInfo::~ElfInfo(void) {
  if (_fd)
    close(_fd);
}

std::string ElfInfo::getFileName(void)
{
  return _fileName;
}

ElfArchType ElfInfo::getArchType(void)
{
  return _arch_type;
}

std::string ElfInfo::getAbsolutePath(void)
{

}
