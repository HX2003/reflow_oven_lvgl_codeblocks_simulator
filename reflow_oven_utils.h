/**
 * @file reflow_oven_utils.h
 *
 */

#ifndef REFLOW_OVEN_UTILS_H
#define REFLOW_OVEN_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

int32_t linear_interpolate(int32_t x, int32_t x2, int32_t y1, int32_t y2);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*REFLOW_OVEN_UTILS_H*/
