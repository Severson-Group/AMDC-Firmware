#ifndef VECTOR_TYPES_H
#define VECTOR_TYPES_H

typedef union vec_dq {
    struct {
        double d;
        double q;
    };
    double elems[2];
} vec_dq_t;

typedef union vec_dqz {
    struct {
        double d;
        double q;
        double z;
    };
    double elems[3];
} vec_dqz_t;

typedef union vec_abc {
    struct {
    	double a;
    	double b;
    	double c;
    };
    double elems[3];
} vec_abc_t;

#endif // VECTOR_TYPES_H
