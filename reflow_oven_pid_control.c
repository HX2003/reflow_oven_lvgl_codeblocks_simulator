/**
 * @file reflow_oven_pid_control.c
 *
 */
/*********************
 *      INCLUDES
 *********************/
#include "reflow_oven_pid_control.h"
#include "reflow_oven_ui.h"
#include "reflow_oven_settings.h"
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

 /**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void pid_control_setup(){
	//This should be done only when reflow is started

}
void temp_log_setup(uint16_t max_points_param){
	max_points = max_points_param;
	temp_data = malloc(max_points*sizeof(lv_coord_t)); //Remember to free it later
}
lv_coord_t * temp_log_get(){
	return temp_data;
}

void temp_log_free(){
	free(temp_data);
}
