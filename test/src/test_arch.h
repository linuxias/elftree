#ifndef __TEST_ARCH_H__
#define __TEST_ARCH_H__

#ifdef ARCH_AARCH64
const std::string filePath = "src/approval_tests_aarch64/elftest";
const ElfArchType archType = ElfArchType::ELF_ARCH_64BIT;
#elif ARCH_ARMV7L
const std::string filePath = "src/approval_tests_armv7l/elftest";
const ElfArchType archType = ElfArchType::ELF_ARCH_32BIT;
#elif ARCH_X86_64
const std::string filePath = "src/approval_tests_x86_64/elftest";
const ElfArchType archType = ElfArchType::ELF_ARCH_64BIT;
#elif ARCH_I586
const std::string filePath = "src/approval_tests_i586/elftest";
const ElfArchType archType = ElfArchType::ELF_ARCH_32BIT;
#endif

#endif /* __TEST_ARCH_H__ */
