/**
 * @file reflow_oven_pid_control.h
 *
 */

#ifndef REFLOW_OVEN_PID_CONTROL_H
#define REFLOW_OVEN_PID_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_coord_t *temp_data;
uint16_t max_points;

void pid_control_setup();
void temp_log_setup(uint16_t max_points_param);
lv_coord_t * temp_log_get();
void temp_log_free();

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*REFLOW_OVEN_PID_CONTROL_H*/
