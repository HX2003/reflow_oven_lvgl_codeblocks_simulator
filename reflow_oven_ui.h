/**
 * @file reflow_oven_ui.h
 *
 */

#ifndef REFLOW_OVEN_UI_H
#define REFLOW_OVEN_UI_H

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
typedef enum {
    REFLOW_OVEN_HEATER_STATUS_ACTIVE,
    REFLOW_OVEN_HEATER_STATUS_INACTIVE,
    REFLOW_OVEN_HEATER_STATUS_DISABLED
}reflow_oven_heater_status_type_t;

typedef enum {
    REFLOW_OVEN_DOOR_STATUS_UNLOCKED,
    REFLOW_OVEN_DOOR_STATUS_LOCKED
}reflow_oven_door_status_type_t;
/**********************
 * GLOBAL PROTOTYPES
 **********************/
void reflow_oven_ui(void);
void reflow_oven_error_popup(char* msg, uint8_t error_code_val);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*REFLOW_OVEN_UI_H*/
