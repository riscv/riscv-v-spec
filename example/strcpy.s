    .text
    .balign 4
    .global strcpy
  # char* strcpy(char *dst, const char* src)
strcpy:
      mv a2, a0             # Copy dst
      li t0, -1             # Infinite AVL
loop:
    vsetvli x0, t0, e8,m8   # Max length vectors of bytes
    vleff.v v1, (a1)        # Get src bytes
      csrr t1, vl           # Get number of bytes fetched
    vmseq.vi v0, v1, 0      # Flag zero bytes
    vfirst.m a3, v0         # Zero found?
      add a1, a1, t1        # Bump pointer
    vmsif.m v0, v0          # Set mask up to and including zero byte.
    vse.v v1, (a2), v0.t    # Write out bytes
      add a2, a2, t1        # Bump pointer
      bltz a3, loop         # Zero byte not found, so loop

      ret
