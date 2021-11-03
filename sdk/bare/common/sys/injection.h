#ifndef INJECTION_H
#define INJECTION_H

#include <stdint.h>

typedef enum inj_func_e {
    CONST = 1,
    NOISE,
    CHIRP,
    TRIANGLE,
    SQUARE,
    NONE,
} inj_func_e;

typedef enum inj_op_e {
    ADD = 1,
    SUB,
    SET,
} inj_op_e;

typedef struct inj_func_constant_t {
    double value;
} inj_func_constant_t;

typedef struct inj_func_noise_t {
    double gain;
    double offset;
} inj_func_noise_t;

typedef struct inj_func_chirp_t {
    double gain;
    double freqMin;
    double freqMax;
    double period;
} inj_func_chirp_t;

typedef struct inj_func_triangle_t {
    double valueMin;
    double valueMax;
    double period;
} inj_func_triangle_t;

typedef struct inj_func_square_t {
    double valueMin;
    double valueMax;
    double period;
} inj_func_square_t;

#define INJ_MAX_NAME_LENGTH (24)

typedef struct inj_ctx_t {
    int id;
    uint8_t registered;
    char name[INJ_MAX_NAME_LENGTH];
    struct inj_ctx_t *next;

    inj_func_e inj_func;
    inj_op_e operation;

    inj_func_constant_t constant;
    inj_func_noise_t noise;
    inj_func_chirp_t chirp;
    inj_func_triangle_t triangle;
    inj_func_square_t square;

    double curr_time;
} inj_ctx_t;

void injection_init(void);

void injection_ctx_init(inj_ctx_t *inj_ctx, char *name);
int injection_ctx_register(inj_ctx_t *inj_ctx);
int injection_ctx_unregister(inj_ctx_t *inj_ctx);
void injection_ctx_clear(inj_ctx_t *inj_ctx);

void injection_inj(double *output, inj_ctx_t *ctx, double Ts);

void injection_clear(void);
void injection_list(void);
void injection_const(inj_ctx_t *ctx, inj_op_e op, double value);
void injection_noise(inj_ctx_t *ctx, inj_op_e op, double gain, double offset);
void injection_chirp(inj_ctx_t *ctx, inj_op_e op, double gain, double freqMin, double freqMax, double period);
void injection_triangle(inj_ctx_t *ctx, inj_op_e op, double valueMin, double valueMax, double period);
void injection_square(inj_ctx_t *ctx, inj_op_e op, double valueMin, double valueMax, double period);

inj_ctx_t *injection_find_ctx_by_name(char *name);

#endif // INJECTION_H
