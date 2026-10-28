// Program entry points (given by the linker)
extern unsigned char _binary_user_squares_elf_start[];
extern unsigned char _binary_user_pi_elf_start[];

// Added in Project 3. Each of these exists only once you have created the
// corresponding user/<name>.c -- the linker generates one of these symbols
// per user program automatically.
extern unsigned char _binary_user_idle_elf_start[];     // Quest 2
extern unsigned char _binary_user_keytest_elf_start[];  // Quest 2
extern unsigned char _binary_user_basic_elf_start[];    // Quest 2 (provided)
extern unsigned char _binary_user_logo_elf_start[];     // Quest 3
extern unsigned char _binary_user_term_elf_start[];     // Quest 3
extern unsigned char _binary_user_pong_elf_start[];     // Quest 3 (provided)
extern unsigned char _binary_user_clock_elf_start[];    // Quest 4 (provided)

// Added in Project 4. The diab_* programs (in user/diabolical/) misbehave on
// purpose (Quest 1).
extern unsigned char _binary_user_diab_brk_elf_start[];
extern unsigned char _binary_user_diab_irqoff_elf_start[];
extern unsigned char _binary_user_diab_timer_elf_start[];
extern unsigned char _binary_user_diab_vectors_elf_start[];
extern unsigned char _binary_user_diab_crash_elf_start[];
extern unsigned char _binary_user_diab_stack_elf_start[];

extern unsigned char _binary_user_primecheck_elf_start[];
extern unsigned char _binary_user_hello_elf_start[];
extern unsigned char _binary_user_counter_elf_start[];
