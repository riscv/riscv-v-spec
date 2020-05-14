    .text
    .balign 4
    .global strncpy
  # char* strncpy(char *dst, const char* src, size_t n)
strncpy:
      mv a3, a0             # Copy dst
loop:
    vsetvli x0, a2, e8,m8   # Vectors of bytes.
    vle8ff.v v8, (a1)        # Get src bytes
    vmseq.vi v1, v8, 0      # Flag zero bytes
      csrr t1, vl           # Get number of bytes fetched
    vfirst.m a4, v1         # Zero found?
    vmsif.m v0, v1          # Set mask up to and including zero byte.
    vse8.v v8, (a3), v0.t    # Write out bytes
      sub a2, a2, t1        # Decrement count.
      bgez a4, zero_tail    # Zero remaining bytes.
      add a1, a1, t1        # Bump pointer
      add a3, a3, t1        # Bump pointer
      bnez a2, loop         # Anymore?

      ret

zero_tail:
    vsetvli x0, a2, e8,m8   # Vectors of bytes.
    vmv.v.i v0, 0           # Splat zero.

zero_loop:
    vsetvli t1, a2, e8,m8   # Vectors of bytes.
    vse8.v v0, (a3)          # Store zero.
      sub a2, a2, t1        # Decrement count.
      add a3, a3, t1        # Bump pointer
      bnez a2, zero_loop    # Anymore?

      ret
