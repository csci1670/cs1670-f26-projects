ENTRY(_start)

/*
We need to set text and rodata as read-executable, while data and  bss can
be read-writable.

The PT_DYNAMIC header is how the kernel finds the program's relocations (see
apply_relocations in kernel/elf.c).
*/
PHDRS
{
  text PT_LOAD FLAGS(5);  /* R + X */
  data PT_LOAD FLAGS(6);  /* R + W */
  dynamic PT_DYNAMIC;
}

SECTIONS
{
  /* Programs start at 64 KiB, not at 0x0, so that nothing is ever mapped in
     the lowest 64 KiB of a process's address space. This ensures that
     dereferencing a NULL pointer, or a small offset from one, results in a
     page fault. If you change this, change DIAB_IMAGE_BASE in
     user/diabolical/diab_vm_common.h, too. */
  . = 0x10000;

  .text : {
    __image_start = .;  /* where the program starts */
    *(.text)
  } :text

  . = ALIGN(0x1000);

  .rodata : {
    *(.rodata*)
  } :text

  /* Some linkers (e.g., Ubuntu's) add a build ID note to every program.
     Left unplaced, it lands at address 0 in the data segment, which then
     starts at 0 and overlaps the text segment: loading it overwrites the
     code and the relocations with zeroes. */
  .note.gnu.build-id : {
    *(.note.gnu.build-id)
  } :text

  . = ALIGN(0x1000);

  .data : {
    *(.data)
  } :data

  .dynamic : {
    *(.dynamic)
  } :data :dynamic

  .got : {
    *(.got)
    *(.got.plt)
  } :data

  .bss : {
    *(.bss)
    *(COMMON)
  } :data
}
