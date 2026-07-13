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
#include "Bsp_Key.h"
#include "Mcu_State.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/


/**********************
 *  STATIC VARIABLES
 **********************/
extern lv_indev_t *indev_keypad;
/* 组 */
static lv_group_t *group_main          = NULL;
/* 定时器 */
static lv_timer_t *timer_loop          = NULL;      /* 循环执行 */
static lv_timer_t *timer_spinbox_blink = NULL;      /* Spinbox文字闪烁 */
static lv_timer_t *timer_roller_blink  = NULL;      /* Roller 文字闪烁 */

/**********************
 *      FUNCTION
 **********************/
/**
  * @brief 循环处理回调
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
static void loop_timer_cb(lv_timer_t * t)
{
    static bool     last_edit      = false;                 /* 上次编辑状态 */
    static KEY_TYPE key_type_last  = KEY_TYPE_NULL;         /* 上次按键类型 */
    KEY_TYPE        key_type       = Key_GetType();         /* 当前按键类型 */
    lv_obj_t       *obj            = NULL;                  /* 当前对象 */

    /* 区分按键类型 */
    switch (key_type)
    {
        default:
            break;

        /* ENTER */
        case KEY_TYPE_ENTER:
            /* 编辑模式逻辑 */
            if (false == State_GetEdit())
            {
                if (3000 <= Key_GetPressedTime())
                {
                    State_SetEdit(true);
                }
            }

            /* 是否变更状态了 */
            if (last_edit != State_GetEdit())
            {
                /* 正在编辑 */
                if (true == State_GetEdit())
                {
                    /* 手动触发首个对象的焦点事件 */
                    lv_group_focus_obj(lv_group_get_obj_by_index(group_main, 0));
                }
                /* 退出编辑 */
                else
                {
                    /* 手动触发当前对象的失去焦点事件 */
                    lv_obj_send_event(lv_group_get_focused(group_main), LV_EVENT_DEFOCUSED, lv_group_get_focused(group_main));
                }
            }

            /* 更新状态 */
            last_edit = State_GetEdit();
            break;

        /* UP & DOWN */
        case KEY_TYPE_UP:
        case KEY_TYPE_DOWN:
            /* 自适应步进逻辑 */
            if (true == State_GetEdit())
            {
                /* 获取聚焦对象 */
                obj = lv_group_get_focused(group_main);

                /* 对象为Spinbox */
                if (true == lv_obj_check_type(obj, &lv_spinbox_class))
                {
                    /* 只有持续按住才允许进位 */
                    if (true == Key_IsPressing())
                    {
                        /* 一直复位着定时器（闪烁） */
                        lv_timer_reset(timer_spinbox_blink);

                        /* 长按判断 */
                        if (3000 <= Key_GetPressedTime())
                        {
                            /* 清空按下的时间 */
                            Key_ClearPressedTime();

                            /* 进位 */
                            lv_spinbox_step_prev(obj);

                            /* 根据进位调整长按间隔时间 */
                            if (10 == lv_spinbox_get_step(obj))
                            {
                                lv_indev_set_long_press_repeat_time(indev_keypad, 300);
                            }
                            else
                            {
                                lv_indev_set_long_press_repeat_time(indev_keypad, 500);
                            }
                        }
                    }
                    /* 松开了 */
                    else
                    {
                        /* 复原光标位置 */
                        lv_spinbox_set_cursor_pos(obj, 0);
                        /* 复原长按间隔 */
                        lv_indev_set_long_press_repeat_time(indev_keypad, 100);
                    }
                }
            }
            break;
    }

    /* 保存按键类型 */
    key_type_last = key_type;
}

/**
  * @brief Spinbox定时器回调
  * @param[in]   t      定时器指针
  * @param[out]  void
  * @return void
*/
static void spinbox_timer_cb(lv_timer_t * t)
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
            /* 非编辑模式不处理 */
            if (true == State_GetEdit())
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
                        lv_timer_reset(timer_spinbox_blink);
                        break;

                    /* ENTER */
                    case LV_KEY_ENTER:
                        /* 复原光标位置 */
                        lv_spinbox_set_cursor_pos(obj, 0);
                        /* 复原长按间隔 */
                        lv_indev_set_long_press_repeat_time(indev_keypad, 100);
                        /* 复用为切换对象 */
                        lv_group_focus_next(group_main);
                        break;
                }
            }
            break;

        /* 获得焦点事件 */
        case LV_EVENT_FOCUSED:
            /* 非编辑模式不处理 */
            if (true == State_GetEdit())
            {
                /* 更新定时器（闪烁）的用户数据 */
                lv_timer_set_user_data(timer_spinbox_blink, obj);
                /* 复位定时器（闪烁） */
                lv_timer_reset(timer_spinbox_blink);
                /* 恢复定时器（闪烁） */
                lv_timer_resume(timer_spinbox_blink);
                /* 强制隐藏文字 */
                lv_obj_set_style_text_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
            }
            break;

        /* 失去焦点事件 */
        case LV_EVENT_DEFOCUSED:
            /* 暂停定时器（闪烁） */
            lv_timer_pause(timer_spinbox_blink);
            /* 强制显示文字 */
            lv_obj_set_style_text_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
            break;
    }
}

/**
  * @brief Roller定时器回调
  * @param[in]   t      定时器指针
  * @param[out]  void
  * @return void
*/
static void roller_timer_cb(lv_timer_t * t)
{
    lv_obj_t *obj = NULL;
    lv_opa_t  opa = LV_OPA_COVER;

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
            /* 非编辑模式不处理 */
            if (true == State_GetEdit())
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
                        lv_timer_reset(timer_roller_blink);
                        break;

                        /* ENTER */
                    case LV_KEY_ENTER:
                        /* 当前是最后一个对象 */
                        if ((lv_group_get_obj_by_index(group_main, (lv_group_get_obj_count(group_main) - 1))) == obj)
                        {
                            /* 所有对象设置完毕，退出编辑模式 */
                            State_SetEdit(false);
                        }
                        else
                        {
                            /* 复用为切换对象 */
                            lv_group_focus_next(group_main);
                        }
                        break;
                }
            }
            break;

        /* 获得焦点事件 */
        case LV_EVENT_FOCUSED:
            /* 非编辑模式不处理 */
            if (true == State_GetEdit())
            {
                /* 更新定时器（闪烁）的用户数据 */
                lv_timer_set_user_data(timer_roller_blink, obj);
                /* 复位定时器（闪烁） */
                lv_timer_reset(timer_roller_blink);
                /* 恢复定时器（闪烁） */
                lv_timer_resume(timer_roller_blink);
                /* 强制隐藏文字 */
                lv_obj_set_style_text_opa(obj, LV_OPA_TRANSP, LV_PART_SELECTED);
            }
            break;

        /* 失去焦点事件 */
        case LV_EVENT_DEFOCUSED:
            /* 暂停定时器（闪烁） */
            lv_timer_pause(timer_roller_blink);
            /* 强制显示文字 */
            lv_obj_set_style_text_opa(obj, LV_OPA_COVER, LV_PART_SELECTED);
            break;
    }
}

/**
 * Create a demo application
 */

void custom_init(gg_ui_t *ui)
{
    /* 创建变量 */
    group_main          = lv_group_create();
    timer_loop          = lv_timer_create(loop_timer_cb,      20, NULL);
    timer_spinbox_blink = lv_timer_create(spinbox_timer_cb,  500, NULL);
    timer_roller_blink  = lv_timer_create(roller_timer_cb,   500, NULL);

    if ((NULL == ui                 )
     || (NULL == group_main         )
     || (NULL == timer_loop         )
     || (NULL == timer_spinbox_blink)
     || (NULL == timer_roller_blink ))
    {
        return;
    }

    /* LED显示绿色 */
    lv_led_set_color(guider_ui.screen_main.led_state, lv_color_hex(0x00FF00));

    /* 暂停定时器 */
    lv_timer_pause(timer_spinbox_blink);
    lv_timer_pause(timer_roller_blink);

    /* 将组绑定到输入设备 */
    lv_indev_set_group(indev_keypad, group_main);

    /* 将对象绑定到组 */
    lv_group_add_obj(group_main, guider_ui.screen_main.spinbox_time_on);
    lv_group_add_obj(group_main, guider_ui.screen_main.roller_time_on_unit);
    lv_group_add_obj(group_main, guider_ui.screen_main.spinbox_time_off);
    lv_group_add_obj(group_main, guider_ui.screen_main.roller_time_off_unit);

    /* Spinbox */
    lv_obj_add_event_cb(guider_ui.screen_main.spinbox_time_on,      spinbox_event_cb, LV_EVENT_KEY,           guider_ui.screen_main.spinbox_time_on);
    lv_obj_add_event_cb(guider_ui.screen_main.spinbox_time_on,      spinbox_event_cb, LV_EVENT_FOCUSED,       guider_ui.screen_main.spinbox_time_on);
    lv_obj_add_event_cb(guider_ui.screen_main.spinbox_time_on,      spinbox_event_cb, LV_EVENT_DEFOCUSED,     guider_ui.screen_main.spinbox_time_on);
    lv_obj_add_event_cb(guider_ui.screen_main.spinbox_time_off,     spinbox_event_cb, LV_EVENT_KEY,           guider_ui.screen_main.spinbox_time_off);
    lv_obj_add_event_cb(guider_ui.screen_main.spinbox_time_off,     spinbox_event_cb, LV_EVENT_FOCUSED,       guider_ui.screen_main.spinbox_time_off);
    lv_obj_add_event_cb(guider_ui.screen_main.spinbox_time_off,     spinbox_event_cb, LV_EVENT_DEFOCUSED,     guider_ui.screen_main.spinbox_time_off);
    /* Roller */
    lv_obj_add_event_cb(guider_ui.screen_main.roller_time_on_unit,  roller_event_cb,  LV_EVENT_KEY,           guider_ui.screen_main.roller_time_on_unit);
    lv_obj_add_event_cb(guider_ui.screen_main.roller_time_on_unit,  roller_event_cb,  LV_EVENT_FOCUSED,       guider_ui.screen_main.roller_time_on_unit);
    lv_obj_add_event_cb(guider_ui.screen_main.roller_time_on_unit,  roller_event_cb,  LV_EVENT_DEFOCUSED,     guider_ui.screen_main.roller_time_on_unit);
    lv_obj_add_event_cb(guider_ui.screen_main.roller_time_off_unit, roller_event_cb,  LV_EVENT_KEY,           guider_ui.screen_main.roller_time_off_unit);
    lv_obj_add_event_cb(guider_ui.screen_main.roller_time_off_unit, roller_event_cb,  LV_EVENT_FOCUSED,       guider_ui.screen_main.roller_time_off_unit);
    lv_obj_add_event_cb(guider_ui.screen_main.roller_time_off_unit, roller_event_cb,  LV_EVENT_DEFOCUSED,     guider_ui.screen_main.roller_time_off_unit);
}
