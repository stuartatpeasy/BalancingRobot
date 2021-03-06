#include <atmel_start.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hardware/adxl345.h"
#include "hardware/motor.h"
#include "hardware/steppermotor.h"


int main(void)
{
    StepperMotor_t stepper;
    Motor_t motor;
    struct io_descriptor *usart_io, *spi_io, *i2c_io;

	atmel_start_init();         // Initialise MCU, drivers and firmware
    stepper_motor_init(&stepper, MOTORA0);
    motor_init(&motor, MOTORB0);

    usart_sync_get_io_descriptor(&USART_0, &usart_io);
    usart_sync_set_baud_rate(&USART_0, 230400);
    usart_sync_enable(&USART_0);

    gpio_set_pin_level(SPI_nSS, true);
    spi_m_sync_get_io_descriptor(&SPI_0, &spi_io);
    spi_m_sync_set_mode(&SPI_0, SPI_MODE_3);
    spi_m_sync_set_char_size(&SPI_0, SPI_CHAR_SIZE_8);
    spi_m_sync_set_data_order(&SPI_0, SPI_DATA_ORDER_MSB_1ST);
    spi_m_sync_enable(&SPI_0);

	i2c_m_sync_get_io_descriptor(&I2C_0, &i2c_io);
	i2c_m_sync_enable(&I2C_0);
	i2c_m_sync_set_slaveaddr(&I2C_0, 0x53, I2C_M_SEVEN);
	io_write(i2c_io, (uint8_t *)"Hello World!", 12);

    // TODO init ADXL345 - verify DEVID

    // Enable accelerometer
    adxl345_write_reg_byte(i2c_io, ADXL345Reg_DATA_FORMAT, ADXL345_FULL_RES);
    adxl345_write_reg_byte(i2c_io, ADXL345Reg_POWER_CTL, ADXL345_MEASURE);

    while(1)
    {
        char buf[32];
        int16_t g[3];
        adxl345_read_reg(i2c_io, ADXL345Reg_DATAX0, g, 6);

//        gpio_set_pin_level(SPI_nSS, false);
        sprintf(buf, "(%d, %d, %d)\n", g[0], g[1], g[2]);
        io_write(usart_io, (uint8_t *) buf, strlen(buf));
//        io_write(spi_io, (uint8_t *) "Hello, World!", 13);
//        gpio_set_pin_level(SPI_nSS, true);



        delay_ms(100);
    }

    for(uint32_t i = 0; ++i;)
    {
        if((i & 0xfff) == 0xfff)
        {
            io_write(usart_io, (uint8_t *) "Hello, World!", 13);
            gpio_toggle_pin_level(MOTORB0);
        }

        stepper_motor_step(&stepper, i & 0x800);
        delay_us(2000);
    }
}
