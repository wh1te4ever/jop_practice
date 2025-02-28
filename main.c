#include <stdio.h>
#include <stdint.h>

uint64_t my_call8(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7, uint64_t a8) {

    printf("a1 = 0x%llx, a2 = 0x%llx, a3 = 0x%llx, a4 = 0x%llx, a5 = 0x%llx, a6 = 0x%llx, a7 = 0x%llx, a8 = 0x%llx\n", \
        a1, a2, a3, a4, a5, a6, a7, a8);

    return 0xCAFEBABE41424344;
}

__attribute__((naked)) uint64_t gadget_populate(uint64_t *x0) {
    __asm__ volatile (
        "ldp x5, x8, [x0, #0x10] \n"
        "ldp x1, x2, [x0, #0x20] \n" 
        "ldp x3, x4, [x0, #0x30] \n" 
        "mov x0, x8 \n"
        "br x5 \n"
    );
}

__attribute__((naked)) uint64_t gadget_prologue(uint64_t *x0) {
    __asm__ volatile (
        "stp x20, x19, [sp, #-0x20]!\n" 
        "stp x29, x30, [sp, #0x10]\n" 
        "add x29, sp, #0x10\n"        
        "mov x19, x1\n"                
        "ldr x0, [x0, #0x98]\n"          
        "ldr x8, [x0]\n"               
        "ldr x8, [x8, #0x7C0]\n"      
        "blr x8\n"                    
        "str x0, [x19]\n"             
        "mov w0, #0\n"               
        "ldp x29, x30, [sp, #0x10]\n"   
        "ldp x20, x19, [sp], #0x20\n"   
        "ret\n"
    );
}

__attribute__((naked)) void mov_x16_x1__br_x2(void) {
    __asm__ volatile (
        "mov x16, x1\n"
        "br x2\n"
    );
}

__attribute__((naked)) void mov_x15_x3__br_x4(void) {
    __asm__ volatile (
        "mov x15, x3\n"
        "br x4\n"
    );
}

__attribute__((naked)) void mov_x7_x16__br_x2(void) {
    __asm__ volatile (
        "mov x7, x16\n"
        "br x2\n"
    );
}

__attribute__((naked)) void mov_x16_x3__br_x4(void) {
    __asm__ volatile (
        "mov x16, x3\n"
        "br x4\n"
    );
}

__attribute__((naked)) void mov_x5_x16__br_x15(void) {
    __asm__ volatile (
        "mov x5, x16\n"
        "br x15\n"
    );
}

__attribute__((naked)) void mov_x012345__br_x6(void) {
    __asm__ volatile (
        "mov x0, x1\n"
        "mov x1, x2\n"
        "mov x2, x3\n"
        "mov x3, x4\n"
        "mov x4, x5\n"
        "br x6\n"
    );
}

__attribute__((naked)) void mov_x0_x3__br_x4(void) {
    __asm__ volatile (
        "mov x0, x3\n"
        "br x4\n"
    );
}

__attribute__((naked)) void mov_x0123__br_x4(void) {
    __asm__ volatile (
        "mov x0, x1\n"
        "mov x1, x2\n"
        "mov x2, x3\n"
        "br x4\n"
    );
}

__attribute__((naked)) void br_x6(void) {
    __asm__ volatile (
        "br x6\n"
    );
}

uint32_t my_call6(uint64_t addr, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7) {
    uint32_t ret = 0;

    //TODO: call my_call8 using addr
    uint64_t (*func)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = 
        (uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)) addr;

    ret = (uint32_t)func(a1, a2, a3, a4, a5, a6, a7, 0);

    return ret;
}


int main(int argc, char *argv[], char *envp[]) {
    printf("jop_practice\n");

    uint64_t ret = my_call8(0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48);
    printf("my_call8 ret = 0x%llx\n", ret);

    my_call6(my_call8, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47);

    return 0;
}