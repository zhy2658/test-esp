#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "esp_err.h" 

//按键回调函数
typedef void(*button_press_cb_t)(int gpio);

//获取gpio电平的操作回调函数
typedef int(*button_getlevel_cb_t)(int gpio);

typedef struct {
    int gpio_num;           //GPIO编号
    int active_level;       //按下的电平 
    int long_press_time;    //长按的时间(ms)
    button_press_cb_t short_press_cb; //短按回调函数
    button_press_cb_t long_press_cb; //长按回调函数
    button_getlevel_cb_t getlevel_cb; //获取电平操作
}button_config_t;

typedef enum{
    BUTTON_RELEASE,  //按键松开
    BUTTON_PRESS,    //消抖状态（按键按下）
    BUTTON_HOLD,     //按键按住状态
    BUTTON_LONG_PRESS_HOLD   //等待松手
}BUTTON_STATE;

typedef struct Button_info{
    button_config_t btn_cfg;   //按键配置
    BUTTON_STATE state;        //当前按键状态
    int press_cnt;             //计数器
    struct Button_info *next;  //下一个按键参数
}button_info_t;

esp_err_t button_event_set(button_config_t* cfg);


#endif
