#pragma once

#include <stdint.h>

typedef uint8_t elf_byte_t;

typedef uint32_t elf32_addr_t;
typedef uint32_t elf32_off_t;
typedef int32_t elf32_sword_t;
typedef uint32_t elf32_word_t;
typedef uint16_t elf32_half_t;
typedef uint64_t elf32_lword_t;

typedef uint64_t elf64_addr_t;
typedef uint64_t elf64_off_t;
typedef int32_t elf64_shalf_t;

typedef int32_t elf64_sword_t;
typedef uint32_t elf64_word_t;

typedef int64_t elf64_sxword_t;
typedef uint64_t elf64_xword_t;
typedef uint64_t elf64_lword_t;

typedef uint32_t elf64_half_t;
typedef uint16_t elf64_quarter_t;

// e_ident[] identification indices
#define EI_MAG0 0       // magic (1/4)
#define EI_MAG1 1       // magic (2/4)
#define EI_MAG2 2       // magic (3/4)
#define EI_MAG3 3       // magic (4/4)
#define EI_CLASS 4      // file class
#define EI_DATA 5       // data encoding
#define EI_VERSION 6    // header version
#define EI_OSABI 7      // OS/ABI
#define EI_ABIVERSION 8 // ABI version
#define EI_PAD 9        // start of pad bytes
#define EI_NIDENT 16    // size of e_ident[]

// e_ident[] magic numbers
#define ELFMAG0 0x7f     // e_ident[EI_MAG0]
#define ELFMAG1 'E'      // e_ident[EI_MAG1]
#define ELFMAG2 'L'      // e_ident[EI_MAG2]
#define ELFMAG3 'F'      // e_ident[EI_MAG3]
#define ELFMAG "\177ELF" // magic
#define ELFMAGSZ 4       // size of magic

// e_ident[] file class
#define ELFCLASSNONE 0 // invalid
#define ELFCLASS32 1   // 32-bit object
#define ELFCLASS64 2   // 64-bit object

// e_ident[] data encoding
#define ELFDATANONE 0 // invalid
#define ELFDATA2LSB 1 // little-endian
#define ELFDATA2MSB 2 // big-endian

// e_ident[] OS/ABI
#define ELFOSABI_SYSV 0         // UNIX System V ABI
#define ELFOSABI_HPUX 1         // HP-UX operating system
#define ELFOSABI_NETBSD 2       // NetBSD
#define ELFOSABI_LINUX 3        // GNU/Linux
#define ELFOSABI_HURD 4         // GNU/Hurd
#define ELFOSABI_86OPEN 5       // 86Open common IA32 ABI
#define ELFOSABI_SOLARIS 6      // Solaris
#define ELFOSABI_MONTEREY 7     // Monterey
#define ELFOSABI_IRIX 8         // IRIX
#define ELFOSABI_FREEBSD 9      // FreeBSD
#define ELFOSABI_TRU64 10       // TRU64 UNIX
#define ELFOSABI_MODESTO 11     // Novell Modesto
#define ELFOSABI_OPENBSD 12     // OpenBSD
#define ELFOSABI_ARM 97         // ARM
#define ELFOSABI_STANDALONE 255 // standalone application

typedef struct {
  elf_byte_t e_ident[EI_NIDENT]; // identification bytes
  elf32_half_t e_type;           // file type
  elf32_half_t e_machine;        // machine type
  elf32_word_t e_version;        // version number
  elf32_addr_t e_entry;          // entry point
  elf32_off_t e_phoff;           // program header offset
  elf32_off_t e_shoff;           // section header offset
  elf32_word_t e_flags;          // processor flags
  elf32_half_t e_ehsize;         // sizeof ehdr
  elf32_half_t e_phentsize;      // program header entry size
  elf32_half_t e_phnum;          // number of program headers
  elf32_half_t e_shentsize;      // section header entry size
  elf32_half_t e_shnum;          // number of section headers
  elf32_half_t e_shstrndx;       // string table index
} elf32_header_t;

typedef struct {
  elf_byte_t e_ident[EI_NIDENT]; // identification bytes
  elf64_quarter_t e_type;        // file type
  elf64_quarter_t e_machine;     // machine type
  elf64_half_t e_version;        // version number
  elf64_addr_t e_entry;          // entry point
  elf64_off_t e_phoff;           // program header offset
  elf64_off_t e_shoff;           // section header offset
  elf64_half_t e_flags;          // processor flags
  elf64_quarter_t e_ehsize;      // sizeof ehdr
  elf64_quarter_t e_phentsize;   // program header entry size
  elf64_quarter_t e_phnum;       // number of program headers
  elf64_quarter_t e_shentsize;   // section header entry size
  elf64_quarter_t e_shnum;       // number of section headers
  elf64_quarter_t e_shstrndx;    // string table index
} elf64_header_t;

// e_type
#define ET_NONE 0        // no file type
#define ET_REL 1         // relocatable file
#define ET_EXEC 2        // executable file
#define ET_DYN 3         // shared object file
#define ET_CORE 4        // core file
#define ET_LOPROC 0xff00 // reserved range for processor
#define ET_HIPROC 0xffff // specific e_type

// e_machine
#define EM_NONE 0         // no machine
#define EM_M32 1          // AT&T WE 32100
#define EM_SPARC 2        // SPARC
#define EM_386 3          // Intel 80386
#define EM_68K 4          // Motorola 68000
#define EM_88K 5          // Motorola 88000
#define EM_860 7          // Intel 80860
#define EM_MIPS 8         // MIPS R3000 big-endian only
#define EM_MIPS_RS4_BE 10 // MIPS R4000 big-endian
#define EM_SPARC64 11     // SPARC v9 64-bit unofficial
#define EM_PARISC 15      // HPPA
#define EM_SPARC32PLUS 18 // Enhanced instruction set SPARC
#define EM_PPC 20         // PowerPC
#define EM_PPC64 21       // PowerPC 64
#define EM_ARM 40         // Advanced RISC machines ARM
#define EM_ALPHA 41       // DEC ALPHA
#define EM_SH 42          // Hitachi/Renesas Super-H
#define EM_SPARCV9 43     // SPARC version 9
#define EM_IA_64 50       // Intel IA-64 processor
#define EM_AMD64 62       // AMD64 architecture
#define EM_VAX 75         // DEC VAX
#define EM_AARCH64 183    // ARM 64-bit architecture (aarch64)

// e_version
#define EV_NONE 0    // invalid
#define EV_CURRENT 1 // current

typedef struct {
  elf32_word_t sh_name;      // section name
  elf32_word_t sh_type;      // section type
  elf32_word_t sh_flags;     // section flags
  elf32_addr_t sh_addr;      // virtual address
  elf32_off_t sh_offset;     // file offset
  elf32_word_t sh_size;      // section size
  elf32_word_t sh_link;      // link to another
  elf32_word_t sh_info;      // misc info
  elf32_word_t sh_addralign; // memory alignment
  elf32_word_t sh_entsize;   // table entry size
} elf32_section_header_t;

typedef struct {
  elf64_half_t sh_name;       // section name
  elf64_half_t sh_type;       // section type
  elf64_xword_t sh_flags;     // section flags
  elf64_addr_t sh_addr;       // virtual address
  elf64_off_t sh_offset;      // file offset
  elf64_xword_t sh_size;      // section size
  elf64_half_t sh_link;       // link to another
  elf64_half_t sh_info;       // misc info
  elf64_xword_t sh_addralign; // memory alignment
  elf64_xword_t sh_entsize;   // table entry size
} elf64_section_header_t;

// special section indices
#define SHN_UNDEF 0          // undefined
#define SHN_LORESERVE 0xff00 // lower bounds of reserved indices
#define SHN_LOPROC 0xff00    // reserved range for processor
#define SHN_HIPROC 0xff1f    // specific section indices
#define SHN_ABS 0xfff1       // absolute value
#define SHN_COMMON 0xfff2    // common symbol
#define SHN_XINDEX 0xffff    // escape, index stored elsewhere
#define SHN_HIRESERVE 0xffff // upper bounds of reserved indices

// sh_type
#define SHT_NULL 0                  // inactive
#define SHT_PROGBITS 1              // program defined information
#define SHT_SYMTAB 2                // symbol table section
#define SHT_STRTAB 3                // string table section
#define SHT_RELA 4                  // relocation section with addend
#define SHT_HASH 5                  // symbol hash table section
#define SHT_DYNAMIC 6               // dynamic section
#define SHT_NOTE 7                  // note section
#define SHT_NOBITS 8                // no space section
#define SHT_REL 9                   // relation section without addends
#define SHT_SHLIB 10                // reserved - purpose unknown
#define SHT_DYNSYM 11               // dynamic symbol table section
#define SHT_INIT_ARRAY 14           // pointers to init functions
#define SHT_FINI_ARRAY 15           // pointers to termination functions
#define SHT_PREINIT_ARRAY 16        // ptrs to funcs called before init
#define SHT_GROUP 17                // defines a section group
#define SHT_SYMTAB_SHNDX 18         // Section indices (see SHN_XINDEX).
#define SHT_RELR 19                 // relative-only relocation section
#define SHT_LOOS 0x60000000         // reserved range for OS specific
#define SHT_SUNW_dof 0x6ffffff4     // used by dtrace
#define SHT_GNU_LIBLIST 0x6ffffff7  // libraries to be prelinked
#define SHT_SUNW_move 0x6ffffffa    // inf for partially init'ed symbols
#define SHT_SUNW_syminfo 0x6ffffffc // ad symbol information
#define SHT_SUNW_verdef 0x6ffffffd  // symbol versioning inf
#define SHT_SUNW_verneed 0x6ffffffe // symbol versioning req
#define SHT_SUNW_versym 0x6fffffff  // symbol versioning table
#define SHT_HIOS 0x6fffffff         // section header types
#define SHT_LOPROC 0x70000000       // reserved range for processor
#define SHT_HIPROC 0x7fffffff       // specific section header types
#define SHT_LOUSER 0x80000000       // reserved range for application
#define SHT_HIUSER 0xffffffff       // specific indices

// section names
#define ELF_BSS ".bss"               // uninitialized data
#define ELF_DATA ".data"             // initialized data
#define ELF_CTF ".SUNW_ctf"          // CTF data
#define ELF_DEBUG ".debug"           // debug
#define ELF_DYNAMIC ".dynamic"       // dynamic linking information
#define ELF_DYNSTR ".dynstr"         // dynamic string table
#define ELF_DYNSYM ".dynsym"         // dynamic symbol table
#define ELF_FINI ".fini"             // termination code
#define ELF_GOT ".got"               // global offset table
#define ELF_HASH ".hash"             // symbol hash table
#define ELF_INIT ".init"             // initialization code
#define ELF_REL_DATA ".rel.data"     // relocation data
#define ELF_REL_FINI ".rel.fini"     // relocation termination code
#define ELF_REL_INIT ".rel.init"     // relocation initialization code
#define ELF_REL_DYN ".rel.dyn"       // relocation dynamic link info
#define ELF_REL_RODATA ".rel.rodata" // relocation read-only data
#define ELF_REL_TEXT ".rel.text"     // relocation code
#define ELF_RODATA ".rodata"         // read-only data
#define ELF_SHSTRTAB ".shstrtab"     // section header string table
#define ELF_STRTAB ".strtab"         // string table
#define ELF_SYMTAB ".symtab"         // symbol table
#define ELF_TEXT ".text"             // code

// sh_flags
#define SHF_WRITE 0x1              // writable
#define SHF_ALLOC 0x2              // occupies memory
#define SHF_EXECINSTR 0x4          // executable
#define SHF_MERGE 0x10             // may be merged
#define SHF_STRINGS 0x20           // contains strings
#define SHF_INFO_LINK 0x40         // sh_info holds section index
#define SHF_LINK_ORDER 0x80        // ordering requirements
#define SHF_OS_NONCONFORMING 0x100 // OS-specific processing required
#define SHF_GROUP 0x200            // member of section group
#define SHF_TLS 0x400              // thread local storage
#define SHF_COMPRESSED 0x800       // contains compressed data
#define SHF_MASKOS 0x0ff00000      // OS-specific semantics
#define SHF_MASKPROC 0xf0000000    // reserved bits for processor

typedef struct {
  elf32_word_t st_name;  // symbol name index in string table
  elf32_addr_t st_value; // type/binding attributes
  elf32_word_t st_size;  // unused
  elf_byte_t st_info;    // section index of symbol
  elf_byte_t st_other;   // value of symbol
  elf32_half_t st_shndx; // size of symbol
} elf32_symbol_t;

typedef struct {
  elf64_half_t st_name;     // symbol name index in string table
  elf_byte_t st_info;       // type/binding attributes
  elf_byte_t st_other;      // unused
  elf64_quarter_t st_shndx; // section index of symbol
  elf64_xword_t st_value;   // value of symbol
  elf64_xword_t st_size;    // size of symbol
} elf64_symbol_t;

#define ELF32_ST_BIND(x) ((x) >> 4)
#define ELF32_ST_TYPE(x) (((unsigned int) x) & 0xf)
#define ELF32_ST_INFO(b, t) (((b) << 4) + ((t) &0xf))

#define ELF64_ST_BIND(x) ((x) >> 4)
#define ELF64_ST_TYPE(x) (((unsigned int) x) & 0xf)
#define ELF64_ST_INFO(b, t) (((b) << 4) + ((t) &0xf))

#define ELF_ST_VISIBILITY(v) ((v) &0x3)

#define ELF32_ST_VISIBILITY ELF_ST_VISIBILITY
#define ELF64_ST_VISIBILITY ELF_ST_VISIBILITY

// symbol binding - ELF32_ST_BIND - st_info
#define STB_LOCAL 0   // local symbol
#define STB_GLOBAL 1  // global symbol
#define STB_WEAK 2    // like global, byt lower precedence
#define STB_LOPROC 13 // reserved range for processor
#define STB_HIPROC 15 // specific symbol bindings

// symbol type - ELF32_ST_TYPE - st_info
#define STT_NOTYPE 0  // not specified
#define STT_OBJECT 1  // data object
#define STT_FUNC 2    // function
#define STT_SECTION 3 // section
#define STT_FILE 4    // file
#define STT_TLS 6     // thread local storage
#define STT_LOPROC 13 // reserved range for processor
#define STT_HIPROC 15 // specific symbol types

// symbol visibility - st_other
#define STV_DEFAULT 0   // visibility set by binding type
#define STV_INTERNAL 1  // OS specific version of STV_HIDDEN
#define STV_HIDDEN 2    // can only be seen inside own .so
#define STV_PROTECTED 3 // hidden inside, default outside

typedef struct {
  elf32_addr_t r_offset; // offset of relocation
  elf32_word_t r_info;   // symbol table index and type
} elf32_relocation_t;

typedef struct {
  elf32_addr_t r_offset;  // offset of relocation
  elf32_word_t r_info;    // symbol table index and type
  elf32_sword_t r_addend; // adjustment value
} elf32_relocation_add_t;

typedef struct {
  elf64_xword_t r_offset; // offset of relocation
  elf64_xword_t r_info;   // symbol table index and type
} elf64_relocation_t;

typedef struct {
  elf64_xword_t r_offset;  // offset of relocation
  elf64_xword_t r_info;    // symbol table index and type
  elf64_sxword_t r_addend; // adjustment value
} elf64_relocation_add_t;

// relocation types
#define R_386_NONE 0
#define R_386_32 1         // symbol + addend
#define R_386_PC32 2       // symbol + addend - section offset
#define R_386_GOT32 3      // used by build-time linker to create GOT entry
#define R_386_PLT32 4      // used by build-time linker to create PLT entry
#define R_386_COPY 5       // https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter4-10454.html#chapter4-84604
#define R_386_GLOB_DAT 6   // relation between GOT entry and symbol
#define R_386_JMP_SLOT 7   // fixed up by dynamic loader
#define R_386_RELATIVE 8   // base address + addend
#define R_386_TLS_TPOFF 14 // negative offset into the static TLS storage
#define R_386_TLS_TPOFF32 37

#define R_X86_64_NONE 0
#define R_X86_64_64 1
#define R_X86_64_GLOB_DAT 6
#define R_X86_64_JUMP_SLOT 7
#define R_X86_64_RELATIVE 8
#define R_X86_64_TPOFF64 18

#define ELF32_R_SYM(i) ((i) >> 8)
#define ELF32_R_TYPE(i) ((unsigned char) (i))
#define ELF32_R_INFO(s, t) (((s) << 8) + (unsigned char) (t))

#define ELF64_R_SYM(info) ((info) >> 32)
#define ELF64_R_TYPE(info) ((info) &0xFFFFFFFF)
#define ELF64_R_INFO(s, t) (((s) << 32) + (uint32_t) (t))

typedef struct {
  elf32_word_t p_type;   // segment type
  elf32_off_t p_offset;  // segment offset
  elf32_addr_t p_vaddr;  // virtual address of segment
  elf32_addr_t p_paddr;  // physical address, ignored?
  elf32_word_t p_filesz; // number of bytes in file for segment
  elf32_word_t p_memsz;  // number of bytes in memory for segment
  elf32_word_t p_flags;  // flags
  elf32_word_t p_align;  // memory alignment
} elf32_program_header_t;

typedef struct {
  elf64_half_t p_type;    // segment type
  elf64_half_t p_flags;   // segment offset
  elf64_off_t p_offset;   // virtual address of segment
  elf64_addr_t p_vaddr;   // physical address, ignored?
  elf64_addr_t p_paddr;   // number of bytes in file for segment
  elf64_xword_t p_filesz; // number of bytes in memory for segment
  elf64_xword_t p_memsz;  // flags
  elf64_xword_t p_align;  // memory alignment
} elf64_program_header_t;

// segment types - p_type
#define PT_NULL 0            // unused
#define PT_LOAD 1            // loadable segment
#define PT_DYNAMIC 2         // dynamic linking section
#define PT_INTERP 3          // the RTLD
#define PT_NOTE 4            // auxiliary information
#define PT_SHLIB 5           // reserved - purpose undefined
#define PT_PHDR 6            // program header
#define PT_TLS 7             // thread local storage
#define PT_LOOS 0x60000000   // reserved range for OS
#define PT_HIOS 0x6fffffff   // specific segment types
#define PT_LOPROC 0x70000000 // reserved range for processor
#define PT_HIPROC 0x7fffffff // specific segment types

// segment flags - p_flags
#define PF_X 0x1               // executable
#define PF_W 0x2               // writable
#define PF_R 0x4               // readable
#define PF_MASKPROC 0xf0000000 // reserved bits for processor

typedef struct {
  elf32_sword_t d_tag; // controls meaning of d_val

  union {
    elf32_word_t d_val; // multiple meanings, see d_tag
    elf32_addr_t d_ptr; // program virtual address
  } d_un;
} elf32_dynamic_t;

typedef struct {
  elf64_xword_t d_tag; // controls meaning of d_val

  union {
    elf64_addr_t d_ptr;  // multiple meanings, see d_tag
    elf64_xword_t d_val; // program virtual address
  } d_un;
} elf64_dynamic_t;

// dynamic array tags - d_tag
#define DT_NULL 0
#define DT_NEEDED 1
#define DT_PLTRELSZ 2
#define DT_PLTGOT 3
#define DT_HASH 4
#define DT_STRTAB 5
#define DT_SYMTAB 6
#define DT_RELA 7
#define DT_RELASZ 8
#define DT_RELAENT 9
#define DT_STRSZ 10
#define DT_SYMENT 11
#define DT_INIT 12
#define DT_FINI 13
#define DT_SONAME 14
#define DT_RPATH 15
#define DT_SYMBOLIC 16
#define DT_REL 17
#define DT_RELSZ 18
#define DT_RELENT 19
#define DT_PLTREL 20
#define DT_DEBUG 21
#define DT_TEXTREL 22
#define DT_JMPREL 23
#define DT_BIND_NOW 24
#define DT_INIT_ARRAY 25
#define DT_FINI_ARRAY 26
#define DT_INIT_ARRAYSZ 27
#define DT_FINI_ARRAYSZ 28
#define DT_RUNPATH 29
#define DT_FLAGS 30
#define DT_ENCODING 31
#define DT_PREINIT_ARRAY 32
#define DT_PREINIT_ARRAYSZ 33
#define DT_RELRSZ 35
#define DT_RELR 36
#define DT_RELRENT 37
#define DT_LOOS 0x6000000d
#define DT_HIOS 0x6ffff000
#define DT_LOPROC 0x70000000
#define DT_HIPROC 0x7fffffff

// dynamic flags - DT_FLAGS .dynamic entry
#define DF_ORIGIN 0x00000001
#define DF_SYMBOLIC 0x00000002
#define DF_TEXTREL 0x00000004
#define DF_BIND_NOW 0x00000008
#define DF_STATIC_TLS 0x00000010

// d ynamic flags - DT_FLAGS_1 .dynamic entry
#define DF_1_NOW 0x00000001
#define DF_1_GLOBAL 0x00000002
#define DF_1_GROUP 0x00000004
#define DF_1_NODELETE 0x00000008
#define DF_1_LOADFLTR 0x00000010
#define DF_1_INITFIRST 0x00000020
#define DF_1_NOOPEN 0x00000040
#define DF_1_ORIGIN 0x00000080
#define DF_1_DIRECT 0x00000100
#define DF_1_TRANS 0x00000200
#define DF_1_INTERPOSE 0x00000400
#define DF_1_NODEFLIB 0x00000800
#define DF_1_NODUMP 0x00001000
#define DF_1_CONLFAT 0x00002000

typedef struct {
  elf32_word_t n_namesz;
  elf32_word_t n_descsz;
  elf32_word_t n_type;
} elf32_note_header_t;

typedef struct {
  elf64_half_t n_namesz;
  elf64_half_t n_descsz;
  elf64_half_t n_type;
} elf64_note_header_t;

typedef struct {
  elf32_word_t namesz;
  elf32_word_t descsz;
  elf32_word_t type;
} elf32_note_t;

typedef struct {
  elf64_half_t namesz;
  elf64_half_t descsz;
  elf64_half_t type;
} elf64_note_t;

// values for n_type
#define NT_PRSTATUS 1 // process status
#define NT_FPREGSET 2 // floating point registers
#define NT_PRPSINFO 3 // process state info

typedef struct {
  elf32_sword_t au_id; // 32-bit id
  elf32_word_t au_v;   // 32-bit value
} elf32_aux_info_t;

typedef struct {
  elf64_shalf_t au_id; // 64-bit id
  elf64_xword_t au_v;  // 64-bit value
} elf64_aux_info_t;

// AUX ID
#define AUX_NULL 0
#define AUX_IGNORE 1
#define AUX_EXECFD 2
#define AUX_PHDR 3
#define AUX_PHENT 4
#define AUX_PHNUM 5
#define AUX_PAGESZ 6
#define AUX_BASE 7
#define AUX_FLAGS 8
#define AUX_ENTRY 9
