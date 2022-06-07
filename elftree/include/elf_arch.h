#ifndef __ELF_ARCH_H__
#define __ELF_ARCH_H__

#if defined(ARCH_AARCH64) || defined(ARCH_X86_64)

#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Shdr Elf64_Shdr
#define Elf_Dyn  Elf64_Dyn

#else

#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Shdr Elf32_Shdr
#define Elf_Dyn  Elf32_Dyn

#endif /* ARCH_AARCH64  */

typedef struct {
  Elf_Ehdr* ehdr;
  Elf_Phdr* phdr;
  Elf_Shdr* shdr;
} ElfLowInfo;

#endif /* __ELF_ARCH_H__ */
