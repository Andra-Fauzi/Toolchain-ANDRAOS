/* crt0 for AndraOS - x86-64
 *
 * The kernel jumps here via iretq with RSP pointing to a 16-byte aligned
 * address (USER_STACK_TOP - 16).  We must NOT create a stack frame here
 * because that would misalign RSP before we call main().
 *
 * x86-64 SysV ABI requires RSP % 16 == 8 at _the point of the call
 * instruction_ that enters a function, so that inside the function, after
 * the call pushes the 8-byte return address, RSP % 16 == 0.
 *
 * Kernel sets RSP = (TOP - 16) & ~0xF  → RSP % 16 == 0.
 * We call main() → call pushes 8 bytes → RSP % 16 == 8  inside main. ✓
 * But we also push 0 below RSP first (fake return addr sentinel) to keep
 * the stack looking like a normal C call frame: RSP % 16 == 8 at call site.
 */

#include <stddef.h>
extern int main(int argc, char **argv);
extern void _exit(int);

__attribute__((naked)) void _start(void) {
    __asm__ __volatile__(
        /* Zero the frame pointer – we are the outermost frame */
        "xor  %rbp, %rbp\n\t"
        /* argc = 0, argv = NULL */
        "xor  %edi, %edi\n\t"
        "xor  %esi, %esi\n\t"
        /* We DO NOT push anything before calling main. 
           Kernel iretq set RSP to 16-byte aligned value.
           The 'call' instruction itself pushes RIP (8 bytes),
           so main() entry RSP will be 16n - 8. 
           Inside main preamble, 'push rbp' will make RSP 16n again. Correct!
        */
        "call main\n\t"
        /* Pass return value of main to _exit */
        "mov  %eax, %edi\n\t"
        "call _exit\n\t"
        /* Should never reach here */
        "1: hlt\n\t"
        "jmp 1b\n\t"
    );
}