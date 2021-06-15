#include <string>
#include <stdexcept>
#include <list>
#include <iostream>
#include <iomanip>
#include <sstream>

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

#include "elfinfo.h"

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
    if (errno == EMFILE)
      throw std::ios_base::failure("Failed to create elfinfo : "
          + std::string(strerror(errno)));
    else
      throw std::invalid_argument("This file is not exists");
  }

  boost::filesystem::path p(filePath);
  _dirPath = p.parent_path().string();
  _fileName = p.filename().string();

  _dirPath = boost::filesystem::absolute(_dirPath).string();

  loadMemoryMap();
  if (isELF(_mem) == false)
    throw std::invalid_argument("This file is not ELF");

  loadElfInfo();
}

ElfInfo::~ElfInfo() {
  if (_mem != nullptr) {
    struct stat st;
    errno = 0;
    if (fstat(_fd, &st) < 0) {
      perror("fstat");
      exit(-1);
    }

    munmap(_mem, st.st_size);
  }

  if (_fd)
    close(_fd);
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
  return _dirPath;
}

const std::string getElfHeaderData(auto& ehdr)
{
  switch (ehdr->e_ident[EI_CLASS]) {
    case ELFDATANONE:
      return "none";
    case ELFDATA2LSB:
      return "2's complement, little endian";
    case ELFDATA2MSB:
      return "2's complement, big endian";
    default:
      std::stringstream ss;
      ss << "<unknwon : " << std::hex << ehdr->e_ident[EI_CLASS] << ">";
      return ss.str();
  }
}

const std::string getElfHeaderClass(auto& ehdr)
{
  switch (ehdr->e_ident[EI_CLASS]) {
    case ELFCLASS32:
      return "ELF32";
    case ELFCLASS64:
      return "ELF64";
  }
  return "UNKNOWN : " + ehdr->e_ident[EI_CLASS];
}

const std::string getElfHeaderVersion(auto& ehdr)
{
  switch (ehdr->e_ident[EI_VERSION]) {
    case EV_CURRENT:
      return "current";
    default:
      std::stringstream ss;
      ss << "<unknwon : " << std::hex << ehdr->e_ident[EI_VERSION] << ">";
      return ss.str();
  }
}

const std::string getElfHeaderOSABI(auto& ehdr)
{
  switch (ehdr->e_ident[EI_OSABI]) {
    case ELFOSABI_NONE:
      return "UNIX - System V";
    case ELFOSABI_HPUX:
      return "UNIX - HP-UX";
    case ELFOSABI_LINUX:
      return "UNIX - GNU";
    case ELFOSABI_SOLARIS:
      return "UNIX - Solaris";
    case ELFOSABI_FREEBSD:
      return "UNIX - FreeBSD";
    case ELFOSABI_STANDALONE:
      return "Standalone App";
    case ELFOSABI_ARM_AEABI:
      return "ARM EABI";
    case ELFOSABI_ARM:
      return "ARM";
    default:
      std::stringstream ss;
      ss << "<unknwon : " << std::hex << ehdr->e_ident[EI_OSABI] << ">";
      return ss.str();
  }
}

const std::string getElfHeaderType(auto& ehdr)
{
  switch (ehdr->e_type) {
    case ET_NONE:
      return "NONE (None)";
    case ET_REL:
      return "REL (Relocatable file)";
    case ET_EXEC:
      return "EXEC (Executable file)";
    case ET_DYN:
      return "DYN (Shared object file)";
    case ET_CORE:
      return "CORE (core file)";
    default:
      std::stringstream ss;
      if (ehdr->e_type >= ET_LOOS && ehdr->e_type <= ET_HIOS)
        ss << "OS-specific <";
      else if (ehdr->e_type >=ET_LOPROC && ehdr->e_type <= ET_HIPROC)
        ss << "Processor-specific <";
      else
        ss << "<unknwon :";
      ss << std::hex << ehdr->e_type << ">";
      return ss.str();
  }
}

const std::string getElfHeaderMachine(auto& ehdr)
{
  switch(ehdr->e_machine) {
    case EM_NONE:
      return "None";
    case EM_ARM:
      return "ARM";
    case EM_AARCH64:
      return "AArch64";
    case EM_IA_64:
      return "Intel IA-64";
    case EM_X86_64:
      return "Advanced Micro Devices X86-64";
    default:
      std::stringstream ss;
      ss << "<unknwon : " << std::hex << ehdr->e_machine << ">";
      return ss.str();
  }
}

std::string ElfInfo::getElfHeaderFormat(void)
{
  std::stringstream output;
  auto*& ehdr = _elf64.ehdr;
  output << "File: " << _fileName << std::endl;
  output << "Class: " << getElfHeaderClass(ehdr) << std::endl;
  output << "Data : " << getElfHeaderData(ehdr) << std::endl;
  output << "Version : " << getElfHeaderVersion(ehdr) << std::endl;
  output << "OS/ABI : " << getElfHeaderOSABI(ehdr) << std::endl;
  output << "ABI Version : " << std::to_string(ehdr->e_ident[EI_ABIVERSION]) << std::endl;
  output << "Type : " << getElfHeaderType(ehdr) << std::endl;
  output << "Machine : " << getElfHeaderMachine(ehdr) << std::endl;
  output << "Entry point address : 0x" << std::hex << ehdr->e_entry << std::endl;
  output << "Start of program header : 0x" << std::hex << ehdr->e_phoff << std::endl;
  output << "Start of section header : 0x" << std::hex << ehdr->e_shoff << std::endl;
  output << "Flags : 0x" << std::hex << ehdr->e_flags << std::endl;
  output << "Size of this headers : " << std::dec << ehdr->e_ehsize << " (Byte)" << std::endl;
  output << "Size of program headers : " << std::dec << ehdr->e_phentsize << " (Byte)" << std::endl;
  output << "Number of program headers : " << std::dec << ehdr->e_phnum << std::endl;
  output << "Size of section headers : " << std::dec << ehdr->e_shentsize << " (Byte)" << std::endl;
  output << "Number of section headers : " << std::dec << ehdr->e_shnum << std::endl;
  output << "Section header string table index : " << std::dec << ehdr->e_shstrndx << std::endl;

  return output.str();
}

const std::string getSegmentTypeName(auto* phdr)
{
  switch(phdr->p_type) {
    case PT_PHDR:
      return "PHDR";
    case PT_INTERP:
      return "INTERP";
    case PT_LOAD:
      return "LOAD";
    case PT_DYNAMIC:
      return "DYNAMIC";
    case PT_NOTE:
      return "NOTE";
    case PT_TLS:
      return "TLS";
    case PT_GNU_EH_FRAME:
      return "GNU_EH_FRAME";
    case PT_GNU_STACK:
      return "GNU_STACK";
    case PT_GNU_RELRO:
      return "GNU_RELRO";
    default:
      std::stringstream ss;
      ss << "<unknwon : " << phdr->p_type << ">";
      return ss.str();
  }
}

const std::string getSegmentFlags(auto* phdr)
{
  char flags[4] = "   ";

  if (phdr->p_flags & PF_R)
    flags[0] = 'R';
  if (phdr->p_flags & PF_W)
    flags[1] = 'W';
  if (phdr->p_flags & PF_X)
    flags[2] = 'E';

  return std::string(flags);
}

std::string ElfInfo::getProgramHeaderFormat(void)
{
  std::stringstream output;
  auto* phdr = _elf64.phdr;

  output << "Program Header : " << std::endl;
  output << std::setw(6) << "Type";
  output << std::setw(17) << "Offset";
  output << std::setw(21) << "VirtAddr";
  output << std::setw(19) << "PhysAddr";
  output << std::endl;
  output << std::setw(24) << "FileSiz";
  output << std::setw(18) << "MemSiz";
  output << std::setw(17) << "Flag";
  output << std::setw(7) << "Align";
  output << std::endl;
  for (int i = 0; i < _elf64.ehdr->e_phnum; i++) {
    output << "  ";
    output << std::setw(14) << std::setfill(' ') << std::left;
    output << getSegmentTypeName(&phdr[i]);
    output << std::setw(0) <<" 0x" << std::setw(16) << std::setfill('0') << std::right;
    output << std::hex <<phdr[i].p_offset;
    output << std::setw(0) <<" 0x" << std::setw(16) << std::setfill('0') << std::right;
    output << std::hex << phdr[i].p_vaddr;
    output << std::setw(0) <<" 0x" << std::setw(16) << std::setfill('0') << std::right;
    output << std::hex << phdr[i].p_paddr;
    output << std::endl;
    output << "  ";
    output << std::setw(14) << std::setfill(' ') <<" " << std::right;
    output << std::setw(0) <<" 0x" << std::setw(16) << std::setfill('0') << std::right;
    output << std::hex <<phdr[i].p_filesz;
    output << std::setw(0) <<" 0x" << std::setw(16) << std::setfill('0') << std::right;
    output << std::hex << phdr[i].p_memsz;
    output << std::setw(5) << std::setfill(' ') << std::right;
    output << getSegmentFlags(&phdr[i]);
    output << std::setw(0) <<" 0x" << std::setw(16) << std::setfill(' ') << std::left;
    output << std::hex << phdr[i].p_align;
    output << std::endl;
  }

  return output.str();
}

template <typename T>
const std::string getShTypeString(T* shdr)
{
  switch(shdr->sh_type) {
    case SHT_NULL:
      return "NULL";
    case SHT_PROGBITS:
      return "PROGBITS";
    case SHT_SYMTAB:
      return "SYMTAB";
    case SHT_STRTAB:
      return "STRTAB";
    case SHT_RELA:
      return "RELA";
    case SHT_HASH:
      return "HASH";
    case SHT_DYNAMIC:
      return "DYNAMIC";
    case SHT_NOTE:
      return "NOTE";
    case SHT_NOBITS:
      return "NOBITS";
    case SHT_REL:
      return "REL";
    case SHT_SHLIB:
      return "SHLIB";
    case SHT_DYNSYM:
      return "DYNSYM";
    case SHT_INIT_ARRAY:
      return "INIT_ARRAY";
    case SHT_FINI_ARRAY:
      return "FINI_ARRAY";
    case SHT_PREINIT_ARRAY:
      return "PREINIT_ARRAY";
    case SHT_GROUP:
      return "GROUP";
    case SHT_SYMTAB_SHNDX:
      return "SYMTAB_SHNDX";
    case SHT_GNU_ATTRIBUTES:
      return "GNU_ATTRIBUTES";
    case SHT_GNU_HASH:
      return "GNU_HASH";
    case SHT_GNU_verdef:
      return "VERDEF";
    case SHT_GNU_verneed:
      return "VERNEED";
    case SHT_GNU_versym:
      return "VERSYM";
  }
  std::stringstream ss;
  ss << "<unknwon : " << shdr->sh_type << ">";
  return ss.str();
}

template <typename T>
const std::string getSectionFlags(T* shdr)
{
  std::string flag = "";
  if (shdr->sh_flags & SHF_WRITE)
    flag += "W";
  if (shdr->sh_flags & SHF_ALLOC)
    flag += "A";
  if (shdr->sh_flags & SHF_EXECINSTR)
    flag += "X";
  if (shdr->sh_flags & SHF_MERGE)
    flag += "M";
  if (shdr->sh_flags & SHF_STRINGS)
    flag += "S";
  if (shdr->sh_flags & SHF_INFO_LINK)
    flag += "I";
  if (shdr->sh_flags & SHF_LINK_ORDER)
    flag += "L";
  if (shdr->sh_flags & SHF_GROUP)
    flag += "G";
  if (shdr->sh_flags & SHF_TLS)
    flag += "T";

  return flag;
}

std::string ElfInfo::getSectionHeaderFormat(void)
{
  std::stringstream output;
  auto* ehdr = _elf64.ehdr;
  auto* shdr = _elf64.shdr;
  unsigned char* string_table;
  string_table = &_mem[shdr[ehdr->e_shstrndx].sh_offset];

  output << "Section Header : " << std::endl;
  output << "[Nr] Name               Type               Address            Offset" << std::endl;
  output << "     Size               EntSize            Flags  Link  Info  Align" << std::endl;

  for (int i = 0; i < ehdr->e_shnum; i++) {
    boost::io::ios_all_saver guard(output);
    output << std::right;
    output << "[" << std::setw(2) << std::to_string(i) << std::right << "] ";
    output << std::setw(17) << std::setfill(' ') << std::left;
    output << std::left;
    output << std::setw(19) << &string_table[shdr[i].sh_name];
    output << std::setw(18) << getShTypeString(&shdr[i]);
    output << std::setw(0) <<" 0x" << std::setw(16) << std::setfill('0') << std::right;
    output << std::hex << shdr[i].sh_addr;
    output << std::setw(0) <<" 0x" << std::setw(8) << std::setfill('0') << std::right;
    output << std::hex << shdr[i].sh_offset;
    output << std::endl;
    output << "  ";
    output << std::setw(2) << std::setfill(' ') <<" " << std::right;
    output << std::setw(0) <<" 0x" << std::setw(16) << std::setfill('0') << std::right;
    output << std::hex <<shdr[i].sh_size;
    output << std::setw(0) <<" 0x" << std::setw(16) << std::setfill('0') << std::right;
    output << std::hex << shdr[i].sh_entsize;
    output << std::setw(4) << std::setfill(' ') << std::right;
    output << getSectionFlags(&shdr[i]);
    output << std::setw(7) << shdr[i].sh_link;
    output << std::setw(6) << shdr[i].sh_info;
    output << std::setw(6) << shdr[i].sh_addralign;
    output << std::endl;
  }

  return output.str();
}
