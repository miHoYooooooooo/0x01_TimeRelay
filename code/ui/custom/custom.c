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
#include "Mcu_Param.h"
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
/* 键盘 */
extern lv_indev_t *indev_keypad;
/* 组 */
static lv_group_t *group_main          = NULL;
/* 定时器 */
static lv_timer_t *timer_loop          = NULL;      /* 循环执行 */
static lv_timer_t *timer_spinbox_blink = NULL;      /* Spinbox文字闪烁 */
static lv_timer_t *timer_roller_blink  = NULL;      /* Roller 文字闪烁 */
static lv_timer_t *timer_edit_timeout  = NULL;      /* 编辑超时 */
/* 动画 */
static lv_anim_t   anim_arc_edit       = {0};       /* 编辑圆弧动画 */

/**********************
 *      FUNCTION
 **********************/
/**
  * @brief 进入编辑模式
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
static void edit_mode_enter(void)
{
    /* 同步编辑模式状态 */
    State_SetEdit(true);

    /* 手动触发首个对象的焦点事件 */
    lv_group_focus_obj(lv_group_get_obj_by_index(group_main, 0));

    /* 恢复定时器（启动编辑模式超时退出） */
    lv_timer_reset (timer_edit_timeout);
    lv_timer_resume(timer_edit_timeout);

    /* 跟随编辑模式自动退出定时器来显示剩余操作时间 */
    lv_arc_set_max_value(guider_ui.screen_main.arc_edit, MCU_PARAM_KEY_EDIT_QUIT_TIME);
    lv_arc_set_value    (guider_ui.screen_main.arc_edit, MCU_PARAM_KEY_EDIT_QUIT_TIME);
    lv_anim_set_values  (&anim_arc_edit, MCU_PARAM_KEY_EDIT_QUIT_TIME, 0);
    lv_anim_set_duration(&anim_arc_edit, MCU_PARAM_KEY_EDIT_QUIT_TIME);
    lv_anim_start       (&anim_arc_edit);
}

/**
  * @brief 退出编辑模式
  * @param[in]   bAuto  自动退出标志位
  * @param[out]  void
  * @return void
*/
static void edit_mode_exit(bool bAuto)
{
    PARAM_OBJECT *pParamObj = Param_GetObj();

    /* 同步编辑模式状态 */
    State_SetEdit(false);

    /* 防止没松开按键就又进入编辑模式了 */
    Key_ClearType();

    /* 手动触发当前对象的失去焦点事件 */
    lv_obj_send_event(lv_group_get_focused(group_main), LV_EVENT_DEFOCUSED, lv_group_get_focused(group_main));

    /* 暂停编辑模式超时退出定时器 */
    lv_timer_pause(timer_edit_timeout);

    /* 退出分两种：一种是超时退出，一种是手动退出 */
    /* 手动退出需要同步圆弧进度 */
    if (false == bAuto)
    {
        lv_anim_set_values  (&anim_arc_edit, lv_arc_get_value(guider_ui.screen_main.arc_edit), 0);      /* 这里必须重新设置开始值，否则重置动画时会变为满量程 */
        lv_anim_set_duration(&anim_arc_edit, 100);
    }

    /* 获取参数 */
    pParamObj->relayOnObj.time  = lv_spinbox_get_value(guider_ui.screen_main.spinbox_time_on);
    pParamObj->relayOffObj.time = lv_spinbox_get_value(guider_ui.screen_main.spinbox_time_off);
    pParamObj->relayOnObj.unit  = (RELAY_UNIT)lv_roller_get_selected(guider_ui.screen_main.roller_time_on_unit);
    pParamObj->relayOffObj.unit = (RELAY_UNIT)lv_roller_get_selected(guider_ui.screen_main.roller_time_off_unit);

    /* 更新参数 */
    Param_Update();
}

/**
  * @brief 编辑模式超时定时器回调
  * @param[in]   t      定时器指针
  * @param[out]  void
  * @return void
*/
static void edit_timeout_timer_cb(lv_timer_t * t)
{
    /* 超时自动退出编辑模式 */
    edit_mode_exit(true);
}

/**
  * @brief 循环处理回调
  * @param[in]   t      定时器指针
  * @param[out]  void
  * @return void
*/
static void loop_timer_cb(lv_timer_t * t)
{
    static bool last_run  = false;                  /* 上次运行状态 */
    KEY_TYPE    key_type  = Key_GetType();          /* 当前按键类型 */
    lv_obj_t    *obj      = NULL;                   /* 当前对象 */

    /* 区分按键类型 */
    switch (key_type)
    {
        default:
            break;

        /* NULL */
        case KEY_TYPE_NULL:
            /* 非编辑模式 */
            if (false == State_GetEdit())
            {
                /* 如果当前编辑进度圆弧有值，则代表还没有进入编辑模式就松开了 */
                if (0 != lv_arc_get_value(guider_ui.screen_main.arc_edit))
                {
                    /* 开启进度倒退动画 */
                    lv_anim_set_values  (&anim_arc_edit, lv_arc_get_value(guider_ui.screen_main.arc_edit), 0);
                    lv_anim_set_duration(&anim_arc_edit, 100);
                    lv_anim_start(&anim_arc_edit);
                }
            }
            break;

        /* ENTER */
        case KEY_TYPE_ENTER:
            /* 编辑模式更改逻辑 */

            /* 不处于编辑模式 */
            if (false == State_GetEdit())
            {
                /* 是否持续按下超过目标时间 */
                if (MCU_PARAM_KEY_EDIT_ENTER_TIME <= Key_GetPressedTime())
                {
                    /* 进入编辑模式 */
                    edit_mode_enter();
                }
                else
                {
                    /* 同步进入编辑模式进度值 */
                    lv_arc_set_max_value(guider_ui.screen_main.arc_edit, MCU_PARAM_KEY_EDIT_ENTER_TIME);
                    lv_arc_set_value(guider_ui.screen_main.arc_edit, Key_GetPressedTime());
                }
            }
            break;

        /* UP & DOWN */
        case KEY_TYPE_UP:
        case KEY_TYPE_DOWN:
            /* 自适应步进逻辑 */

            /* 必须处于编辑模式 */
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

            /* 复位编辑模式超时退出定时器 */
            lv_timer_reset(timer_edit_timeout);
            break;
    }

    /**************** 运行状态 ****************/

    /* 是否发生变化 */
    if (last_run != State_GetRun())
    {
        /* 设置文本 */
        lv_label_set_text(guider_ui.screen_main.label_state, (State_GetRun() ? "RUN" : "STOP"));
        /* 设置背景颜色 */
        lv_obj_set_style_bg_color(guider_ui.screen_main.label_state, (State_GetRun() ? lv_color_hex(0x00FF00) : lv_color_hex(0xFF0000)), LV_PART_MAIN);

        /* 更新运行状态 */
        last_run = State_GetRun();
    }
}

/**
  * @brief 重置动画
  * @param[in]   a      动画对象
  * @param[out]  void
  * @return void
*/
void anim_restart(lv_anim_t * a)
{
    lv_anim_t anim = *a;

    /* 删除 */
    lv_anim_delete(a, NULL);

    /* 重新初始化 */
    lv_anim_init        (a);
    lv_anim_set_var     (a, anim.var);
    lv_anim_set_exec_cb (a, anim.exec_cb);
    lv_anim_set_values  (a, anim.start_value, anim.end_value);
    lv_anim_set_duration(a, anim.duration);
    lv_anim_set_repeat_count(a, anim.repeat_cnt);
    lv_anim_set_repeat_delay(a, anim.repeat_delay);
    lv_anim_start(a);
}

/**
  * @brief Arc动画回调
  * @param[in]   obj    对象
  * @param[in]   v      值
  * @param[out]  void
  * @return void
*/
static void arc_anim_cb(void * obj, int32_t v)
{
    if (obj == guider_ui.screen_main.arc_edit)
    {
        /* 设置数值 */
        lv_arc_set_value(obj, v);
    }
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

                        /* 切换到下一个对象 */
                        lv_group_focus_next(group_main);
                        break;
                }

                /* 重置动画 */
                anim_restart(&anim_arc_edit);

                /* 复位编辑模式超时退出定时器 */
                lv_timer_reset(timer_edit_timeout);
            }
            break;

        /* 获得焦点事件 */
        case LV_EVENT_FOCUSED:
            /* 非编辑模式不处理 */
            if (true == State_GetEdit())
            {
                /* 更新定时器（闪烁）的用户数据 */
                lv_timer_set_user_data(timer_spinbox_blink, obj);
                /* 恢复定时器（闪烁） */
                lv_timer_reset (timer_spinbox_blink);
                lv_timer_resume(timer_spinbox_blink);
                /* 强制隐藏文字 */
                lv_obj_set_style_text_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
            }
            break;

        /* 失去焦点事件 */
        case LV_EVENT_DEFOCUSED:
            /* 暂停并复位定时器（闪烁） */
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
                            /* 所有对象设置完毕，手动退出编辑模式 */
                            edit_mode_exit(false);
                        }
                        else
                        {
                            /* 切换到下一个对象 */
                            lv_group_focus_next(group_main);
                        }
                        break;
                }

                /* 重置动画 */
                anim_restart(&anim_arc_edit);

                /* 复位编辑模式超时退出定时器 */
                lv_timer_reset(timer_edit_timeout);
            }
            break;

        /* 获得焦点事件 */
        case LV_EVENT_FOCUSED:
            /* 非编辑模式不处理 */
            if (true == State_GetEdit())
            {
                /* 更新定时器（闪烁）的用户数据 */
                lv_timer_set_user_data(timer_roller_blink, obj);
                /* 恢复定时器（闪烁） */
                lv_timer_reset (timer_roller_blink);
                lv_timer_resume(timer_roller_blink);
                /* 强制隐藏文字 */
                lv_obj_set_style_text_opa(obj, LV_OPA_TRANSP, LV_PART_SELECTED);
            }
            break;

        /* 失去焦点事件 */
        case LV_EVENT_DEFOCUSED:
            /* 暂停并复位定时器（闪烁） */
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
    PARAM_OBJECT *pParamObj = Param_GetObj();

    /* 创建变量 */
    group_main          = lv_group_create();
    timer_loop          = lv_timer_create(loop_timer_cb,          20, NULL);
    timer_spinbox_blink = lv_timer_create(spinbox_timer_cb,      500, NULL);
    timer_roller_blink  = lv_timer_create(roller_timer_cb,       500, NULL);
    timer_edit_timeout  = lv_timer_create(edit_timeout_timer_cb, MCU_PARAM_KEY_EDIT_QUIT_TIME, NULL);

    if ((NULL == ui                 )
     || (NULL == group_main         )
     || (NULL == timer_loop         )
     || (NULL == timer_spinbox_blink)
     || (NULL == timer_roller_blink )
     || (NULL == timer_edit_timeout ))
    {
        return;
    }

    /* 暂停并复位定时器 */
    lv_timer_pause(timer_spinbox_blink);
    lv_timer_pause(timer_roller_blink);
    lv_timer_pause(timer_edit_timeout);

    /* 将组绑定到输入设备 */
    lv_indev_set_group(indev_keypad, group_main);

    /* 将对象绑定到组 */
    lv_group_add_obj(group_main, guider_ui.screen_main.spinbox_time_on);
    lv_group_add_obj(group_main, guider_ui.screen_main.roller_time_on_unit);
    lv_group_add_obj(group_main, guider_ui.screen_main.spinbox_time_off);
    lv_group_add_obj(group_main, guider_ui.screen_main.roller_time_off_unit);

    /* 动画 */
    lv_anim_init        (&anim_arc_edit);
    lv_anim_set_var     (&anim_arc_edit, guider_ui.screen_main.arc_edit);
    lv_anim_set_exec_cb (&anim_arc_edit, arc_anim_cb);
    lv_anim_set_duration(&anim_arc_edit, 100);
    lv_anim_set_repeat_count(&anim_arc_edit, 0);    /* 不重复 */
    lv_anim_set_repeat_delay(&anim_arc_edit, 0);

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

    /* 更新参数 */
    lv_spinbox_set_value  (guider_ui.screen_main.spinbox_time_on,      pParamObj->relayOnObj.time);
    lv_spinbox_set_value  (guider_ui.screen_main.spinbox_time_off,     pParamObj->relayOffObj.time);
    lv_roller_set_selected(guider_ui.screen_main.roller_time_on_unit,  pParamObj->relayOnObj.unit,  LV_ANIM_OFF);
    lv_roller_set_selected(guider_ui.screen_main.roller_time_off_unit, pParamObj->relayOffObj.unit, LV_ANIM_OFF);
}
