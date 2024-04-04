#include <immintrin.h>
#include <inttypes.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <wchar.h>

/*
    _m128 t = (1, 2, 3, 4)
    _m128 x = (1, 1, 1, 1)
    _m128 y = (2, 2, 2, 2)
    mov x, y, t into xmm1 xmm2 xmm3

    loop
        vfmadd231 xmm1, xmm2, xmm3

*/

#define EXPAND_S6(expr) { \
        expr; expr; expr; expr; expr; expr; expr; expr; \
        expr; expr; expr; expr; expr; expr; expr; expr; \
        expr; expr; expr; expr; expr; expr; expr; expr; \
        expr; expr; expr; expr; expr; expr; expr; expr; \
        expr; expr; expr; expr; expr; expr; expr; expr; \
        expr; expr; expr; expr; expr; expr; expr; expr; \
        expr; expr; expr; expr; expr; expr; expr; expr; \
        expr; expr; expr; expr; expr; expr; expr; expr; \
}
#define LOOP_LV1(expr) { EXPAND_S6(expr); }             // 1 << 6
#define LOOP_LV2(expr) { EXPAND_S6(LOOP_LV1(expr)); }   // 1 << 12

#define CNT_PER_LOOP (1)
#define NS_PER_SECOND (1000000000ULL)
#define RES_CNT_MAX (1ULL << 33)

void run_test_float_packed() {
    printf("total instruction execute cnt: %llu\n", RES_CNT_MAX);
    __m128 t = _mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f); // xmm1
    __m128 x = _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f); // xmm2
    __m128 y = _mm_set_ps(2.0f, 2.0f, 2.0f, 2.0f); // xmm3
    asm("vmovaps %%xmm1, %0;" : : "m"(t));
    asm("vmovaps %%xmm2, %0;" : : "m"(x));
    asm("vmovaps %%xmm3, %0;" : : "m"(y));

    register unsigned long long result_cnt = 0;
    struct timespec t_1, t_2;

    (void)clock_gettime(CLOCK_MONOTONIC, &t_1);

    while (result_cnt < RES_CNT_MAX) {
        LOOP_LV2(asm("vfmadd231ps %%xmm1, %%xmm2, %%xmm3" : :));
        result_cnt += (1ULL << (2 * 6));
    }

    (void)clock_gettime(CLOCK_MONOTONIC, &t_2);
    // move xmm1 to output
    asm("vmovaps %0, %%xmm1" : "+m" (t) : "m" (t));
    float output[4];
    _mm_storeu_ps(output, t);
    unsigned long long ns_cost = (t_2.tv_sec * NS_PER_SECOND + t_2.tv_nsec) -
        (t_1.tv_sec * NS_PER_SECOND + t_1.tv_nsec);

    result_cnt *= 4;
    printf("cnt: %llu ns_cost: %lld expect: %f\n",
        result_cnt, ns_cost, 1.0f * (CNT_PER_LOOP * RES_CNT_MAX));
    for (int i = 0; i < 4; ++i) {
        printf("%f ", output[i]);
    }
    printf("\n");

}