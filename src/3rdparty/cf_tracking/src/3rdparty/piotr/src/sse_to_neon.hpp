#ifndef SSE2NEON_H
#define SSE2NEON_H

// This header file provides a simple API translation layer
// between SSE intrinsics to their corresponding ARM NEON versions
//
// This header file does not (yet) translate *all* of the SSE intrinsics.
// Since this is in support of a specific porting effort, I have only
// included the intrinsics I needed to get my port to work.
//
// Questions/Comments/Feedback send to: jratcliffscarab@gmail.com
//
// If you want to improve or add to this project, send me an
// email and I will probably approve your access to the depot.
//
// Project is located here:
//
//  https://github.com/jratcliff63367/sse2neon
//
// TipJar: 1PzgWDSyq4pmdAXRH8SPUtta4SWGrt4B1p : https://blockchain.info/address/1PzgWDSyq4pmdAXRH8SPUtta4SWGrt4B1p
//
//
// Contributors to this project are:
//
// John W. Ratcliff : jratcliffscarab@gmail.com
// Brandon Rowlett : browlett@nvidia.com
// Ken Fast : kfast@gdeb.com

#define FORCE_INLINE                    inline __attribute__((always_inline))

#include "arm_neon.h"

typedef float32x4_t __m128;
typedef int32x4_t __m128i;

// Sets the four single-precision, floating-point values to w. https://msdn.microsoft.com/en-us/library/vstudio/2x1se8ha(v=vs.100).aspx
FORCE_INLINE __m128 _mm_set1_ps(float _w) {
    return vdupq_n_f32(_w);
}

// Sets the four single-precision, floating-point values to the four inputs. https://msdn.microsoft.com/en-us/library/vstudio/afh0zf75(v=vs.100).aspx
FORCE_INLINE __m128 _mm_set_ps(float w, float z, float y, float x) {
    float __attribute__((aligned(16))) data[4] = { x, y, z, w };
    return vld1q_f32(data);
}

// Sets the 4 signed 32-bit integer values to i. https://msdn.microsoft.com/en-us/library/vstudio/h4xscxat(v=vs.100).aspx
FORCE_INLINE __m128i _mm_set1_epi32(int _i) {
    return vdupq_n_s32(_i);
}

// Loads four single-precision, floating-point values. https://msdn.microsoft.com/en-us/library/vstudio/zzd50xxt(v=vs.100).aspx
FORCE_INLINE __m128 _mm_load_ps(const float* p) {
    return vld1q_f32(p);
}

// Loads four single-precision, floating-point values.  https://msdn.microsoft.com/en-us/library/x1b16s7z%28v=vs.90%29.aspx
FORCE_INLINE __m128 _mm_loadu_ps(const float* p) {
    return _mm_load_ps(p);
}

// Stores four single-precision, floating-point values. https://msdn.microsoft.com/en-us/library/vstudio/s3h4ay6y(v=vs.100).aspx
FORCE_INLINE void _mm_store_ps(float* p, __m128 a) {
    vst1q_f32(p, a);
}

// Stores the lower single - precision, floating - point value. https://msdn.microsoft.com/en-us/library/tzz10fbx(v=vs.100).aspx
FORCE_INLINE void _mm_store_ss(float* p, __m128 a) {
    vst1q_lane_f32(p, a, 0);
}

// Stores four single-precision, floating-point values. https://msdn.microsoft.com/en-us/library/44e30x22(v=vs.100).aspx
FORCE_INLINE void _mm_storeu_ps(float* p, __m128 a) {
    vst1q_f32(p, a);
}

// Adds the 4 signed or unsigned 32-bit integers in a to the 4 signed or unsigned 32-bit integers in b. https://msdn.microsoft.com/en-us/library/vstudio/09xs4fkk(v=vs.100).aspx
FORCE_INLINE __m128i _mm_add_epi32(__m128i a, __m128i b) {
    return vaddq_s32(a, b);
}

// Adds the four single-precision, floating-point values of a and b. https://msdn.microsoft.com/en-us/library/vstudio/c9848chc(v=vs.100).aspx
FORCE_INLINE __m128 _mm_add_ps(__m128 a, __m128 b) {
    return vaddq_f32(a, b);
}

// Subtracts the four single-precision, floating-point values of a and b. https://msdn.microsoft.com/en-us/library/vstudio/1zad2k61(v=vs.100).aspx
FORCE_INLINE __m128 _mm_sub_ps(__m128 a, __m128 b) {
    return vsubq_f32(a, b);
}

// Multiplies the four single-precision, floating-point values of a and b. https://msdn.microsoft.com/en-us/library/vstudio/22kbk6t9(v=vs.100).aspx
FORCE_INLINE __m128 _mm_mul_ps(__m128 a, __m128 b) {
    return vmulq_f32(a, b);
}

// Computes the minima of the four single-precision, floating-point values of a and b. https://msdn.microsoft.com/en-us/library/vstudio/wh13kadz(v=vs.100).aspx
FORCE_INLINE __m128 _mm_min_ps(__m128 a, __m128 b) {
    return vminq_f32(a, b);
}

// Computes the approximations of reciprocals of the four single-precision, floating-point values of a. https://msdn.microsoft.com/en-us/library/vstudio/796k1tty(v=vs.100).aspx
FORCE_INLINE __m128 _mm_rcp_ps(__m128 in) {
    __m128 recip = vrecpeq_f32(in);
    recip = vmulq_f32(recip, vrecpsq_f32(recip, in));
    return recip;
}

// Computes the approximations of square roots of the four single-precision, floating-point values of a. First computes reciprocal square roots and then reciprocals of the four values. https://msdn.microsoft.com/en-us/library/vstudio/8z67bwwk(v=vs.100).aspx
FORCE_INLINE __m128 _mm_sqrt_ps(__m128 in) {
    __m128 recipsq = vrsqrteq_f32(in);
    __m128 sq = vrecpeq_f32(recipsq);
    // ??? use step versions of both sqrt and recip for better accuracy?
    return sq;
}

// Computes the maximums of the four single-precision, floating-point values of a and b. https://msdn.microsoft.com/en-us/library/vstudio/ff5d607a(v=vs.100).aspx
FORCE_INLINE __m128 _mm_max_ps(__m128 a, __m128 b) {
    return vmaxq_f32(a, b);
}

// Computes the bitwise AND of the four single-precision, floating-point values of a and b. https://msdn.microsoft.com/en-us/library/vstudio/73ck1xc5(v=vs.100).aspx
FORCE_INLINE __m128 _mm_and_ps(__m128 a, __m128 b) {
    return (__m128)vandq_s32((__m128i)a, (__m128i)b);
}

// Computes the bitwise AND of the 128-bit value in a and the 128-bit value in b. https://msdn.microsoft.com/en-us/library/vstudio/6d1txsa8(v=vs.100).aspx
FORCE_INLINE __m128i _mm_and_si128(__m128i a, __m128i b) {
    return (__m128i)vandq_s32(a, b);
}

// Computes the bitwise AND-NOT of the four single-precision, floating-point values of a and b. https://msdn.microsoft.com/en-us/library/vstudio/68h7wd02(v=vs.100).aspx
FORCE_INLINE __m128 _mm_andnot_ps(__m128 a, __m128 b) {
    return (__m128)vbicq_s32((__m128i)b, (__m128i)a); // *NOTE* argument swap
}

// Computes the bitwise OR of the four single-precision, floating-point values of a and b. https://msdn.microsoft.com/en-us/library/vstudio/7ctdsyy0(v=vs.100).aspx
FORCE_INLINE __m128 _mm_or_ps(__m128 a, __m128 b) {
    return (__m128)vorrq_s32((__m128i)a, (__m128i)b);
}

// Computes bitwise EXOR (exclusive-or) of the four single-precision, floating-point values of a and b. https://msdn.microsoft.com/en-us/library/ss6k3wk8(v=vs.100).aspx
FORCE_INLINE __m128 _mm_xor_ps(__m128 a, __m128 b) {
    return (__m128)veorq_s32((__m128i)a, (__m128i)b);
}

// Compares for greater than. https://msdn.microsoft.com/en-us/library/vstudio/11dy102s(v=vs.100).aspx
FORCE_INLINE __m128 _mm_cmpgt_ps(__m128 a, __m128 b) {
    return (__m128)vcgtq_f32(a, b);
}

// Compares for less than https://msdn.microsoft.com/en-us/library/vstudio/f330yhc8(v=vs.100).aspx
FORCE_INLINE __m128 _mm_cmplt_ps(__m128 a, __m128 b) {
    return (__m128)vcltq_f32(a, b);
}

// Compares the 4 signed 32-bit integers in a and the 4 signed 32-bit integers in b for greater than. https://msdn.microsoft.com/en-us/library/vstudio/1s9f2z0y(v=vs.100).aspx
FORCE_INLINE __m128i _mm_cmpgt_epi32(__m128i a, __m128i b) {
    return (__m128i)vcgtq_s32(a, b);
}

// Compares the 4 signed 32-bit integers in a and the 4 signed 32-bit integers in b for less than. https://msdn.microsoft.com/en-us/library/vstudio/4ak0bf5d(v=vs.100).aspx
FORCE_INLINE __m128i _mm_cmplt_epi32(__m128i a, __m128i b) {
    return (__m128i)vcltq_s32(a, b);
}

// Converts the four signed 32-bit integer values of a to single-precision, floating-point values https://msdn.microsoft.com/en-us/library/vstudio/36bwxcx5(v=vs.100).aspx
FORCE_INLINE __m128 _mm_cvtepi32_ps(__m128i a) {
    return vcvtq_f32_s32(a);
}

// Converts the four single-precision, floating-point values of a to signed 32-bit integer values using truncate. https://msdn.microsoft.com/en-us/library/vstudio/1h005y6x(v=vs.100).aspx
FORCE_INLINE __m128i _mm_cvttps_epi32(__m128 a) {
    return vcvtq_s32_f32(a);
}

#endif
