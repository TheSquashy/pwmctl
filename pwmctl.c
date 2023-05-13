#include <stdio.h>
#include <string.h>
#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <pico/binary_info.h>

//define our address and some other shit like bitrate.
const uint LED_PIN = 25;
static const uint I2C_SLAVE_ADDRESS = 0x17;
static const uint I2C_BAUDRATE = 100000;

//define what pins were gonna be using. These ones down here VV
static const uint I2C_SLAVE_SDA_PIN = PICO_DEFAULT_I2C_SDA_PIN;
static const uint I2C_SLAVE_SCL_PIN = PICO_DEFAULT_I2C_SCL_PIN;

static struct{
    bool cmdstart;
    uint8_t execute;
    int loc;
    uint8_t cmd[4];
    uint8_t cmdbuf[4];
} context;

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
        case I2C_SLAVE_RECEIVE: // if we get something
            if(context.loc > 3) {context.cmdstart = false;context.loc=0;context.execute++;memcpy(context.cmdbuf, context.cmd, sizeof(context.cmd));} // if we are beyond the last byte of the string, reset the commandin variable, reset the position variable, and copy the command to a more protected buffer. Then, change the value of the execute flag to let the main loop know it needs to execute a new command.
            if (!(i2c_read_byte_raw(i2c) & 0x80)) {
                uint8_t command_id = i2c_read_byte_raw(i2c);
                context.cmd[0] = command_id;
                context.cmdstart = true;
                context.loc=0;
            } else {
                uint8_t data = i2c_read_byte_raw(i2c);
                context.cmd[context.loc] = data;
            }
            context.loc++;
    }
    
}
bool run = true;



int main() {
    context.execute = 0;
    uint8_t currentid = 0;
    uint8_t localbuf[4];
    while (run == true) {
        if (context.execute != currentid) {
            currentid = context.execute;
            memcpy(localbuf,context.cmdbuf,sizeof(context.cmdbuf));
            switch (localbuf[0]) {
                case 0x11:
                    pwm_handle(localbuf);
                    break;
                case 0x12:
                    direction_handle(localbuf);
                    break;
                case 0x13:
                    lights_handle(localbuf);
                    break;
                default:
                    break;
            }
        }
    }
}