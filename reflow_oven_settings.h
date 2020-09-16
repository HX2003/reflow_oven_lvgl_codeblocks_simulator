/**
 * @file reflow_oven_settings.h
 *
 */

#ifndef REFLOW_OVEN_SETTINGS_H
#define REFLOW_OVEN_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

typedef struct{
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t patch_version;
    uint8_t display_brightness;
    uint8_t speaker_volume;
    float pid_p_presoak;
    float pid_i_presoak;
    float pid_d_presoak;
    float pid_p_soak;
    float pid_i_soak;
    float pid_d_soak;
    float pid_p_reflow;
    float pid_i_reflow;
    float pid_d_reflow;
    uint8_t current_reflow_profile;
}reflow_oven_settings_t;

#define REFLOW_OVEN_FIRMWARE_MAJOR_VERSION 1
#define REFLOW_OVEN_FIRMWARE_MINOR_VERSION 1
#define REFLOW_OVEN_FIRMWARE_PATCH_VERSION 0

extern reflow_oven_settings_t reflow_oven_setting;

void settings_setup();
void settings_load();
void settings_save();
void settings_reset();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*REFLOW_OVEN_SETTINGS_H*/
