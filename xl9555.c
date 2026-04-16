#include "xl9555.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h" 
#include "driver/i2c_master.h"
#define XL9555_ADDR 0X20

static i2c_master_bus_handle_t xl9555_bus_handle = NULL;
static i2c_master_dev_handle_t xl9555_dev_handle = NULL;

static EventGroupHandle_t xl9555_io_event = NULL;
static xl9555_input_cb_t xl9555_io_callback = NULL;

static gpio_num_t xl9555_int_io;

#define XL9555_ISR_BIT BIT0

static void IRAM_ATTR xl9555_int_handle(void* param){
    BaseType_t taskWake;
    xEventGroupSetBitsFromISR(xl9555_io_event,XL9555_ISR_BIT,&taskWake);
    portYIELD_FROM_ISR(taskWake);
}

static void xl9555_task(void* param){

    EventBits_t ev;
    uint16_t last_input = 0;
    xl9555_read_word(0x00,&last_input);
    while (1)
    {
        ev = xEventGroupWaitBits(xl9555_io_event,XL9555_ISR_BIT,pdTRUE,pdFALSE,portMAX_DELAY);
        if(ev & XL9555_ISR_BIT){
            uint16_t input = 0;
            esp_rom_delay_us(1000);
            if(gpio_get_level(xl9555_int_io) != 0){
                continue;
            }
            esp_err_t ret = xl9555_read_word(0x00,&input);
            if(ret == ESP_OK){
                for(int i=0;i < 16;i++){
                    uint8_t last_value = last_input&(1<<i)?1:0;
                    uint8_t value = input&(1<<i)?1:0;
                    if(value != last_value){
                        if (xl9555_io_callback != NULL) {
                            xl9555_io_callback((1<<i),value);
                        }
                    }
                }
                last_input = input;
            }
        }
    }
    
}

// 初始化i2c和xl9555器件的函数
esp_err_t xl9555_init(gpio_num_t sda,gpio_num_t scl,gpio_num_t isr,xl9555_input_cb_t f){
    // 初始化i2c总线
    i2c_master_bus_config_t bus_config={
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .glitch_ignore_cnt = 7,
        .i2c_port = -1
    };
    i2c_new_master_bus(&bus_config,&xl9555_bus_handle);

    i2c_device_config_t dev_config ={
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = XL9555_ADDR,
        .scl_speed_hz = 100000,

    };

    i2c_master_bus_add_device(xl9555_bus_handle,&dev_config,&xl9555_dev_handle);
    xl9555_io_callback = f;
    if(isr != GPIO_NUM_NC){
        xl9555_io_event =xEventGroupCreate();
        xl9555_int_io = isr;
        gpio_config_t int_config = {
            .intr_type = GPIO_INTR_NEGEDGE,
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pin_bit_mask = (1<<isr)
        };
        gpio_config(&int_config);
        gpio_install_isr_service(0);
        gpio_isr_handler_add(isr,xl9555_int_handle,(void*)isr);

        xTaskCreatePinnedToCore(xl9555_task,"xl9555",4096,NULL,3,NULL,1);
    }
    return ESP_OK;
}

//写入2个字节
esp_err_t xl9555_write_word(uint8_t reg,uint16_t data){
    uint8_t write_buf[3];
    write_buf[0] = reg;
    write_buf[1] = data&0xff;
    write_buf[2] = (data>>8)&0xff;
    return i2c_master_transmit(xl9555_dev_handle,write_buf,3,500);
}

// 读取2个字节
esp_err_t xl9555_read_word(uint8_t reg,uint16_t *data){
    uint8_t addr[1];
    addr[0] = reg;
    return i2c_master_transmit_receive(xl9555_dev_handle,addr,1,(uint8_t *) data ,2 ,500);
}

//配置xl9555
esp_err_t xl9555_ioconfig(uint16_t config){
    esp_err_t ret;
    do{
        ret = xl9555_write_word(0x06,config);
        vTaskDelay(pdMS_TO_TICKS(150));
    } while (ret != ESP_OK);
    return ret;
}


int xl9555_pin_read(uint16_t pin){
    uint16_t data =0;
    xl9555_read_word(0x00,&data);
    return (data&pin) ? 1: 0;
}

esp_err_t xl9555_pin_write(uint16_t pin,int level){
    uint16_t data;
    xl9555_read_word(0x02,&data);
    if(level ){
        data |=pin;
    }
    else{
        data &= ~pin;
    }
    return xl9555_write_word(0x02,data);
}
