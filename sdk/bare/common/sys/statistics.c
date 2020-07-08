/**
 * This code uses the standard deviation and rolling statistics code
 * described at https://www.johndcook.com/blog/standard_deviation/
 * and in Knuth TAOCP vol 2, 3rd edition, page 232
 */
#include "sys/statistics.h"
#include <math.h>

void statistics_init(statistics_t *stats)
{
    statistics_clear(stats);
}

void statistics_clear(statistics_t *stats)
{
    stats->min = 0;
    stats->max = 0;
    stats->mean = 0;
    stats->__old_mean = 0;
    stats->__s = 0;
    stats->__old_s = 0;

    stats->num_samples = 0;
    stats->value = 0;
}

void statistics_push(statistics_t *stats, double val)
{
    // TODO: should return an error
    if (isnan(val) || isinf(val)) {
        return;
    }

    // See Knuth TAOCP vol 2, 3rd edition, page 232
    stats->num_samples++;
    stats->value = val;

    if (stats->num_samples == 1) {
        stats->min = val;
        stats->max = val;
        stats->__s = 0;
        stats->__old_s = 0;
        stats->mean = val;
        stats->__old_mean = val;
    } else {
        stats->mean = stats->__old_mean + (val - stats->__old_mean) / stats->num_samples;
        stats->__s = stats->__old_s + (val - stats->__old_mean) * (val - stats->mean);

        stats->__old_mean = stats->mean;
        stats->__old_s = stats->__s;

        if (val > stats->max) {
            stats->max = val;
        }

        if (val < stats->min) {
            stats->min = val;
        }
    }
}

double statistics_variance(statistics_t *stats)
{
    if (stats->num_samples > 1) {
        return stats->__s / (stats->num_samples);
    } else {
        return 0;
    }
}
