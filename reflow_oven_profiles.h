/**
 * @file reflow_oven_profiles.h
 *
 */

#ifndef REFLOW_OVEN_PROFILES_H
#define REFLOW_OVEN_PROFILES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    const char *title;
    const uint8_t presoak_start_temp;
    const uint8_t presoak_end_temp;
    const uint8_t presoak_dur;
    const uint8_t soak_end_temp;
    const uint8_t soak_dur;
    const uint16_t reflow_begin_end_temp;
    const uint8_t reflow_begin_dur;
    const uint8_t reflow_hold_dur;
    const uint8_t cooldown_end_temp;
    const uint8_t cooldown_dur;
}reflow_oven_profile_t;


const reflow_oven_profile_t reflow_profile_1 = {
    "Lead-free paste 138~C [ChipQuick SMDLTLFP]",
    75,
    150,
    90,
    175,
    100,
    245,
    60,
    25,
    100,
    60
};
const reflow_oven_profile_t reflow_profile_2 = {
    "Leaded paste 138~C [ChipQuick SMDLTLFP]",
    75,
    150,
    90,
    175,
    100,
    245,
    60,
    25,
    100,
    60
};
const reflow_oven_profile_t reflow_profile_3 = {
    "Super A Leaded paste 128~C [ChipQuick SMDLTLFP]",
    75,
    150,
    90,
    175,
    100,
    245,
    60,
    25,
    100,
    60
};
const reflow_oven_profile_t reflow_profile_4 = {
    "Super B Leaded paste 128~C [ChipQuick SMDLTLFP]",
    75,
    150,
    90,
    175,
    100,
    245,
    60,
    25,
    100,
    60
};
const reflow_oven_profile_t reflow_profile_5 = {
    "Super C Leaded paste 128~C [ChipQuick SMDLTLFP]",
    75,
    150,
    90,
    175,
    100,
    245,
    60,
    25,
    100,
    60
};
const reflow_oven_profile_t reflow_profile_6 = {
    "Super D Leaded paste 128~C [ChipQuick SMDLTLFP]",
    75,
    150,
    90,
    175,
    100,
    245,
    60,
    25,
    100,
    60
};
const reflow_oven_profile_t *const reflow_profiles[6] = {
    &reflow_profile_1,
    &reflow_profile_2,
    &reflow_profile_3,
    &reflow_profile_4,
    &reflow_profile_5,
    &reflow_profile_6
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*REFLOW_OVEN_PROFILES_H*/
