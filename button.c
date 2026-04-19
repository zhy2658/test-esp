 #include "button.h"
 #include "esp_log.h"
 #include <string.h>
 #include "esp_timer.h"

#define TAG "button"

static button_info_t  *button_head = NULL; // 链表头，指向第一个按键信息节点。
static esp_timer_handle_t  button_timer_handle; //ESP32 定时器句柄，用于周期性扫描按键
static bool timer_running = false; // 标志定时器是否已启动（避免重复创建）

static void button_handle(void* arg);

esp_err_t button_event_set(button_config_t* cfg){
    button_info_t* btn = (button_info_t*)malloc(sizeof(button_info_t));
    if(!btn)
        return ESP_FAIL;
    memset(btn,0,sizeof(button_info_t));
    memcpy(&btn->btn_cfg,cfg,sizeof(button_config_t));
    if(!button_head){
        button_head=btn;
    }
    else{
        button_info_t* info = button_head;
        while (info->next)
            info = info->next;
        info->next = btn;
    }

    if(!timer_running){
        static int button_interval = 5;
        esp_timer_create_args_t button_timer={
            .callback = button_handle,
            .name = "button",
            .dispatch_method = ESP_TIMER_TASK,
            .arg = (void*)button_interval,
        };
        esp_timer_create(&button_timer,&button_timer_handle);
        esp_timer_start_periodic(button_timer_handle,5000);
        timer_running = true;
    }
    return ESP_OK;
    
}

static void button_handle(void* arg){

}
