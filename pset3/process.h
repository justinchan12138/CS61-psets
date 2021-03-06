// vim: set tabstop=8: -*- tab-width: 8 -*-
#ifndef WEENSYOS_PROCESS_H
#define WEENSYOS_PROCESS_H
#include "lib.h"
#include "x86.h"

// process.h
//
//    Support code for WeensyOS processes.


// SYSTEM CALLS

static inline uint32_t syscall_0(int) __attribute__((always_inline));
static inline uint32_t syscall_1(int, uint32_t) __attribute__((always_inline));
static inline uint32_t syscall_2(int, uint32_t, uint32_t)
    __attribute__((always_inline));
static inline uint32_t syscall_3(int, uint32_t, uint32_t, uint32_t)
    __attribute__((always_inline));

// sys_getpid
//    Return current process ID.
static inline pid_t sys_getpid(void) {
    return (pid_t) syscall_1(INT_SYS_GETPID, 0);
}

// sys_yield
//    Yield control of the CPU to the kernel, which will pick another
//    process to run. (It might run this process again, depending on the
//    scheduling policy.)
static inline void sys_yield(void) {
    (void) syscall_0(INT_SYS_YIELD);
}

// sys_page_alloc(addr)
//    Allocate a page of memory at address `addr`. `Addr` must be page-aligned
//    (i.e., a multiple of PAGESIZE == 4096). Returns 0 on success and -1
//    on failure.
static inline int sys_page_alloc(void *addr) {
    return (int) syscall_1(INT_SYS_PAGE_ALLOC, (uintptr_t) addr);
}

// sys_fork()
//    Fork the current process. On success, return the child's process ID to
//    the parent, and return 0 to the child. On failure, return -1.
static inline pid_t sys_fork(void) {
    return (pid_t) syscall_0(INT_SYS_FORK);
}

// sys_exit()
//    Exit this process. Does not return.
static inline void sys_exit(void) __attribute__((noreturn));
static inline void sys_exit(void) {
    syscall_0(INT_SYS_EXIT);
 loop: goto loop;		// will never reach here
}


// OTHER HELPER FUNCTIONS

// app_printf(format, ...)
//    Calls console_printf() (see lib.h). The cursor position is read from
//    `cursorpos`, a shared variable defined by the kernel, and written back
//    into that variable. The initial color is based on the current process ID.
void app_printf(int colorid, const char *format, ...);


// Actual syscall_0, syscall_1, syscall_2, and syscall_3 functions.
//    We use the following calling convention for system calls.
//    (1) The interrupt number is the system call number.
//    (2) Up to 3 parameters may be passed in registers %eax, %ecx, and %edx.
//    (3) The return value is passed in %eax.

static inline uint32_t syscall_0(int syscall_number) {
    uintptr_t result;
    asm volatile("int %1\n"
		 : "=a" (result)	// return value in %eax
		 : "i" (syscall_number)	// pass system call number to `int`
		 : "cc", "memory");
    return result;
}

static inline uint32_t syscall_1(int syscall_number, uint32_t arg0) {
    asm volatile("int %1\n"
		 : "+a" (arg0)		// return value + 1st arg in %eax
		 : "i" (syscall_number)
		 : "cc", "memory");
    return arg0;
}

static inline uint32_t syscall_2(int syscall_number, uint32_t arg0,
				 uint32_t arg1) {
    asm volatile("int %1\n"
		 : "+a" (arg0)		// return value + 1st arg in %eax
		 : "i" (syscall_number),
		   "c" (arg1)		// 2nd arg in %ecx
		 : "cc", "memory");
    return arg0;
}

static inline uint32_t syscall_3(int syscall_number, uint32_t arg0,
				 uint32_t arg1, uint32_t arg2) {
    asm volatile("int %1\n"
		 : "+a" (arg0)		// return value + 1st arg in %eax
		 : "i" (syscall_number),
		   "c" (arg1),		// 2nd arg in %ecx
		   "d" (arg2)		// 3rd arg in %edx
		 : "cc", "memory");
    return arg0;
}

#endif
