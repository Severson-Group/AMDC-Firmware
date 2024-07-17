#ifndef STATISTICS_H
#define STATISTICS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    double min;
    double max;
    double mean;
    double value;
    uint32_t num_samples;

    // Private variables
    double __old_mean;
    double __old_s;
    double __s;
} statistics_t;

void statistics_init(statistics_t *stats);
void statistics_clear(statistics_t *stats);
void statistics_push(statistics_t *stats, double val);
double statistics_variance(statistics_t *stats);

#endif // STATISTICS_H
