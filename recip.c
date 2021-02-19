#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <fenv.h>
#include <float.h>

#if !defined(__STDC_IEC_559__) && !defined(__APPLE__)
# error Need IEEE 754 FP
#endif

typedef union {
  float f;
  uint32_t i;
} f32_union;

#define P 7                // precision of approximation
#define S 23               // significand bits in binary32
#define E 8                // exponent bits in binary32
#define B ((1UL<<(E-1))-1) // binary32 exponent bias
#define N (1UL<<P)         // number of LUT entries

uint32_t rsqrt_lut[N];
uint32_t recip_lut[N];

#define rsqrt_lut_idx(sig, exp) (((sig) >> (S-P+1)) | ((exp & 1UL) << (P-1)))
#define recip_lut_idx(sig) ((sig) >> (S-P))

uint32_t estimate_rsqrt_sig(uint32_t idx)
{
  const int ip = P, op = P;

  // P-bit index corresponds to {exp[0], sig[S-1:S-(P-1)]}
  uint32_t exp = (idx >> (ip-1)) ? B-2 : B-1; // 1 bit from exp -> [0.25, 1.0)
  uint32_t sig = (idx & ((1UL<<(ip-1))-1)) << (S-(ip-1)); // P-1 bits from sig

  // sqrt(leftmost point on interval)
  // (If P is increased substantially, need to increase precision beyond double.)
  f32_union in = {.i = (exp << S) | sig};
  double left = sqrt(in.f);

  // sqrt(rightmost point on interval)
  f32_union in1 = {.i = in.i + (1UL<<(S-ip+1))};
  double right = sqrt(nextafter((double)(in1.f), 0.0));

  // Naively search the space of 2^P output values for the one that minimizes
  // the maximum error on the interval.  Since the function is monotonic,
  // evaluating the error on the extremes of the interval suffices.
  // (This could obviously be done more efficiently, but 2^P is small.)
  double best_error = INFINITY;
  f32_union best = {.f = 0.0f};
  f32_union base = {.i = B << S}; // [1.0, 2.0)
  for (f32_union cand = base; cand.i < base.i + (1UL<<S); cand.i += 1UL<<(S-op)) {
    double error = fmax(fabs(1.0 - ((double)(cand.f)) * left),
                        fabs(1.0 - ((double)(cand.f)) * right));
    if (error < best_error) {
      best_error = error;
      best = cand;
    }
  }

  // Return P MSBs of mantissa
  return (best.i >> (S-op)) & ((1UL<<op)-1);
}

uint32_t estimate_recip_sig(uint32_t idx)
{
  const int ip = P, op = P;

  // P-bit index corresponds to sig[S-1:S-P]
  uint32_t sig = idx << (S-ip);
  uint32_t exp = B-1; // [0.5, 1.0)

  // Leftmost point on interval
  f32_union in = {.i = (exp << S) | sig};
  double left = in.f;

  // Rightmost point on interval
  f32_union in1 = {.i = in.i + (1UL<<(S-ip))};
  double right = nextafter((double)(in1.f), 0.0);

  // Naively search the space of 2^P output values for the one that minimizes
  // the maximum error on the interval.  Since the function is monotonic,
  // evaluating the error on the extremes of the interval suffices.
  // (This could obviously be done more efficiently, but 2^P is small.)
  double best_error = INFINITY;
  f32_union best = {.f = 0.0f};
  f32_union base = {.i = B << S}; // [1.0, 2.0)
  for (f32_union cand = base; cand.i < base.i + (1UL<<S); cand.i += 1UL<<(S-op)) {
    double error = fmax(fabs(1.0 - (double)(cand.f) * left),
                        fabs(1.0 - (double)(cand.f) * right));
    if (error < best_error) {
      best_error = error;
      best = cand;
    }
  }

  // Return P MSBs of mantissa
  return (best.i >> (S-op)) & ((1UL<<op)-1);
}

float rsqrt(float a)
{
  f32_union in = {.f = a};

  bool sign = in.i >> (S+E);
  uint32_t exp = (in.i >> S) & ((1UL<<E)-1);
  uint32_t sig = in.i & ((1UL<<S)-1);

  if (exp == 0 && sig == 0) {
    // zero => inf of same sign; raise divide-by-zero
    feraiseexcept(FE_DIVBYZERO);
    return copysignf(INFINITY, a);
  } else if (exp == ((1UL<<E)-1) && sig != 0) {
    // NaN => canonical NaN
    if (!(sig >> (S-1))) // raise invalid on sNaN
      feraiseexcept(FE_INVALID);
    return NAN;
  } else if (sign) {
    // nonzero negative => NaN; raise invalid
    feraiseexcept(FE_INVALID);
    return NAN;
  } else if (exp == ((1UL<<E)-1)) {
    // +inf => +zero
    return copysignf(0, a);
  } else if (exp == 0) {
    // normalize the subnormal
    while ((sig & (1UL<<(S-1))) == 0)
      exp--, sig <<= 1;
    sig = (sig << 1) & ((1UL<<S)-1);
  }

  uint32_t out_sig = rsqrt_lut[rsqrt_lut_idx(sig, exp)] << (S-P);
  uint32_t out_exp = (3 * B + ~exp) / 2;
  f32_union res = {.i = (out_exp << S) | out_sig};
  return res.f;
}

float recip(float a)
{
  f32_union in = {.f = a};

  bool sign = in.i >> (S+E);
  uint32_t exp = (in.i >> S) & ((1UL<<E)-1);
  uint32_t sig = in.i & ((1UL<<S)-1);

  if (exp == ((1UL<<E)-1) && sig == 0) {
    // inf => zero of same sign
    return copysignf(0, a);
  } else if (exp == ((1UL<<E)-1)) {
    // NaN => canonical NaN
    if (!(sig >> (S-1))) // raise invalid on sNaN
      feraiseexcept(FE_INVALID);
    return NAN;
  } else if (exp == 0 && sig == 0) {
    // zero => inf of same sign; raise divide-by-zero
    feraiseexcept(FE_DIVBYZERO);
    return copysignf(INFINITY, a);
  } else if (exp == 0) {
    // normalize the subnormal
    while ((sig & (1UL<<(S-1))) == 0)
      exp--, sig <<= 1;
    sig = (sig << 1) & ((1UL<<S)-1);

    if (exp != 0 && exp != (uint32_t)-1) {
      // overflow to inf or max value of same sign, depending on sign and
      // rounding mode
      feraiseexcept(FE_INEXACT | FE_OVERFLOW);

      if (fegetround() == FE_TOWARDZERO ||
          (fegetround() == FE_DOWNWARD && !sign) ||
          (fegetround() == FE_UPWARD && sign))
        return copysignf(FLT_MAX, a);
      else
        return copysignf(INFINITY, a);
    }
  }

  uint32_t out_exp = 2 * B + ~exp;
  uint32_t out_sig = recip_lut[recip_lut_idx(sig)] << (S-P);

  if (out_exp == 0 || out_exp == (uint32_t)-1) {
    // the result is subnormal, but don't raise the underflow exception,
    // because there's no additional loss of precision.
    out_sig = (out_sig >> 1) | (1UL << (S-1));
    if (out_exp == (uint32_t)-1) {
      out_sig >>= 1;
      out_exp = 0;
    }
  }

  f32_union res = {.i = ((uint32_t)sign << (E+S)) | (out_exp << S) | out_sig};
  return res.f;
}

void populate_luts()
{
  for (size_t i = 0; i < N; i++) {
    rsqrt_lut[i] = estimate_rsqrt_sig(i);
    recip_lut[i] = estimate_recip_sig(i);
  }
}

void verilog()
{
  printf("module RSqrt%dLUT (input [%d:0] in, output reg [%d:0] out);\n", P, P-1, P-1);
  printf("  // in[%d] corresponds to exp[0]\n", P-1);
  printf("  // in[%d:0] corresponds to sig[S-1:S-%d]\n", P-2, P-2);
  printf("  // out[%d:0] corresponds to sig[S-1:S-%d]\n", P-1, P-1);
  printf("  always @(*)\n");
  printf("    case (in)\n");
  for (size_t i = 0; i < N; i++)
    printf("      %zu: out = %" PRIu32 ";\n", i, rsqrt_lut[i]);
  printf("    endcase\n");
  printf("endmodule\n");

  printf("module Recip%dLUT (input [%d:0] in, output reg [%d:0] out);\n", P, P-1, P-1);
  printf("  // in[%d:0] and out[%d:0] correspond to sig[S-1:S-%d]\n", P-1, P-1, P-1);
  printf("  always @(*)\n");
  printf("    case (in)\n");
  for (size_t i = 0; i < N; i++)
    printf("      %zu: out = %" PRIu32 ";\n", i, recip_lut[i]);
  printf("    endcase\n");
  printf("endmodule\n");
}

void test()
{
  double max_error = 0;

  for (uint32_t i = 0x3F000000; i <= 0x3F800000; i++) {
    f32_union r = {.i = i};
    double error = 1.0 - recip(r.f) * r.f;
    max_error = fmax(fabs(error), max_error);
  }
  printf("max recip error on [0.5, 1]: 2^%g\n", log2(max_error));

  max_error = 0;
  for (uint32_t i = 0x3E800000; i <= 0x3F800000; i++) {
    f32_union r = {.i = i};
    double error = 1.0 - rsqrt(r.f) * sqrt(r.f);
    max_error = fmax(fabs(error), max_error);
  }
  printf("max rsqrt error on [0.25, 1]: 2^%g\n", log2(max_error));
}

void test_slow()
{
  double max_error = 0;

  for (uint32_t i = 0x0; i <= 0x7f7fffff; i++) {
    f32_union r = {.i = i};
    float rcp = recip(r.f);
    double error = 1.0 - rcp * r.f;

    if (!isfinite(rcp)) {
      assert(!isfinite(1.0f / r.f));
    } else {
      max_error = fmax(fabs(error), max_error);
    }
  }
  printf("max recip error: 2^%g\n", log2(max_error));

  max_error = 0;
  for (uint32_t i = 0; i <= 0x7f7fffff; i++) {
    f32_union r = {.i = i};
    double error = 1.0 - rsqrt(r.f) * sqrt(r.f);
    max_error = fmax(fabs(error), max_error);
  }
  printf("max rsqrt error: 2^%g\n", log2(max_error));
}

int main(int argc, char** argv)
{
  populate_luts();

  if (argc == 2 && strcmp(argv[1], "--verilog") == 0) {
    verilog();
    return 0;
  }

  if (argc == 2 && strcmp(argv[1], "--test") == 0) {
    test();
    return 0;
  }

  if (argc == 2 && strcmp(argv[1], "--test-long") == 0) {
    test_slow();
    return 0;
  }

  printf("Invoke me with --verilog, --test, or --test-long\n");
  return 1;
}
