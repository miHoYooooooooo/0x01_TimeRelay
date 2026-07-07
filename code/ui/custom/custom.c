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

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_timer_t *timer_spinbox_blink = NULL;
static lv_timer_t *timer_roller_blink  = NULL;

void lable_event_cb(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t       *obj   = (lv_obj_t *)lv_event_get_user_data(e);

    /* 处理事件 */
    switch (event)
    {
        default:
            break;

        case LV_EVENT_KEY:
            /* 按键处理 */
            switch (lv_indev_get_key(indev_keypad))
            {
                default:
                    lv_label_set_text(obj, "KEY");
                    break;
            }
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



/**
  * @brief Spinbox定时器（闪烁）回调
  * @param[in]   t      定时器指针
  * @param[out]  void
  * @return void
*/
static void spinbox_timer_blink_cb(lv_timer_t * t)
{
    lv_obj_t *obj = NULL;
    lv_opa_t  opa = LV_OPA_COVER;

    if (NULL == t)
    {
        return;
    }

    /* 获取自定义数据 */
    obj = (lv_obj_t *)lv_timer_get_user_data(t);

    if (NULL == obj)
    {
        return;
    }

    /* 获取当前不透明度 */
    opa = lv_obj_get_style_text_opa(obj, LV_PART_MAIN);

    /* 判断透明度 */
    if (LV_OPA_TRANSP == opa)
    {
        opa = LV_OPA_COVER;
    }
    else
    {
        opa = LV_OPA_TRANSP;
    }

    /* 调整文字透明度 */
    lv_obj_set_style_text_opa(obj, opa, LV_PART_MAIN);
}

/**
  * @brief Spinbox事件回调
  * @param[in]   e      事件指针
  * @param[out]  void
  * @return void
*/
static void spinbox_event_cb(lv_event_t * e)
{
    lv_event_code_t     event       = LV_EVENT_LAST;
    lv_obj_t           *obj         = NULL;
    uint32_t            key         = 0;

    if (NULL == e)
    {
        return;
    }

    /* 获取变量 */
    event = lv_event_get_code(e);
    obj   = (lv_obj_t *)lv_event_get_user_data(e);

    if (NULL == obj)
    {
        return;
    }

    /* 处理事件 */
    switch (event)
    {
        default:
            break;

        /* 按键事件 */
        case LV_EVENT_KEY:
            if (NULL != indev_keypad)
            {
                /* 获取按键 */
                key = lv_indev_get_key(indev_keypad);

                /* 按键处理 */
                switch (key)
                {
                    default:
                        break;

                    /* UP & DOWN */
                    case LV_KEY_UP:
                    case LV_KEY_DOWN:
                        /* 数字长显 */
                        lv_obj_set_style_text_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
                        /* 一直复位着定时器（闪烁） */
                        if (NULL != timer_spinbox_blink)
                        {
                            lv_timer_reset(timer_spinbox_blink);
                        }
                        break;
                }
            }
            break;

        /* 获得焦点事件 */
        case LV_EVENT_FOCUSED:
            /* 是否存在定时器（闪烁） */
            if (NULL != timer_spinbox_blink)
            {
                /* 更新定时器（闪烁）的用户数据 */
                lv_timer_set_user_data(timer_spinbox_blink, obj);
                /* 复位定时器（闪烁） */
                lv_timer_reset(timer_spinbox_blink);
                /* 恢复定时器（闪烁） */
                lv_timer_resume(timer_spinbox_blink);
                /* 强制不显示文字 */
                lv_obj_set_style_text_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
            }
            break;

        /* 失去焦点事件 */
        case LV_EVENT_DEFOCUSED:
            /* 是否存在定时器（闪烁） */
            if (NULL != timer_spinbox_blink)
            {
                /* 暂停定时器（闪烁） */
                lv_timer_pause(timer_spinbox_blink);
                /* 强制显示文字 */
                lv_obj_set_style_text_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
            }
            break;
    }
}

/**
  * @brief Roller定时器（闪烁）回调
  * @param[in]   t      定时器指针
  * @param[out]  void
  * @return void
*/
static void roller_timer_blink_cb(lv_timer_t * t)
{
    lv_obj_t *obj = NULL;
    lv_opa_t  opa = LV_OPA_COVER;

    if (NULL == t)
    {
        return;
    }

    /* 获取自定义数据 */
    obj = (lv_obj_t *)lv_timer_get_user_data(t);

    if (NULL == obj)
    {
        return;
    }

    /* 获取当前不透明度 */
    opa = lv_obj_get_style_text_opa(obj, LV_PART_SELECTED);

    /* 判断透明度 */
    if (LV_OPA_TRANSP == opa)
    {
        opa = LV_OPA_COVER;
    }
    else
    {
        opa = LV_OPA_TRANSP;
    }

    /* 调整文字透明度 */
    lv_obj_set_style_text_opa(obj, opa, LV_PART_SELECTED);
}

/**
  * @brief Roller事件回调
  * @param[in]   e      事件指针
  * @param[out]  void
  * @return void
*/
static void roller_event_cb(lv_event_t * e)
{
    lv_event_code_t     event       = LV_EVENT_LAST;
    lv_obj_t           *obj         = NULL;
    uint32_t            key         = 0;

    if (NULL == e)
    {
        return;
    }

    /* 获取变量 */
    event = lv_event_get_code(e);
    obj   = (lv_obj_t *)lv_event_get_user_data(e);

    if (NULL == obj)
    {
        return;
    }

    /* 处理事件 */
    switch (event)
    {
        default:
            break;

        /* 按键事件 */
        case LV_EVENT_KEY:
            if (NULL != indev_keypad)
            {
                /* 获取按键 */
                key = lv_indev_get_key(indev_keypad);

                /* 按键处理 */
                switch (key)
                {
                    default:
                        break;

                    /* UP & DOWN */
                    case LV_KEY_UP:
                    case LV_KEY_DOWN:
                        /* 数字长显 */
                        lv_obj_set_style_text_opa(obj, LV_OPA_COVER, LV_PART_SELECTED);
                        /* 一直复位着定时器（闪烁） */
                        if (NULL != timer_roller_blink)
                        {
                            lv_timer_reset(timer_roller_blink);
                        }
                        break;
                }
            }
            break;

        /* 获得焦点事件 */
        case LV_EVENT_FOCUSED:
            /* 是否存在定时器（闪烁） */
            if (NULL != timer_roller_blink)
            {
                /* 更新定时器（闪烁）的用户数据 */
                lv_timer_set_user_data(timer_roller_blink, obj);
                /* 复位定时器（闪烁） */
                lv_timer_reset(timer_roller_blink);
                /* 恢复定时器（闪烁） */
                lv_timer_resume(timer_roller_blink);
                /* 强制不显示文字 */
                lv_obj_set_style_text_opa(obj, LV_OPA_TRANSP, LV_PART_SELECTED);
            }
            break;

        /* 失去焦点事件 */
        case LV_EVENT_DEFOCUSED:
            /* 是否存在定时器（闪烁） */
            if (NULL != timer_roller_blink)
            {
                /* 暂停定时器（闪烁） */
                lv_timer_pause(timer_roller_blink);
                /* 强制显示文字 */
                lv_obj_set_style_text_opa(obj, LV_OPA_COVER, LV_PART_SELECTED);
            }
            break;
    }
}

/**
 * Create a demo application
 */

void custom_init(gg_ui_t *ui)
{
    lv_group_t *group_main = NULL;

    /* 创建变量 */
    group_main          = lv_group_create();
    timer_spinbox_blink = lv_timer_create(spinbox_timer_blink_cb, 500, NULL);
    timer_roller_blink  = lv_timer_create(roller_timer_blink_cb,  500, NULL);

    if ((NULL == ui)
     || (NULL == group_main)
     || (NULL == timer_spinbox_blink)
     || (NULL == timer_roller_blink))
    {
        return;
    }

    /* LED显示绿色 */
    lv_led_set_color(ui->screen_main.led_state, lv_color_hex(0x00FF00));

    /* 暂停定时器 */
    lv_timer_pause(timer_spinbox_blink);
    lv_timer_pause(timer_roller_blink);

    /* 将组绑定到输入设备 */
    lv_indev_set_group(indev_keypad, group_main);

    lv_group_add_obj(group_main ,ui->screen_main.spinbox_time_on);
    lv_group_add_obj(group_main ,ui->screen_main.roller_time_on_unit);
    lv_group_add_obj(group_main ,ui->screen_main.spinbox_time_off);
    lv_group_add_obj(group_main ,ui->screen_main.roller_time_off_unit);
    lv_group_add_obj(group_main ,ui->screen_main.button_run);

    lv_obj_add_event_cb(ui->screen_main.spinbox_time_on,  spinbox_event_cb, LV_EVENT_KEY,       ui->screen_main.spinbox_time_on);
    lv_obj_add_event_cb(ui->screen_main.spinbox_time_on,  spinbox_event_cb, LV_EVENT_FOCUSED,   ui->screen_main.spinbox_time_on);
    lv_obj_add_event_cb(ui->screen_main.spinbox_time_on,  spinbox_event_cb, LV_EVENT_DEFOCUSED, ui->screen_main.spinbox_time_on);
    lv_obj_add_event_cb(ui->screen_main.spinbox_time_off, spinbox_event_cb, LV_EVENT_KEY,       ui->screen_main.spinbox_time_off);
    lv_obj_add_event_cb(ui->screen_main.spinbox_time_off, spinbox_event_cb, LV_EVENT_FOCUSED,   ui->screen_main.spinbox_time_off);
    lv_obj_add_event_cb(ui->screen_main.spinbox_time_off, spinbox_event_cb, LV_EVENT_DEFOCUSED, ui->screen_main.spinbox_time_off);

    lv_obj_add_event_cb(ui->screen_main.roller_time_on_unit,  roller_event_cb, LV_EVENT_KEY,       ui->screen_main.roller_time_on_unit);
    lv_obj_add_event_cb(ui->screen_main.roller_time_on_unit,  roller_event_cb, LV_EVENT_FOCUSED,   ui->screen_main.roller_time_on_unit);
    lv_obj_add_event_cb(ui->screen_main.roller_time_on_unit,  roller_event_cb, LV_EVENT_DEFOCUSED, ui->screen_main.roller_time_on_unit);
    lv_obj_add_event_cb(ui->screen_main.roller_time_off_unit, roller_event_cb, LV_EVENT_KEY,       ui->screen_main.roller_time_off_unit);
    lv_obj_add_event_cb(ui->screen_main.roller_time_off_unit, roller_event_cb, LV_EVENT_FOCUSED,   ui->screen_main.roller_time_off_unit);
    lv_obj_add_event_cb(ui->screen_main.roller_time_off_unit, roller_event_cb, LV_EVENT_DEFOCUSED, ui->screen_main.roller_time_off_unit);

    /* 手动触发一下 */
    lv_group_focus_obj(ui->screen_main.spinbox_time_on);
}

