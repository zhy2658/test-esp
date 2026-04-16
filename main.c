#include <stdio.h>
#include "xl9555.h"
#include "esp_log.h"

#define TAG "main"

void xl9555_callback(uint16_t pin,int level){
    switch (pin){
        case IO0_1:
            ESP_LOGI(TAG,"Button 1 check,level:%d",level);
            break;
        case IO0_2:
            ESP_LOGI(TAG,"Button 2 check,level:%d",level);
            break;
        case IO0_3:
            ESP_LOGI(TAG,"Button 3 check,level:%d",level);
            break;
        case IO0_4:
            ESP_LOGI(TAG,"Button 4 check,level:%d",level);
            break;
        default:break;
    }
}

void app_main(void){
    xl9555_init(GPIO_NUM_10,GPIO_NUM_11,GPIO_NUM_17,xl9555_callback);
    xl9555_ioconfig(0xFFFF);

}
