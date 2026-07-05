/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
extern lv_indev_t * indev_keypad;
lv_group_t *group_key = NULL;

/**********************
 *  STATIC VARIABLES
 **********************/

void lable_event_cb(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t       *obj   = (lv_obj_t *)lv_event_get_user_data(e);

    /* 处理事件 */
    switch (event)
    {
        default:
            break;

        /* 获得焦点事件 */
        case LV_EVENT_FOCUSED:
            lv_label_set_text(obj, "ON");
            break;

        /* 失去焦点事件 */
        case LV_EVENT_DEFOCUSED:
            lv_label_set_text(obj, "OFF");
            break;
    }
}


void spinbox_event_cb(lv_event_t * e)
{
    static uint8_t  pos   = 0;
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t       *obj   = (lv_obj_t *)lv_event_get_user_data(e);
    uint32_t        key   = lv_indev_get_key(indev_keypad);

    /* 处理事件 */
    switch (event)
    {
        default:
            break;

        /* 按键事件 */
        case LV_EVENT_KEY:
            /* 按键处理 */
            switch (key)
            {
                default:
                    break;

                /* LEFT */
                case LV_KEY_LEFT:
                    /* 光标循环 */
                    if (3 == ++pos)
                    {
                        pos = 0;
                        /* 光标-位置-改为0 */
                        lv_spinbox_set_cursor_pos(obj, 0);
                    }
                    break;
            }
            break;

        /* 获得焦点事件 */
        case LV_EVENT_FOCUSED:
            /* 光标-位置-改为0 */
            lv_spinbox_set_cursor_pos(obj, 0);
            /* 光标-背景-改为黑色 */
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x000000),
                            LV_PART_CURSOR | LV_STATE_DEFAULT);
            /* 光标-文本-改为白色 */
            lv_obj_set_style_text_color(obj, lv_color_hex(0xFFFFFF),
                            LV_PART_CURSOR | LV_STATE_DEFAULT);
            break;

        /* 失去焦点事件 */
        case LV_EVENT_DEFOCUSED:
            pos = 0;
            /* 光标-背景-改为白色 */
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFFFF),
                            LV_PART_CURSOR | LV_STATE_DEFAULT);
            /* 光标-文本-改为黑色 */
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000),
                            LV_PART_CURSOR | LV_STATE_DEFAULT);
            break;
    }
}

/**
 * Create a demo application
 */

void custom_init(gg_ui_t *ui)
{
    /* Add your codes here */

    group_key = lv_group_create();

    // lv_obj_set_style_pad_all(ui->screen_main.spinbox_on_time,  0, 0);
    // lv_obj_set_style_pad_all(ui->screen_main.spinbox_off_time, 0, 0);

    lv_indev_set_group(indev_keypad, group_key);	//将组绑定到输入设备
    lv_group_set_editing(group_key, false);     //导航模式

    // lv_group_add_obj(group_key ,ui->screen_main.label_state);
    lv_group_add_obj(group_key ,ui->screen_main.spinbox_on_time);
    lv_group_add_obj(group_key ,ui->screen_main.spinbox_off_time);

    lv_obj_add_event_cb(ui->screen_main.label_state,      lable_event_cb,   LV_EVENT_FOCUSED,   ui->screen_main.label_state);
    lv_obj_add_event_cb(ui->screen_main.label_state,      lable_event_cb,   LV_EVENT_DEFOCUSED, ui->screen_main.label_state);
    lv_obj_add_event_cb(ui->screen_main.spinbox_on_time,  spinbox_event_cb, LV_EVENT_KEY,       ui->screen_main.spinbox_on_time);
    lv_obj_add_event_cb(ui->screen_main.spinbox_on_time,  spinbox_event_cb, LV_EVENT_FOCUSED,   ui->screen_main.spinbox_on_time);
    lv_obj_add_event_cb(ui->screen_main.spinbox_on_time,  spinbox_event_cb, LV_EVENT_DEFOCUSED, ui->screen_main.spinbox_on_time);
    lv_obj_add_event_cb(ui->screen_main.spinbox_off_time, spinbox_event_cb, LV_EVENT_KEY,       ui->screen_main.spinbox_off_time);
    lv_obj_add_event_cb(ui->screen_main.spinbox_off_time, spinbox_event_cb, LV_EVENT_FOCUSED,   ui->screen_main.spinbox_off_time);
    lv_obj_add_event_cb(ui->screen_main.spinbox_off_time, spinbox_event_cb, LV_EVENT_DEFOCUSED, ui->screen_main.spinbox_off_time);
}

