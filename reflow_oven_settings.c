/**
 * @file reflow_oven_settings.c
 *
 */
/*********************
 *      INCLUDES
 *********************/
#include "reflow_oven_settings.h"

/*********************
 *      DEFINES
 *********************/
reflow_oven_settings_t reflow_oven_setting = {
    .major_version = REFLOW_OVEN_FIRMWARE_MAJOR_VERSION,
    .minor_version = REFLOW_OVEN_FIRMWARE_MINOR_VERSION,
    .patch_version = REFLOW_OVEN_FIRMWARE_PATCH_VERSION,
    .display_brightness = 16,
    .speaker_volume = 16,
    .pid_p_presoak = 100,
    .pid_i_presoak = 0.01,
    .pid_d_presoak = 50,
    .pid_p_soak = 250,
    .pid_i_soak = 0.05,
    .pid_d_soak = 300,
    .pid_p_reflow = 300,
    .pid_i_reflow = 0.05,
    .pid_d_reflow = 350,
    .current_reflow_profile = 1
};
void settings_setup(){

}
void settings_load(){

}
void settings_save(){

}
void settings_reset(){

}
