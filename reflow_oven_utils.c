/**
 * @file reflow_oven_utils.c
 *
 */
/*********************
 *      INCLUDES
 *********************/
#include "reflow_oven_utils.h"

/*********************
 *      DEFINES
 *********************/

 /**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t linear_interpolate(int32_t x, int32_t x2, int32_t y1, int32_t y2){
    int32_t rise = y2 - y1;
    int32_t run = x2;
    int32_t val = (x)*(rise*1.0/run) + y1;
    return val;
};

