
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <inttypes.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <sys/time.h>

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
#define LOOP_LV3(expr) { EXPAND_S6(LOOP_LV2(expr)); }   // 1 << 18

#define _REAL_TYPE float

#define MOV_XMM1_TO_VA(va) { \
    asm volatile ("vmovss %0, %%xmm1;" : "+m" (va) : "m" (va) ); \
}
#define MOV_XMM2_TO_VA(va) { \
    asm volatile ("vmovss %0, %%xmm2;" : "+m" (va) : "m" (va)); \
}
#define MOV_XMM3_TO_VA(va) { \
    asm volatile ("vmovss %0, %%xmm3;" : "+m" (va) : "m" (va)); \
}
// need constant_x/y/t
#define PREPROCESS(v1, v2, v3) { \
    asm volatile ("vmovss %%xmm1, %0;" :  : "m" (v1) : "%xmm1"); \
    asm volatile ("vmovss %%xmm2, %0;" :  : "m" (v2) : "%xmm2"); \
    asm volatile ("vmovss %%xmm3, %0;" :  : "m" (v3) : "%xmm3"); \
}
// vfmadd132ss 1 2 3 -> 1 = round(1 * 3 + 2)
// vfmadd231ss 1 2 3 -> 1 = round(2 * 3 + 1)
#define ONE_OP asm("vfmadd231ss %%xmm1, %%xmm2, %%xmm3;" : : )

#define RES_CNT_MAX (1ULL << 19)
#define CNT_PER_LOOP_SHIFT 2
#define CNT_PER_LOOP (1 << (CNT_PER_LOOP_SHIFT * 6))
#define LOOPN(n, expr) LOOP_LV##n(expr)
#define LOOP_N_VA(n, expr) LOOPN(n, expr)
#define NS_PER_SECOND (1000000000ULL)

void run_test_float_scalar() {
    printf("total instruction execute cnt:2^%llu\n", __builtin_ctzll(RES_CNT_MAX * CNT_PER_LOOP));
    register unsigned long long result_cnt = 0;

    float constant_t = 0.0f;
    float constant_x = 1.0f;
    float constant_y = 2.0f;
    PREPROCESS(constant_t, constant_x, constant_y);

    struct timespec t_1, t_2;
    (void)clock_gettime(CLOCK_MONOTONIC, &t_1);

    while (result_cnt < RES_CNT_MAX) {
        LOOP_N_VA(CNT_PER_LOOP_SHIFT, ONE_OP);
        result_cnt += 1;
    }

    (void)clock_gettime(CLOCK_MONOTONIC, &t_2);
    MOV_XMM1_TO_VA(constant_t);
    // constant_t = 0.0f;
    // constant_x = 3.0f;
    // constant_y = 4.0f;
    // MOV_XMM2_TO_VA(constant_x);
    // MOV_XMM3_TO_VA(constant_y);
    // printf("%f %f %f\n", constant_t, constant_x, constant_y);

    unsigned long long ns_cost = (t_2.tv_sec * NS_PER_SECOND + t_2.tv_nsec) - (t_1.tv_sec * NS_PER_SECOND + t_1.tv_nsec);
    result_cnt = result_cnt * CNT_PER_LOOP;
    printf("cnt: %llu ns_cost: %lld constant_t: %f expect: %f\n",
        result_cnt, ns_cost, constant_t,
        1.0f * (CNT_PER_LOOP * RES_CNT_MAX));
}