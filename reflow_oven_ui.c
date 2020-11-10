/**
 * @file reflow_oven_ui.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "reflow_oven_ui.h"
#include "reflow_oven_theme.h"
#include "reflow_oven_utils.h"
#include "reflow_oven_profiles.h"
#include "reflow_oven_settings.h"
#include "reflow_oven_pid_control.h"
#include "lv_drivers/win_drv.h"
#include "lv_settings.h"
/*********************
 *      DEFINES
 *********************/
/*Bg positions*/
#define REFLOW_OVEN_BG_NONE (-LV_VER_RES)
#define REFLOW_OVEN_BG_FULL 0
#define REFLOW_OVEN_BG_NORMAL (-2 * (LV_VER_RES / 3))
#define REFLOW_OVEN_BG_SMALL (-5 * (LV_VER_RES / 6))

/*Sizes*/
#define REFLOW_OVEN_BTN_H   (240)
#define REFLOW_OVEN_BTN_W   (240)

/*Animations*/
#define REFLOW_OVEN_ANIM_Y (LV_VER_RES / 20)
#define REFLOW_OVEN_ANIM_DELAY (40)
#define REFLOW_OVEN_ANIM_TIME  (150)
#define REFLOW_OVEN_ANIM_TIME_BG  (300)

/*Padding*/
#define REFLOW_OVEN_TITLE_PAD 10

#define DEBUG_UI
/**********************
 *      TYPEDEFS
 **********************/

typedef struct{
     lv_obj_t * cont;
     lv_obj_t * obj;
}icon_t;

typedef struct{
     icon_t * icon1;
     icon_t * icon2;
     icon_t * icon3;
     icon_t * icon4;
}home_icon_t;

home_icon_t home_icon;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void home_open(uint32_t delay);
static void add_home_icon_group();
static void start_reflow();
static void toggle_door();
static void open_settings();
static void open_profiles();

LV_EVENT_CB_DECLARE(back_to_home_event_cb);
LV_EVENT_CB_DECLARE(start_reflow_icon_event_cb);
LV_EVENT_CB_DECLARE(toggle_door_icon_event_cb);
LV_EVENT_CB_DECLARE(open_settings_icon_event_cb);
LV_EVENT_CB_DECLARE(open_profiles_icon_event_cb);
LV_EVENT_CB_DECLARE(abort_reflow_event_cb);
LV_EVENT_CB_DECLARE(no_abort_event_cb);

static icon_t * add_icon(lv_obj_t * parent, const void * src,  const void * label_src, lv_color_t color);
static lv_obj_t * add_title(const char * txt, lv_obj_t * src, reflow_oven_theme_t theme);
static lv_obj_t * add_back(lv_event_cb_t event_cb);
static lv_obj_t * add_loader(void (*end_cb)(lv_anim_t *));
static void loader_anim_cb(void * arc, lv_anim_value_t v);

LV_EVENT_CB_DECLARE(icon_generic_event_cb);
static void reflow_oven_anim_bg(uint32_t delay, lv_color_t color, int32_t y_new);
static void reflow_oven_anim_out(lv_obj_t * obj, uint32_t delay);
static void reflow_oven_anim_out_all(lv_obj_t * obj, uint32_t delay);
static void reflow_oven_anim_in(lv_obj_t * obj, uint32_t delay);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool error_state = false;
static lv_group_t * g;
static lv_obj_t * bg_circle;
static lv_obj_t * bg_left_status_icons;
static lv_obj_t * bg_right_status_icons;
static lv_obj_t * bg_shade;
static lv_obj_t * popup;
static lv_obj_t * abort_title;
static lv_obj_t * abort_btn;
static lv_obj_t * no_btn;
static lv_obj_t * reflow_bottom_btn;
static lv_obj_t * heater_state_icon;
static lv_obj_t * door_state_icon;
static lv_obj_t * temp_state_icon;
static lv_obj_t * temp_state_symbol;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void reflow_oven_ui(void)
{
    lv_theme_t * th = reflow_oven_theme_init(LV_COLOR_BLACK, LV_COLOR_BLACK,
            0, &lv_font_montserrat_10, &lv_font_montserrat_12,
            &lv_font_montserrat_14, &lv_font_montserrat_16);
    lv_theme_set_act(th);

    lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, REFLOW_OVEN_BLUE);
    lv_scr_load(scr);

	#ifdef DEBUG_UI
    bg_circle = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_clean_style_list(bg_circle, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_circle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_circle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2c3e40));
    lv_obj_set_style_local_radius(bg_circle, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_VER_RES/2);
    lv_obj_set_size(bg_circle, LV_HOR_RES, LV_VER_RES);
	#endif

    bg_right_status_icons = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(bg_right_status_icons, 24, 100);
    lv_obj_set_style_local_bg_opa(bg_right_status_icons, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_10);
    lv_obj_align(bg_right_status_icons, NULL, LV_ALIGN_IN_RIGHT_MID, -12, 0);

    bg_left_status_icons = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(bg_left_status_icons, 24, 100);
    lv_obj_set_style_local_bg_opa(bg_left_status_icons, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_10);
    lv_obj_align(bg_left_status_icons, NULL, LV_ALIGN_IN_LEFT_MID, 12, 0);

    heater_state_icon = lv_img_create(bg_right_status_icons, NULL);

    door_state_icon = lv_img_create(bg_right_status_icons, NULL);

    display_heater_status(REFLOW_OVEN_HEATER_STATUS_DISABLED);
    display_door_status(REFLOW_OVEN_DOOR_STATUS_UNLOCKED);

    temp_state_icon = lv_label_create(bg_left_status_icons, NULL);
    temp_state_symbol = lv_label_create(bg_left_status_icons, NULL);
    lv_obj_set_size(temp_state_icon, 18, 18);
    lv_obj_set_size(temp_state_symbol, 18, 18);
    lv_obj_set_style_local_text_color(temp_state_icon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_color(temp_state_symbol, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(temp_state_icon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_normal());
    lv_obj_set_style_local_text_font(temp_state_symbol, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_normal());
    lv_label_set_text(temp_state_icon, "?");
    lv_label_set_text(temp_state_symbol, "C");
    lv_obj_align(temp_state_icon, NULL, LV_ALIGN_CENTER, 0, -6);
    lv_obj_align(temp_state_symbol, NULL, LV_ALIGN_CENTER, 0, 6);

    lv_indev_drv_t enc_drv;
    lv_indev_drv_init(&enc_drv);
    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = win_drv_wheel_read;
    lv_indev_t * enc_indev = lv_indev_drv_register(&enc_drv);
    g = lv_group_create();
    lv_indev_set_group(enc_indev, g);

    home_open(0);
}

void display_heater_status(reflow_oven_heater_status_type_t state){
    if(state == REFLOW_OVEN_HEATER_STATUS_DISABLED){
        LV_IMG_DECLARE(reflow_oven_img_heater_disabled);
        lv_img_set_src(heater_state_icon, &reflow_oven_img_heater_disabled);
        lv_obj_align(heater_state_icon, NULL, LV_ALIGN_CENTER, 0, -16);
    }else if(state == REFLOW_OVEN_HEATER_STATUS_INACTIVE){
        LV_IMG_DECLARE(reflow_oven_img_heater_inactive);
        lv_img_set_src(heater_state_icon, &reflow_oven_img_heater_inactive);
        lv_obj_align(heater_state_icon, NULL, LV_ALIGN_CENTER, 0, -16);
    }else if(state == REFLOW_OVEN_HEATER_STATUS_ACTIVE){
        LV_IMG_DECLARE(reflow_oven_img_heater_active);
        lv_img_set_src(heater_state_icon, &reflow_oven_img_heater_active);
        lv_obj_align(heater_state_icon, NULL, LV_ALIGN_CENTER, 0, -16);
    }
}

void display_door_status(reflow_oven_door_status_type_t state){
    if(state == REFLOW_OVEN_DOOR_STATUS_UNLOCKED){
        LV_IMG_DECLARE(reflow_oven_img_unlocked);
        lv_img_set_src(door_state_icon, &reflow_oven_img_unlocked);
        lv_obj_set_style_local_image_recolor_opa(door_state_icon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
        lv_obj_align(door_state_icon, NULL, LV_ALIGN_CENTER, 0, 16);
    }else if(state == REFLOW_OVEN_DOOR_STATUS_LOCKED){
        LV_IMG_DECLARE(reflow_oven_img_locked);
        lv_img_set_src(door_state_icon, &reflow_oven_img_locked);
        lv_obj_set_style_local_image_recolor_opa(door_state_icon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
        lv_obj_set_style_local_image_recolor(door_state_icon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, REFLOW_OVEN_YELLOW);
        lv_obj_align(door_state_icon, NULL, LV_ALIGN_CENTER, 0, 16);
    }
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

static void home_open(uint32_t delay) {
    lv_obj_t * title = add_title("Reflow Oven", lv_scr_act(), REFLOW_OVEN_THEME_TITLE_WHITE);
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, REFLOW_OVEN_TITLE_PAD);

    delay += REFLOW_OVEN_ANIM_DELAY;
    reflow_oven_anim_in(title, delay);

    /* Initialize each group with appropriate objects */
    lv_coord_t box_w = 180;
    lv_obj_t * box = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_opa(box, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_size(box, box_w, box_w);
    lv_obj_align(box, NULL, LV_ALIGN_CENTER, 0, 0);

    LV_IMG_DECLARE(reflow_oven_img_btn_start);
    LV_IMG_DECLARE(reflow_oven_label_start);
    LV_IMG_DECLARE(reflow_oven_img_btn_door);
    LV_IMG_DECLARE(reflow_oven_label_door);
    LV_IMG_DECLARE(reflow_oven_img_btn_settings);
    LV_IMG_DECLARE(reflow_oven_label_settings);
    LV_IMG_DECLARE(reflow_oven_img_btn_profiles);
    LV_IMG_DECLARE(reflow_oven_label_profiles);

    if(home_icon.icon1!=NULL){
        free(home_icon.icon1);
        free(home_icon.icon2);
        free(home_icon.icon3);
        free(home_icon.icon4);
    }
	home_icon.icon1 = add_icon(box, &reflow_oven_img_btn_start, &reflow_oven_label_start, REFLOW_OVEN_LIGHT_GREEN);
    lv_obj_align_origo(home_icon.icon1->cont, NULL, LV_ALIGN_IN_LEFT_MID, (box_w - 64*2)/3 + 64/2, -45);
    lv_obj_set_event_cb(home_icon.icon1->obj, start_reflow_icon_event_cb);

    home_icon.icon2 = add_icon(box, &reflow_oven_img_btn_door, &reflow_oven_label_door, REFLOW_OVEN_LIGHT_BLUE);
    lv_obj_align_origo(home_icon.icon2->cont, NULL, LV_ALIGN_IN_LEFT_MID, (box_w - 64*2)/3*2 + 64/2 + 64 + 10, -45);
    lv_obj_set_event_cb(home_icon.icon2->obj, toggle_door_icon_event_cb);

    home_icon.icon3 = add_icon(box, &reflow_oven_img_btn_profiles, &reflow_oven_label_profiles, REFLOW_OVEN_LIGHT_RED);
    lv_obj_align_origo(home_icon.icon3->cont, NULL, LV_ALIGN_IN_LEFT_MID, (box_w - 64*2)/3*2 + 64/2 + 64 + 10, 45);
    lv_obj_set_event_cb(home_icon.icon3->obj, open_profiles_icon_event_cb);

    home_icon.icon4 = add_icon(box, &reflow_oven_img_btn_settings, &reflow_oven_label_settings, REFLOW_OVEN_LIGHT_ORANGE);
    lv_obj_align_origo(home_icon.icon4->cont, NULL, LV_ALIGN_IN_LEFT_MID, (box_w - 64*2)/3 + 64/2, 45);
    lv_obj_set_event_cb(home_icon.icon4->obj, open_settings_icon_event_cb);

	add_home_icon_group();
    lv_group_focus_obj(home_icon.icon1->obj);
    lv_group_set_wrap(g, true);

    delay += REFLOW_OVEN_ANIM_DELAY;
    reflow_oven_anim_in(box, delay);
}

static void add_home_icon_group(){
    lv_group_add_obj(g, home_icon.icon1->obj);
    lv_group_add_obj(g, home_icon.icon2->obj);
    lv_group_add_obj(g, home_icon.icon3->obj);
    lv_group_add_obj(g, home_icon.icon4->obj);
}

static void start_reflow(){
    reflow_oven_anim_out_all(lv_scr_act(), 0);

    lv_obj_t * title = add_title("Reflow started", lv_scr_act(), REFLOW_OVEN_THEME_TITLE_WHITE);
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, REFLOW_OVEN_TITLE_PAD);

    lv_coord_t box_w = 160;
    lv_obj_t * box = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(box, box_w, box_w);
    lv_obj_align(box, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_bg_color(box, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    lv_group_remove_all_objs(g);

    lv_obj_t * profile_title = lv_label_create(box, NULL);
    lv_label_set_long_mode(profile_title, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(profile_title, box_w - 10, 20);
    lv_obj_set_style_local_text_font(profile_title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_normal());
    lv_obj_set_style_local_bg_color(profile_title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, REFLOW_OVEN_BLUE);
    lv_label_set_text(profile_title, reflow_profiles[reflow_oven_setting.current_reflow_profile]->title);
    lv_obj_align(profile_title, box, LV_ALIGN_IN_TOP_MID, 0, 0);

    lv_obj_t * chart_base = lv_chart_create(box, NULL);
    lv_obj_t * chart_data = lv_chart_create(box, NULL);

    uint16_t reflow_profile_horizontal_max_dur = reflow_profiles[reflow_oven_setting.current_reflow_profile]->presoak_dur +
                                                reflow_profiles[reflow_oven_setting.current_reflow_profile]->soak_dur +
                                                reflow_profiles[reflow_oven_setting.current_reflow_profile]->reflow_begin_dur +
                                                reflow_profiles[reflow_oven_setting.current_reflow_profile]->reflow_hold_dur +
                                                reflow_profiles[reflow_oven_setting.current_reflow_profile]->cooldown_dur;
    uint16_t reflow_profile_presoak_start_time = 0;
    uint16_t reflow_profile_soak_start_time = reflow_profiles[reflow_oven_setting.current_reflow_profile]->presoak_dur;
    uint16_t reflow_profile_reflow_begin_start_time = reflow_profile_soak_start_time + reflow_profiles[reflow_oven_setting.current_reflow_profile]->soak_dur;
    uint16_t reflow_profile_reflow_hold_start_time = reflow_profile_reflow_begin_start_time + reflow_profiles[reflow_oven_setting.current_reflow_profile]->reflow_begin_dur;
    uint16_t reflow_profile_cooldown_start_time = reflow_profile_reflow_hold_start_time + reflow_profiles[reflow_oven_setting.current_reflow_profile]->reflow_hold_dur;


    uint16_t chart_horizontal_max_points = reflow_profile_horizontal_max_dur; //each point representing 1s
    lv_obj_set_width(chart_base, box_w);
    lv_obj_set_height(chart_base, box_w);
    lv_chart_set_point_count(chart_base, chart_horizontal_max_points);
    lv_chart_set_range(chart_base, 0, 250);
    lv_obj_set_style_local_bg_opa(chart_base, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_chart_set_type(chart_base, LV_CHART_TYPE_LINE);
    lv_theme_apply(chart_base, REFLOW_OVEN_THEME_CHART);
    lv_chart_set_y_tick_length(chart_base, LV_CHART_TICK_LENGTH_AUTO, LV_CHART_TICK_LENGTH_AUTO);
    lv_chart_set_x_tick_length(chart_base, LV_CHART_TICK_LENGTH_AUTO, LV_CHART_TICK_LENGTH_AUTO);

    //Generate tick labels
    static char x_tick_texts[32];
    memset(x_tick_texts, 0, 32);
    for(uint16_t i=0; i<ceil(chart_horizontal_max_points/60.0) + 1; i++){
        char mins[2];
        sprintf(mins, "%u", i);
        strcat(x_tick_texts, mins);
        if(i<ceil(chart_horizontal_max_points/60.0)){
            strcat(x_tick_texts, "\n");
        }
    }

    lv_chart_set_y_tick_texts(chart_base, "250\n200\n150\n100\n50\n0", 0, LV_CHART_AXIS_DRAW_LAST_TICK);
    lv_chart_set_x_tick_texts(chart_base, x_tick_texts, 0, LV_CHART_AXIS_DRAW_LAST_TICK);

    lv_chart_series_t * s1 = lv_chart_add_series(chart_base, REFLOW_OVEN_LIGHT_GRAY);

    //I know this is inefficient, a terrible waste of memory but LVGL currently doesn't support what i want,
    for(uint16_t i=0; i<chart_horizontal_max_points; i++){
        int16_t y_temp = 0;
        if(i<reflow_profile_soak_start_time){
            y_temp = linear_interpolate(i, reflow_profiles[reflow_oven_setting.current_reflow_profile]->presoak_dur, reflow_profiles[reflow_oven_setting.current_reflow_profile]->presoak_start_temp, reflow_profiles[reflow_oven_setting.current_reflow_profile]->presoak_end_temp);
            lv_chart_set_next(chart_base, s1, y_temp);
        }else if(i>=reflow_profile_soak_start_time&&i<reflow_profile_reflow_begin_start_time){
            y_temp = linear_interpolate(i - reflow_profile_soak_start_time, reflow_profiles[reflow_oven_setting.current_reflow_profile]->soak_dur, reflow_profiles[reflow_oven_setting.current_reflow_profile]->soak_start_temp, reflow_profiles[reflow_oven_setting.current_reflow_profile]->soak_end_temp);
            lv_chart_set_next(chart_base, s1, y_temp);
        }else if(i>=reflow_profile_reflow_begin_start_time&&i<reflow_profile_reflow_hold_start_time){
            y_temp = linear_interpolate(i - reflow_profile_reflow_begin_start_time, reflow_profiles[reflow_oven_setting.current_reflow_profile]->reflow_begin_dur, reflow_profiles[reflow_oven_setting.current_reflow_profile]->reflow_begin_start_temp, reflow_profiles[reflow_oven_setting.current_reflow_profile]->reflow_begin_end_temp);
            lv_chart_set_next(chart_base, s1, y_temp);
        }else if(i>=reflow_profile_reflow_hold_start_time&&i<reflow_profile_cooldown_start_time){
            y_temp = linear_interpolate(i - reflow_profile_reflow_hold_start_time, reflow_profiles[reflow_oven_setting.current_reflow_profile]->reflow_hold_dur, reflow_profiles[reflow_oven_setting.current_reflow_profile]->reflow_hold_start_temp, reflow_profiles[reflow_oven_setting.current_reflow_profile]->reflow_hold_end_temp);
            lv_chart_set_next(chart_base, s1, y_temp);
        }else if(i>=reflow_profile_cooldown_start_time&&i<reflow_profile_horizontal_max_dur){
            y_temp = linear_interpolate(i - reflow_profile_cooldown_start_time, reflow_profiles[reflow_oven_setting.current_reflow_profile]->cooldown_dur, reflow_profiles[reflow_oven_setting.current_reflow_profile]->cooldown_start_temp, reflow_profiles[reflow_oven_setting.current_reflow_profile]->cooldown_end_temp);
            lv_chart_set_next(chart_base, s1, y_temp);
        }else{
            lv_chart_set_next(chart_base, s1, LV_CHART_POINT_DEF);
        }
    }

    lv_obj_set_height(chart_data, box_w);
    lv_obj_set_width(chart_data, box_w);
    lv_chart_set_point_count(chart_data, chart_horizontal_max_points);
    lv_chart_set_range(chart_data, 0, 250);
    lv_chart_set_type(chart_data, LV_CHART_TYPE_LINE);
    lv_theme_apply(chart_data, REFLOW_OVEN_THEME_CHART);

    lv_chart_series_t * s2 = lv_chart_add_series(chart_data, REFLOW_OVEN_RED);

    temp_log_setup(chart_horizontal_max_points);
    lv_coord_t *temp_readings = temp_log_get();

    for(int i=0; i<100; i++){
        temp_readings[i]=linear_interpolate(i, 100, 50, 150);
    }
    for(int i=100; i<150; i++){
        temp_readings[i]=linear_interpolate(i, 50, 130, 150);
    }
    for(int i=150; i<chart_horizontal_max_points; i++){
        temp_readings[i]=LV_CHART_POINT_DEF;
    }

    uint16_t current_time_past = 150;

    lv_chart_set_ext_array(chart_data, s2, temp_readings, chart_horizontal_max_points);
    lv_chart_refresh(chart_data);

    reflow_bottom_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_theme_apply(reflow_bottom_btn, REFLOW_OVEN_THEME_BTN_NEGATIVE);
    lv_obj_set_size(reflow_bottom_btn, 100, 20);
    lv_obj_align(reflow_bottom_btn, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_local_value_str(reflow_bottom_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "Abort");
    lv_obj_set_style_local_value_font(reflow_bottom_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_event_cb(reflow_bottom_btn, abort_reflow_event_cb);

    lv_group_add_obj(g, reflow_bottom_btn);
    lv_group_focus_obj(reflow_bottom_btn);
}

static void toggle_door(){
    uint32_t delay = 200;

    lv_obj_t * bg_shade = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_opa(bg_shade, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_30);
    lv_obj_set_style_local_bg_color(bg_shade, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(bg_shade, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_VER_RES/2);
    lv_obj_set_size(bg_shade, LV_HOR_RES, LV_VER_RES);

    lv_obj_t * popup = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(popup, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_size(popup, 200, 70);
    lv_obj_align(popup, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * title = add_title("Doors closing...", popup, REFLOW_OVEN_THEME_TITLE);

    //Not sure why cant set bg color, temp workaround
    lv_obj_t * bar0 = lv_bar_create(popup, NULL);
    lv_obj_set_size(bar0, 180, 15);
    lv_obj_align(bar0, NULL, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_local_bg_color(bar0, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, REFLOW_OVEN_LIGHT_GRAY);
    lv_bar_set_value(bar0, 100, LV_ANIM_OFF);

    lv_obj_t * bar1 = lv_bar_create(popup, NULL);
    lv_obj_set_size(bar1, 180, 15);
    lv_obj_align(bar1, NULL, LV_ALIGN_CENTER, 0, 10);
    lv_theme_apply(bar1, LV_THEME_BAR);
    lv_bar_set_anim_time(bar1, 3000);
    lv_bar_set_value(bar1, 100, LV_ANIM_ON);

    /*
    lv_obj_t * reflow_bottom_btn = lv_btn_create(popup, NULL);
    lv_theme_apply(reflow_bottom_btn, REFLOW_OVEN_THEME_BTN_NEGATIVE);
    lv_obj_set_size(reflow_bottom_btn, 180, 20);
    lv_obj_align(reflow_bottom_btn, popup, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_local_value_str(reflow_bottom_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "Cancel");
    lv_obj_set_style_local_value_font(reflow_bottom_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    */

    lv_group_remove_all_objs(g);

    /*
    lv_group_add_obj(g, reflow_bottom_btn);
    lv_group_focus_obj(reflow_bottom_btn);
    */

    reflow_oven_anim_in(popup, delay);

    reflow_oven_anim_out(bg_shade, 2200);
    reflow_oven_anim_out(popup, 2200);

    reflow_oven_focus_door_icon();
}

void reflow_oven_focus_door_icon(){
    lv_group_remove_all_objs(g);
    add_home_icon_group();
    //workaround
    lv_style_int_t trans_delay = lv_obj_get_style_transition_delay(home_icon.icon2->obj, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_transition_delay(home_icon.icon1->obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    //
    lv_group_focus_obj(home_icon.icon2->obj);
    //workaround
    lv_obj_set_style_local_transition_delay(home_icon.icon1->obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, trans_delay);
}
static void open_settings(){
    reflow_oven_anim_out_all(lv_scr_act(), 0);

    lv_obj_t * title = add_title("Settings", lv_scr_act(), REFLOW_OVEN_THEME_TITLE_WHITE);
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, REFLOW_OVEN_TITLE_PAD);

    lv_coord_t box_w = 160;
    lv_obj_t * box = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(box, box_w, box_w);
    lv_obj_align(box, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_group_remove_all_objs(g);

    lv_ex_settings_2(box);
}
static lv_settings_item_t profiles_root_item = {.name = "Back", .value = NULL};

static struct lv_settings_item ** main_profile_menu_items;

static void main_profile_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {
        uint32_t i;
        for(i = 0; main_profile_menu_items[i]->type != LV_SETTINGS_TYPE_INV; i++) {
            if(main_profile_menu_items[i]->type==LV_SETTINGS_TYPE_LIST_BTN){
            lv_obj_t * new_btn = lv_settings_add(main_profile_menu_items[i]);

            if(i==reflow_oven_setting.current_reflow_profile){
                lv_obj_set_style_local_text_color(lv_obj_get_child(new_btn, NULL), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, REFLOW_OVEN_RED);
            }
            }
        }
    }
    else if(e == LV_EVENT_CLICKED) {
        uint8_t num_profiles = sizeof(reflow_profiles) / sizeof reflow_profiles[0];

        for(uint8_t i=0; i < num_profiles; i++){
            if(strcmp(act_item->name, reflow_profiles[i]->title) == 0) {
                lv_obj_t * page = lv_obj_get_parent(act_item->cont);
                lv_obj_t * name = lv_obj_get_child(act_item->cont, NULL);

                lv_obj_t * list_btn = NULL;
                for(uint8_t i=0; i < reflow_oven_setting.current_reflow_profile + 1; i++){
                    list_btn = lv_obj_get_child_back(page, list_btn);
                    if(i==reflow_oven_setting.current_reflow_profile){
                        lv_obj_set_style_local_text_color(lv_obj_get_child(list_btn, NULL), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, REFLOW_OVEN_BLUE);
                        break;
                    }
                }

                lv_obj_set_style_local_text_color(name, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, REFLOW_OVEN_RED);

                reflow_oven_setting.current_reflow_profile = i;
                break;
            }
        }
    }
}
static void open_profiles(){
    reflow_oven_anim_out_all(lv_scr_act(), 0);
    lv_obj_t * title = add_title("Profiles", lv_scr_act(), REFLOW_OVEN_THEME_TITLE_WHITE);
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, REFLOW_OVEN_TITLE_PAD);

    lv_coord_t box_w = 160;
    lv_obj_t * box = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(box, box_w, box_w);
    lv_obj_align(box, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_group_remove_all_objs(g);

    uint8_t num_profiles = sizeof(reflow_profiles) / sizeof reflow_profiles[0];

    lv_settings_set_group(g);

    if(main_profile_menu_items==NULL){
        main_profile_menu_items = malloc((num_profiles + 1) * sizeof(struct lv_settings_item*));
        for(uint8_t i=0; i < num_profiles + 1; i++){
            main_profile_menu_items[i] = malloc(sizeof(struct lv_settings_item));
            main_profile_menu_items[i]->value = NULL;
            if(i < num_profiles){
            main_profile_menu_items[i]->type = LV_SETTINGS_TYPE_LIST_BTN;
            main_profile_menu_items[i]->name = reflow_profiles[i]->title;
            }else{
                main_profile_menu_items[i]->type = LV_SETTINGS_TYPE_INV;
            }
        }
    }
    lv_settings_create(&profiles_root_item, box, 2, back_to_home_event_cb);
    lv_settings_open_page(&profiles_root_item, main_profile_menu_event_cb);
    lv_settings_set_max_width(lv_obj_get_width(box));
}

void reflow_complete(){
    lv_theme_apply(reflow_bottom_btn, REFLOW_OVEN_THEME_BTN_POSITIVE);
    lv_obj_set_style_local_value_str(reflow_bottom_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "Complete");
    lv_obj_set_event_cb(reflow_bottom_btn, back_to_home_event_cb);
}

LV_EVENT_CB_DECLARE(abort_reflow_event_cb){
     if (e == LV_EVENT_CLICKED) {
        bg_shade = lv_obj_create(lv_scr_act(), NULL);
        lv_obj_set_style_local_bg_opa(bg_shade, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_30);
        lv_obj_set_style_local_bg_color(bg_shade, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        lv_obj_set_style_local_radius(bg_shade, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_VER_RES/2);
        lv_obj_set_size(bg_shade, LV_HOR_RES, LV_VER_RES);

        popup = lv_obj_create(lv_scr_act(), NULL);
        lv_obj_set_style_local_bg_color(popup, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_obj_set_size(popup, 200, 80);
        lv_obj_align(popup, NULL, LV_ALIGN_CENTER, 0, 0);

        abort_title = add_title("Do you want abort?", popup, REFLOW_OVEN_THEME_TITLE);

        abort_btn = lv_btn_create(popup, NULL);
        lv_theme_apply(abort_btn, REFLOW_OVEN_THEME_BTN_NEGATIVE);
        lv_obj_set_size(abort_btn, 80, 20);
        lv_obj_align(abort_btn, popup, LV_ALIGN_IN_BOTTOM_MID, 50, -10);
        lv_obj_set_style_local_value_str(abort_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "Abort");
        lv_obj_set_style_local_value_font(abort_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
        lv_obj_set_event_cb(abort_btn, back_to_home_event_cb);

        no_btn = lv_btn_create(popup, NULL);
        lv_theme_apply(no_btn, REFLOW_OVEN_THEME_BTN_NEUTRAL);
        lv_obj_set_size(no_btn, 80, 20);
        lv_obj_align(no_btn, popup, LV_ALIGN_IN_BOTTOM_MID, -50, -10);
        lv_obj_set_style_local_value_str(no_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "No");
        lv_obj_set_style_local_value_font(no_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
        lv_obj_set_event_cb(no_btn, no_abort_event_cb);

        lv_group_remove_all_objs(g);
        lv_group_add_obj(g, no_btn);
        lv_group_add_obj(g, abort_btn);
        lv_group_focus_obj(no_btn);
     }
}

LV_EVENT_CB_DECLARE(no_abort_event_cb){
    if (e == LV_EVENT_CLICKED) {
    reflow_oven_anim_out(bg_shade, 0);
    reflow_oven_anim_out(popup, 0);
    reflow_oven_anim_out(abort_title, 0);
    reflow_oven_anim_out(abort_btn, 0);
    reflow_oven_anim_out(no_btn, 0);

    lv_group_add_obj(g, reflow_bottom_btn);
    lv_group_focus_obj(reflow_bottom_btn);
    }
}

void reflow_oven_update_temperature(char* temp){
    lv_label_set_text(temp_state_icon, temp);
    lv_obj_align(temp_state_icon, NULL, LV_ALIGN_CENTER, 0, -6);
}

void reflow_oven_error_popup(char* msg, uint8_t error_code_val){
    if(!error_state){
        error_state = true;
		lv_obj_t * bg_shade = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_style_local_bg_opa(bg_shade, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_60);
		lv_obj_set_style_local_bg_color(bg_shade, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
		lv_obj_set_style_local_radius(bg_shade, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_VER_RES/2);
		lv_obj_set_size(bg_shade, LV_HOR_RES, LV_VER_RES);
		lv_obj_t * popup = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_size(popup, 200, 80);
		lv_obj_align(popup, NULL, LV_ALIGN_CENTER, 0, 0);
		lv_obj_t * title = add_title("A fatal error occurred!", popup, REFLOW_OVEN_THEME_TITLE_WHITE);
		lv_obj_set_style_local_text_color(title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, REFLOW_OVEN_RED);
		lv_obj_t * desc = lv_label_create(popup, NULL);
		char final_msg[50];
		char error_code[8];
		strcpy(final_msg, "ERR: ");
		strcat(final_msg, msg);
		if(error_code_val>0){
			sprintf(error_code, " [%u]", error_code_val);
			strcat(final_msg, error_code);
		}
		lv_label_set_text(desc, final_msg);
		lv_obj_align(desc, NULL, LV_ALIGN_CENTER, 0, 0);
		lv_obj_t * help = lv_label_create(popup, NULL);
		lv_obj_set_style_local_text_color(help, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, REFLOW_OVEN_GRAY);
		lv_label_set_text(help, "Check the wiring and restart");
		lv_obj_align(help, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
		lv_group_remove_all_objs(g);
		//System will be stuck here unless reboot
	}
}

LV_EVENT_CB_DECLARE(start_reflow_icon_event_cb){
     if (e == LV_EVENT_CLICKED) {
        start_reflow();
    }
    icon_generic_event_cb(obj, e);
}

LV_EVENT_CB_DECLARE(toggle_door_icon_event_cb) {
    if (e == LV_EVENT_CLICKED) {
        toggle_door();
    }
    icon_generic_event_cb(obj, e);
}

LV_EVENT_CB_DECLARE(open_settings_icon_event_cb) {
    if (e == LV_EVENT_CLICKED) {
        open_settings();
    }
    icon_generic_event_cb(obj, e);
}

LV_EVENT_CB_DECLARE(open_profiles_icon_event_cb) {
    if (e == LV_EVENT_CLICKED) {
        open_profiles();
    }
    icon_generic_event_cb(obj, e);
}

LV_EVENT_CB_DECLARE(back_to_home_event_cb)
{
    if (e == LV_EVENT_CLICKED) {
        reflow_oven_anim_out_all(lv_scr_act(), 0);

        home_open(200);

        uint32_t icon_selected = (uint32_t)lv_obj_get_user_data(obj);

        if(icon_selected>0){
            //workaround
            lv_style_int_t trans_delay = lv_obj_get_style_transition_delay(home_icon.icon2->obj, LV_OBJ_PART_MAIN);
            lv_obj_set_style_local_transition_delay(home_icon.icon1->obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
            //
            if(icon_selected==1){
                lv_group_focus_obj(home_icon.icon2->obj);
            }else if(icon_selected==2){
                lv_group_focus_obj(home_icon.icon3->obj);
                settings_save();
            }else if(icon_selected==3){
                lv_group_focus_obj(home_icon.icon4->obj);
                settings_save();
            }
            //workaround
            lv_obj_set_style_local_transition_delay(home_icon.icon1->obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, trans_delay);
        }
    }
}

static icon_t * add_icon(lv_obj_t * parent, const void * src, const void * label_src, lv_color_t color) {
    lv_obj_t * cont = lv_cont_create(parent, NULL);
    lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_cont_set_fit(cont, LV_FIT_TIGHT);
    lv_obj_set_style_local_pad_right(cont, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_pad_left(cont, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_pad_top(cont, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_pad_bottom(cont, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 5);

    lv_obj_t * img_icon = lv_img_create(cont, NULL);
    lv_theme_apply(img_icon, REFLOW_OVEN_THEME_ICON);
    lv_img_set_src(img_icon, src);
    lv_obj_set_click(img_icon, true);

    lv_obj_t * img_label = lv_img_create(cont, NULL);
    lv_theme_apply(img_label, REFLOW_OVEN_THEME_ICON_LABEL);
    lv_img_set_src(img_label, label_src);
    lv_obj_set_style_local_image_recolor(img_label, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(img_label, cont, LV_ALIGN_IN_BOTTOM_MID, 0, 12);
    lv_obj_set_user_data(img_label, lv_obj_get_y(img_label));
    icon_t * icon_struct = malloc(sizeof(icon_t));
    icon_struct->cont = cont;
    icon_struct->obj = img_icon;
    return icon_struct;
}

static lv_obj_t * add_title(const char * txt, lv_obj_t * src, reflow_oven_theme_t theme)
{
    lv_obj_t * title = lv_label_create(src, NULL);
    lv_theme_apply(title, theme);
    lv_label_set_text(title, txt);
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0,
            REFLOW_OVEN_TITLE_PAD);
    return title;
}

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    DISPLAY_AND_SOUND_DISPLAY_BRIGHTNESS = 0,
    DISPLAY_AND_SOUND_SPEAKER_VOLUME,
    MOTOR_ITEM_TYPE,
    MOTOR_ITEM_PROTECT,
    MOTOR_ITEM_PROTECT2,
}motor_item_t;

typedef enum {
    PID_ITEM_PROPORTIONAL = 0,
    PID_ITEM_INTEGRAL,
    PID_ITEM_DERIVATIVE
}pid_item_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void main_menu_event_cb(lv_obj_t * btn, lv_event_t e);
static void display_and_sound_menu_event_cb(lv_obj_t * btn, lv_event_t e);
static void pid_menu_event_cb(lv_obj_t * btn, lv_event_t e);
static void pid_presoak_menu_event_cb(lv_obj_t * btn, lv_event_t e);
static void pid_soak_menu_event_cb(lv_obj_t * btn, lv_event_t e);
static void pid_reflow_menu_event_cb(lv_obj_t * btn, lv_event_t e);
static void about_menu_event_cb(lv_obj_t * btn, lv_event_t e);
static void factory_reset_menu_event_cb(lv_obj_t * btn, lv_event_t e);
/**********************
 *  STATIC VARIABLES
 **********************/

/*Declare items*/
static lv_settings_item_t settings_root_item = {.name = "Back", .value = NULL};

static lv_settings_item_t main_menu_items[] =
{
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Display and Sound", .value = NULL},
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="PID Parameters", .value = NULL},
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="About", .value = NULL},
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Factory Reset", .value = NULL},
       {.type = LV_SETTINGS_TYPE_INV},     /*Mark the last item*/
};

static char firmware_version_value[8];
static char display_brightness_value[8];
static char speaker_volume_value[8];

static char pid_p_presoak_value[8];
static char pid_i_presoak_value[8];
static char pid_d_presoak_value[8];
static char pid_p_soak_value[8];
static char pid_i_soak_value[8];
static char pid_d_soak_value[8];
static char pid_p_reflow_value[8];
static char pid_i_reflow_value[8];
static char pid_d_reflow_value[8];

static slider_config_t slider_range = {
    .min_step = 1,
    .max_step = 16
};
static slider_config_t pid_p_slider_range = {
    .min_step = 0,
    .max_step = 100
};
static slider_config_t pid_i_slider_range = {
    .min_step = 0,
    .max_step = 100
};
static slider_config_t pid_d_slider_range = {
    .min_step = 0,
    .max_step = 100
};

static lv_settings_item_t display_and_sound_menu_items[] = {
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Brightness", .value = display_brightness_value, .data = &slider_range},
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Volume", .value = speaker_volume_value, .data = &slider_range},
    {.type = LV_SETTINGS_TYPE_DDLIST, .name = "Motor type", .value = "Type 1\n"
                                                                    "Type 2\n"
                                                                    "Type 3"},
    {.type = LV_SETTINGS_TYPE_SW, .name = "Short circuit protect", .value = ""},
    {.type = LV_SETTINGS_TYPE_SW, .name = "Short circuit protect2", .value = ""},
    {.type = LV_SETTINGS_TYPE_INV},     /*Mark the last item*/
};

static lv_settings_item_t pid_menu_items[] = {
    {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Presoak", .value = NULL},
    {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Soak", .value = NULL},
    {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Reflow", .value = NULL},
    {.type = LV_SETTINGS_TYPE_INV},       /*Mark the last item*/
};
static lv_settings_item_t pid_presoak_menu_items[] = {
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Proportional", .value=pid_p_presoak_value, .data = &pid_p_slider_range},
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Integral", .value=pid_i_presoak_value, .data = &pid_i_slider_range},
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Derivative", .value=pid_d_presoak_value, .data = &pid_d_slider_range},
    {.type = LV_SETTINGS_TYPE_INV},       /*Mark the last item*/
};
static lv_settings_item_t pid_soak_menu_items[] = {
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Proportional", .value=pid_p_soak_value, .data = &pid_p_slider_range},
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Integral", .value=pid_i_soak_value, .data = &pid_i_slider_range},
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Derivative", .value=pid_d_soak_value, .data = &pid_d_slider_range},
    {.type = LV_SETTINGS_TYPE_INV},       /*Mark the last item*/
};
static lv_settings_item_t pid_reflow_menu_items[] = {
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Proportional", .value=pid_p_reflow_value, .data = &pid_p_slider_range},
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Integral", .value=pid_i_reflow_value, .data = &pid_i_slider_range},
    {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Derivative", .value=pid_d_reflow_value, .data = &pid_d_slider_range},
    {.type = LV_SETTINGS_TYPE_INV},       /*Mark the last item*/
};
static lv_settings_item_t about_menu_items[] =
{
    {.type = LV_SETTINGS_TYPE_LIST_BTN, .name = "Firmware version", .value = firmware_version_value},
    {.type = LV_SETTINGS_TYPE_LIST_BTN, .name = "Designed by", .value = "HX2003"},
    {.type = LV_SETTINGS_TYPE_LIST_BTN, .name = "Settings dump", .value = ""},
    {.type = LV_SETTINGS_TYPE_INV},    /*Mark the last item*/
};
static lv_settings_item_t factory_reset_menu_items[] =
{
    {.type = LV_SETTINGS_TYPE_LIST_BTN, .name = "Confirm factory reset", .value = NULL},
    {.type = LV_SETTINGS_TYPE_INV},    /*Mark the last item*/
};
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


void lv_ex_settings_2(lv_obj_t * parent)
{
    lv_settings_set_group(g);

    /*Load the default values*/
    lv_settings_item_t * i;

    /*Display and sound menu*/
    i = &display_and_sound_menu_items[DISPLAY_AND_SOUND_DISPLAY_BRIGHTNESS];
    i->state = (int32_t)reflow_oven_setting.display_brightness;
    sprintf(i->value, "%.2f%%", (6.25)*i->state);

    i = &display_and_sound_menu_items[DISPLAY_AND_SOUND_SPEAKER_VOLUME];
    i->state = (int32_t)reflow_oven_setting.speaker_volume;
    sprintf(i->value, "%.2f%%", (6.25)*i->state);

    i = &display_and_sound_menu_items[MOTOR_ITEM_TYPE];
    i->state = 1;

    i = &display_and_sound_menu_items[MOTOR_ITEM_PROTECT];
    i->state = 0;
    i->value = "Not protected";

    i = &display_and_sound_menu_items[MOTOR_ITEM_PROTECT2];
    i->state = 0;
    i->value = "Not protected";

    /*PID menus*/
    i = &pid_presoak_menu_items[PID_ITEM_PROPORTIONAL];
    i->state = (int32_t)reflow_oven_setting.pid_p_presoak/1.0;
    sprintf(i->value, "%.2f", (1.0)*i->state);

    i = &pid_presoak_menu_items[PID_ITEM_INTEGRAL];
    i->state = (int32_t)(100*reflow_oven_setting.pid_i_presoak);
    sprintf(i->value, "%.2f", (0.01)*i->state);

    i = &pid_presoak_menu_items[PID_ITEM_DERIVATIVE];
    i->state = (int32_t)reflow_oven_setting.pid_d_presoak/1.0;
    sprintf(i->value, "%.2f", (1.0)*i->state);
    //
    i = &pid_soak_menu_items[PID_ITEM_PROPORTIONAL];
    i->state = (int32_t)reflow_oven_setting.pid_p_soak/1.0;
    sprintf(i->value, "%.2f", (1.0)*i->state);

    i = &pid_soak_menu_items[PID_ITEM_INTEGRAL];
    i->state = (int32_t)(100*reflow_oven_setting.pid_i_soak);
    sprintf(i->value, "%.2f", (0.01)*i->state);

    i = &pid_soak_menu_items[PID_ITEM_DERIVATIVE];
    i->state = (int32_t)reflow_oven_setting.pid_d_soak/1.0;
    sprintf(i->value, "%.2f", (1.0)*i->state);
    //
    i = &pid_reflow_menu_items[PID_ITEM_PROPORTIONAL];
    i->state = (int32_t)reflow_oven_setting.pid_p_reflow/1.0;
    sprintf(i->value, "%.2f", (1.0)*i->state);

    i = &pid_reflow_menu_items[PID_ITEM_INTEGRAL];
    i->state = (int32_t)(100*reflow_oven_setting.pid_i_reflow);
    sprintf(i->value, "%.2f", (0.01)*i->state);

    i = &pid_reflow_menu_items[PID_ITEM_DERIVATIVE];
    i->state = (int32_t)reflow_oven_setting.pid_d_reflow/1.0;
    sprintf(i->value, "%.2f", (1.0)*i->state);

    lv_settings_create(&settings_root_item, parent, 3, back_to_home_event_cb);
    lv_settings_open_page(&settings_root_item, main_menu_event_cb);
    lv_settings_set_max_width(lv_obj_get_width(parent));

    /*About menus*/
    i = &about_menu_items[0];
    sprintf(i->value, "%d.%d.%d", REFLOW_OVEN_FIRMWARE_MAJOR_VERSION, REFLOW_OVEN_FIRMWARE_MINOR_VERSION, REFLOW_OVEN_FIRMWARE_PATCH_VERSION);
    i = &about_menu_items[2];
    i->value = settings_dump();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void main_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {

        uint32_t i;
        for(i = 0; main_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&main_menu_items[i]);
        }
    }
    else if(e == LV_EVENT_CLICKED) {
        if(strcmp(act_item->name, "Display and Sound") == 0) {
           lv_settings_open_page(act_item, display_and_sound_menu_event_cb);
        }
        else if(strcmp(act_item->name, "PID Parameters") == 0) {
           lv_settings_open_page(act_item, pid_menu_event_cb);
        }
        else if(strcmp(act_item->name, "About") == 0) {
           lv_settings_open_page(act_item, about_menu_event_cb);
        }
        else if(strcmp(act_item->name, "Factory Reset") == 0) {
           lv_settings_open_page(act_item, factory_reset_menu_event_cb);
        }
    }
}

static void display_and_sound_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {
       /*Add the display_and_sound_menu_items*/
       uint32_t i;
       for(i = 0; display_and_sound_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
           lv_settings_add(&display_and_sound_menu_items[i]);
       }
    }
    else if(e == LV_EVENT_VALUE_CHANGED) {
        if(strcmp("Brightness", act_item->name) == 0) {
            reflow_oven_setting.display_brightness = act_item->state;
            sprintf(act_item->value, "%.2f%%", (6.25)*act_item->state);
            lv_settings_refr(act_item);
        }
        if(strcmp("Volume", act_item->name) == 0) {
            reflow_oven_setting.speaker_volume = act_item->state;
            sprintf(act_item->value, "%.2f%%", (6.25)*act_item->state);
            lv_settings_refr(act_item);
        }
        else if(strcmp("Motor type", act_item->name) == 0) {
            printf("ddlist: %d\n", act_item->state);
        }
        else if(strcmp("Short circuit protect", act_item->name) == 0) {
            if(act_item->state) act_item->value = "Protected";
            else act_item->value = "Not protected";
            lv_settings_refr(act_item);
        }else if(strcmp("Short circuit protect2", act_item->name) == 0) {
            if(act_item->state) act_item->value = "Protected";
            else act_item->value = "Not protected";
            lv_settings_refr(act_item);
        }
    }
    else if(e == LV_EVENT_CLICKED) {
        if(strcmp("Test motor", act_item->name) == 0) {
            if(strcmp("Start", act_item->value) == 0) act_item->value = "Stop";
            else act_item->value = "Start";

            lv_settings_refr(act_item);
        }
    }
}

static void pid_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller act_item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {
        /*Add the pid_menu_items*/
        uint32_t i;
        for(i = 0; pid_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&pid_menu_items[i]);
        }
    }
    /*Handle the events of the PID menu items*/
    else if(e == LV_EVENT_CLICKED) {
        if(strcmp(act_item->name, "Presoak") == 0) {
           lv_settings_open_page(act_item, pid_presoak_menu_event_cb);
        }
        else if(strcmp(act_item->name, "Soak") == 0) {
           lv_settings_open_page(act_item, pid_soak_menu_event_cb);
        }
        else if(strcmp(act_item->name, "Reflow") == 0) {
           lv_settings_open_page(act_item, pid_reflow_menu_event_cb);
        }
    }
}

static void pid_presoak_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller act_item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {
        /*Add the pid_presoak_menu_items*/
        uint32_t i;
        for(i = 0; pid_presoak_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&pid_presoak_menu_items[i]);
        }
    }
    else if(e == LV_EVENT_VALUE_CHANGED) {
        if(strcmp("Proportional", act_item->name) == 0) {
            reflow_oven_setting.pid_p_presoak = (1.0)*act_item->state;
            sprintf(act_item->value, "%.2f", (1.0)*act_item->state);
            lv_settings_refr(act_item);
        }
        else if(strcmp("Integral", act_item->name) == 0) {
            reflow_oven_setting.pid_i_presoak = (0.01)*act_item->state;
            sprintf(act_item->value, "%.2f", (0.01)*act_item->state);
            lv_settings_refr(act_item);
        }
        else if(strcmp("Derivative", act_item->name) == 0) {
            reflow_oven_setting.pid_d_presoak = (1.0)*act_item->state;
            sprintf(act_item->value, "%.2f", (1.0)*act_item->state);
            lv_settings_refr(act_item);
        }
    }
}

static void pid_soak_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller act_item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {
        /*Add the pid_soak_menu_items*/
        uint32_t i;
        for(i = 0; pid_soak_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&pid_soak_menu_items[i]);
        }
    }
    else if(e == LV_EVENT_VALUE_CHANGED) {
        if(strcmp("Proportional", act_item->name) == 0) {
            reflow_oven_setting.pid_p_soak = (1.0)*act_item->state;
            sprintf(act_item->value, "%.2f", (1.0)*act_item->state);
            lv_settings_refr(act_item);
        }
        else if(strcmp("Integral", act_item->name) == 0) {
            reflow_oven_setting.pid_i_soak = (0.01)*act_item->state;
            sprintf(act_item->value, "%.2f", (0.01)*act_item->state);
            lv_settings_refr(act_item);
        }
        else if(strcmp("Derivative", act_item->name) == 0) {
            reflow_oven_setting.pid_d_soak = (1.0)*act_item->state;
            sprintf(act_item->value, "%.2f", (1.0)*act_item->state);
            lv_settings_refr(act_item);
        }
    }
}

static void pid_reflow_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller act_item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {
        /*Add the pid_reflow_menu_items*/
        uint32_t i;
        for(i = 0; pid_reflow_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&pid_reflow_menu_items[i]);
        }
    }
    else if(e == LV_EVENT_VALUE_CHANGED) {
        if(strcmp("Proportional", act_item->name) == 0) {
            reflow_oven_setting.pid_p_reflow = (1.0)*act_item->state;
            sprintf(act_item->value, "%.2f", (1.0)*act_item->state);
            lv_settings_refr(act_item);
        }
        else if(strcmp("Integral", act_item->name) == 0) {
            reflow_oven_setting.pid_i_reflow = (0.01)*act_item->state;
            sprintf(act_item->value, "%.2f", (0.01)*act_item->state);
            lv_settings_refr(act_item);
        }
        else if(strcmp("Derivative", act_item->name) == 0) {
            reflow_oven_setting.pid_d_reflow = (1.0)*act_item->state;
            sprintf(act_item->value, "%.2f", (1.0)*act_item->state);
            lv_settings_refr(act_item);
        }
    }
}

static void about_menu_event_cb(lv_obj_t * obj, lv_event_t e)
{
    (void)obj;  /*Unused*/

    /*Get the caller act_item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {

        /*Add the items*/
        uint32_t i;
        for(i = 0; about_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&about_menu_items[i]);
        }
    }
    else if(e == LV_EVENT_VALUE_CHANGED) {
            /*
        if(strcmp("Firmware version", act_item->name) == 0) {
            if(act_item->state > 23) act_item->state = 0;
            if(act_item->state < 0) act_item->state = 23;

            sprintf(act_item->value, "%d", act_item->state);
            lv_settings_refr(act_item);

            sprintf(pid_menu_items[0].value, "%02d:%02d", time_menu_items[0].state, time_menu_items[1].state);

        } */
    }
}
static void factory_reset_menu_event_cb(lv_obj_t * obj, lv_event_t e)
{
    (void)obj;  /*Unused*/

    /*Get the caller act_item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {

        /*Add the items*/
        uint32_t i;
        for(i = 0; factory_reset_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&factory_reset_menu_items[i]);
        }
    }
    else if(e == LV_EVENT_CLICKED) {
        if(strcmp("Confirm Factory Reset", act_item->name) == 0) {
            settings_reset(); //Will perform a reboot
        }
    }
}

static lv_obj_t * add_back(lv_event_cb_t event_cb)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
    lv_theme_apply(btn, REFLOW_OVEN_THEME_BTN_BACK);
    lv_obj_set_size(btn, 60, 60);
    lv_obj_set_pos(btn, 40, 20);
    lv_obj_set_event_cb(btn, event_cb);

    return btn;
}

static lv_obj_t * add_loader(void (*end_cb)(lv_anim_t *))
{
    lv_obj_t * arc = lv_arc_create(lv_scr_act(), NULL);
    lv_arc_set_bg_angles(arc, 0, 0);
    lv_arc_set_start_angle(arc, 270);
    lv_obj_set_size(arc, 180, 180);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, loader_anim_cb);
    lv_anim_set_ready_cb(&a, end_cb);
    lv_anim_set_values(&a, 0, 110);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_var(&a, arc);
    lv_anim_start(&a);

    return arc;
}

static void loader_anim_cb(void * arc, lv_anim_value_t v)
{
    if(v > 100) v = 100;
    lv_arc_set_end_angle(arc, v * 360 / 100 + 270);

    static char buf[32];
    lv_snprintf(buf, sizeof(buf), "%d %%", v);
    lv_obj_set_style_local_value_str(arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, buf);
}

LV_EVENT_CB_DECLARE(icon_generic_event_cb)
{
    if (e == LV_EVENT_FOCUSED) {
        lv_obj_t * cont = lv_obj_get_parent(obj);
        lv_obj_t * label = lv_obj_get_child(cont, NULL);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_time(&a, 100);
        lv_anim_set_var(&a, label);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_coord_t initial_y = lv_obj_get_user_data(label);
        lv_anim_set_values(&a, initial_y, initial_y-5);
        lv_anim_start(&a);
        lv_anim_t a1;
        lv_anim_init(&a1);
        lv_anim_set_time(&a1, 100);
        lv_anim_set_var(&a1, label);
        lv_anim_set_exec_cb(&a1, (lv_anim_exec_xcb_t)lv_img_set_zoom);
        lv_anim_set_values(&a1, lv_img_get_zoom(label), 225);
        lv_anim_start(&a1);
    }
    else if(e == LV_EVENT_DEFOCUSED) {
        lv_obj_t * cont = lv_obj_get_parent(obj);
        lv_obj_t * label = lv_obj_get_child(cont, NULL);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_delay(&a, 50);
        lv_anim_set_time(&a, 100);
        lv_anim_set_var(&a, label);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_coord_t initial_y = lv_obj_get_user_data(label);
        lv_anim_set_values(&a, initial_y-5, initial_y);
        lv_anim_start(&a);
        lv_anim_t a1;
        lv_anim_init(&a1);
        lv_anim_set_time(&a1, 100);
        lv_anim_set_var(&a1, label);
        lv_anim_set_exec_cb(&a1, (lv_anim_exec_xcb_t)lv_img_set_zoom);
        lv_anim_set_values(&a1, lv_img_get_zoom(label), 256);
        lv_anim_start(&a1);
    }
}

static void reflow_oven_anim_out(lv_obj_t * obj, uint32_t delay) {
    lv_obj_t * child = lv_obj_get_child_back(obj, NULL);
    while(child) {
        if(child != bg_circle && child != bg_left_status_icons && child != bg_right_status_icons && child != lv_scr_act()) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, child);
            lv_anim_set_time(&a, REFLOW_OVEN_ANIM_TIME);
            lv_anim_set_delay(&a, delay);
            lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
            lv_anim_start(&a);

            lv_obj_fade_out(child, REFLOW_OVEN_ANIM_TIME - 70, delay + 70);
        }
        child = lv_obj_get_child_back(obj, child);
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, REFLOW_OVEN_ANIM_TIME);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
    lv_anim_start(&a);
    lv_obj_fade_out(obj, REFLOW_OVEN_ANIM_TIME - 70, delay + 70);
}

static void reflow_oven_anim_out_all(lv_obj_t * obj, uint32_t delay) {
    lv_obj_t * child = lv_obj_get_child_back(obj, NULL);
    while(child) {
        if(child != bg_circle && child != bg_left_status_icons && child != bg_right_status_icons && child != lv_scr_act()) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, child);
            lv_anim_set_time(&a, REFLOW_OVEN_ANIM_TIME);
            lv_anim_set_delay(&a, delay);

            lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
            lv_anim_start(&a);

            lv_obj_fade_out(child, REFLOW_OVEN_ANIM_TIME - 70, delay + 70);
        }
        child = lv_obj_get_child_back(obj, child);
    }
}

static void reflow_oven_anim_in(lv_obj_t * obj, uint32_t delay)
{
    if (obj != bg_circle && obj != bg_left_status_icons && obj != bg_right_status_icons && obj != lv_scr_act()) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_time(&a, REFLOW_OVEN_ANIM_TIME);
        lv_anim_set_delay(&a, delay);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(obj) - REFLOW_OVEN_ANIM_Y,
                lv_obj_get_y(obj));
        lv_anim_start(&a);

        lv_obj_fade_in(obj, REFLOW_OVEN_ANIM_TIME - 50, delay);
    }
}
