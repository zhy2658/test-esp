#ifndef _XL9555_H_
#define _XL9555_H_
#include "esp_err.h"
#include "driver/gpio.h"

//port 0
#define IO0_0   (1<<0)
#define IO0_1   (1<<1)
#define IO0_2   (1<<2)
#define IO0_3   (1<<3)
#define IO0_4   (1<<4)
#define IO0_5   (1<<5)
#define IO0_6   (1<<6)
#define IO0_7   (1<<7)

//port 1
#define IO1_0   (1<<8)
#define IO1_1   (1<<9)
#define IO1_2   (1<<10)
#define IO1_3   (1<<11)
#define IO1_4   (1<<12)
#define IO1_5   (1<<13)
#define IO1_6   (1<<14)
#define IO1_7   (1<<15)

/*
 * XL9555 command byte / register map
 * 每个地址8bit
 * | B2 | B1 | B0 | Command | Register                    | Protocol        | Power-up default |
 * |----|----|----|---------|-----------------------------|-----------------|------------------|
 * | 0  | 0  | 0  | 0x00    | Input Port 0                | Read byte       | XXXX XXXX        |
 * | 0  | 0  | 1  | 0x01    | Input Port 1                | Read byte       | XXXX XXXX        |
 * | 0  | 1  | 0  | 0x02    | Output Port 0               | Read/write byte | 1111 1111        |
 * | 0  | 1  | 1  | 0x03    | Output Port 1               | Read/write byte | 1111 1111        |
 * | 1  | 0  | 0  | 0x04    | Polarity Inversion Port 0   | Read/write byte | 0000 0000        |
 * | 1  | 0  | 1  | 0x05    | Polarity Inversion Port 1   | Read/write byte | 0000 0000        |
 * | 1  | 1  | 0  | 0x06    | Configuration Port 0        | Read/write byte | 1111 1111        |
 * | 1  | 1  | 1  | 0x07    | Configuration Port 1        | Read/write byte | 1111 1111        |
 */

// 配置存0x06 电平存0x02 

typedef void(*xl9555_input_cb_t)(uint16_t pin,int level);

// 初始化 I2C 总线并挂载 XL9555 设备。
// sda / scl: I2C 数据线和时钟线引脚。
// isr: XL9555 中断引脚；不使用中断时传入 GPIO_NUM_NC。
// f: 输入电平变化时触发的回调函数。
esp_err_t xl9555_init(gpio_num_t sda,gpio_num_t scl,gpio_num_t isr,xl9555_input_cb_t f);


//---------xl9555 pin引脚操作-----------
//读取 XL9555 某个引脚的电平状态（高电平还是低电平）
int xl9555_pin_read(uint16_t pin);

//配置pin脚高低电平，pin 新引脚 ，level电平
esp_err_t xl9555_pin_write(uint16_t pin,int level);


//-------i2c + xl9555寄存器操作---------------
//配置xl9555
esp_err_t xl9555_ioconfig(uint16_t config);

//这个是向xl9555写入2个字节数据
esp_err_t xl9555_write_word(uint8_t reg,uint16_t data);

// 写入2个字节数据
esp_err_t xl9555_read_word(uint8_t reg,uint16_t *data);



#endif
