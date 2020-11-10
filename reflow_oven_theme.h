/**
 * @file reflow_oven_theme.h
 *
 */

#ifndef REFLOW_OVEN_THEME_H
#define REFLOW_OVEN_THEME_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "reflow_oven_ui.h"
#include "lvgl.h"
/*********************
 *      DEFINES
 *********************/
/*Colors*/
#define REFLOW_OVEN_LIGHT           lv_color_hex(0xf3f8fe)
#define REFLOW_OVEN_GRAY            lv_color_hex(0x7f8c8d)
#define REFLOW_OVEN_LIGHT_GRAY      lv_color_hex(0xbdc3c7)
#define REFLOW_OVEN_LIGHT_BLUE      lv_color_hex(0x3498db)
#define REFLOW_OVEN_BLUE            lv_color_hex(0x2c3e50)
#define REFLOW_OVEN_PURPLE          lv_color_hex(0x8e44ad)
#define REFLOW_OVEN_LIGHT_GREEN     lv_color_hex(0x2ecc71)
#define REFLOW_OVEN_GREEN           lv_color_hex(0x4cb242)
#define REFLOW_OVEN_LIGHT_ORANGE    lv_color_hex(0xe67e22)
#define REFLOW_OVEN_LIGHT_RED       lv_color_hex(0xe74c3c)
#define REFLOW_OVEN_RED             lv_color_hex(0xd51732)
#define REFLOW_OVEN_YELLOW          lv_color_hex(0xf0f000)

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    REFLOW_OVEN_THEME_TITLE = _LV_THEME_BUILTIN_LAST,
    REFLOW_OVEN_THEME_TITLE_WHITE,
    REFLOW_OVEN_THEME_LABEL_WHITE,
    REFLOW_OVEN_THEME_ICON,
    REFLOW_OVEN_THEME_ICON_LABEL,
    REFLOW_OVEN_THEME_CHART,
    REFLOW_OVEN_THEME_BTN_BORDER,
    REFLOW_OVEN_THEME_BTN_NEUTRAL,
    REFLOW_OVEN_THEME_BTN_NEGATIVE,
    REFLOW_OVEN_THEME_BTN_POSITIVE,
    REFLOW_OVEN_THEME_BOX_BORDER,
    REFLOW_OVEN_THEME_BTN_BACK
}reflow_oven_theme_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * Initialize the default
 * @param color_primary the primary color of the theme
 * @param color_secondary the secondary color for the theme
 * @param flags ORed flags starting with `LV_THEME_DEF_FLAG_...`
 * @param font_small pointer to a small font
 * @param font_normal pointer to a normal font
 * @param font_subtitle pointer to a large font
 * @param font_title pointer to a extra large font
 * @return a pointer to reference this theme later
 */
lv_theme_t * reflow_oven_theme_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
                                    const lv_font_t * font_small, const lv_font_t * font_normal, const lv_font_t * font_subtitle,
                                    const lv_font_t * font_title);
/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
