    .text
    .balign 4
    .global strlen
# size_t strlen(const char *str)
# a0 holds *str

strlen:
    mv a3, a0             # Save start
    li t0, -1             # Infinite AVL
loop:
    vsetvli a1, t0, e8  # Vector of bytes of maximum length
    vlbff.v v1, (a3)      # Load bytes
    csrr a1, vl           # Get bytes read
    vmseq.vi v0, v1, 0    # Set v0[i] where v1[i] = 0
    vfirst.m a2, v0       # Find first set bit
    add a3, a3, a1        # Bump pointer
    bltz a2, loop         # Not found?

    add a0, a0, a1        # Sum start + bump
    add a3, a3, a2        # Add index
    sub a0, a3, a0        # Subtract start address+bump

    ret
