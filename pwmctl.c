#include <stdio.h>
#include <string.h>
#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <pico/binary_info.h>
#include <hardware/pwm.h>

//define our address and some other shit like bitrate.
const uint LED_PIN = 25;
static const uint I2C_SLAVE_ADDRESS = 0x17;
static const uint I2C_BAUDRATE = 100000;

//define what pins were gonna be using. These ones down here VV
static const uint I2C_SLAVE_SDA_PIN = 14;
static const uint I2C_SLAVE_SCL_PIN = 15;

bool run = true;


static struct{ // define the struct where we stash our variables
    bool cmdstart;
    uint8_t execute;
    int loc;
    uint8_t cmd[4];
    uint8_t cmdbuf[4];
} context;

const uint pins[] = {0,1,2,3,4,5,6,7};
const uint dirpins[] = {16,17,18,19,20,21,22,26};
void startup() {
    int size = sizeof pins / sizeof pins[0];
    for(int i = 0; i<size; i++){
        gpio_set_function(pins[i], GPIO_FUNC_PWM);
    }
    size = sizeof dirpins/ sizeof dirpins[0];
    for(int i =0; i<size;i++){
        gpio_init(dirpins[i]);
        gpio_set_dir(dirpins[i], GPIO_OUT);
    }
}


static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
        case I2C_SLAVE_RECEIVE: // if we start to get something
            
            if (context.cmdstart == false) { // and we're not mid command
                uint8_t command_id = i2c_read_byte_raw(i2c); //receive the byte
                context.cmd[context.loc] = command_id; //shove it in the buffer
                context.cmdstart = true; // we're now mid command
            } else { // we are mid command
                uint8_t data = i2c_read_byte_raw(i2c); //receive the byte
                context.cmd[context.loc] = data;//shove it in the buffer
            }
            context.loc++; // increment the buffer location.
            break;
        case I2C_SLAVE_FINISH: // as soon as the write is done
            context.cmdstart = false; // we're no longer mid-command
            context.loc=0; // so we go back to the beginning of the buffer
            context.execute++; // increment the execute flag
            memcpy(context.cmdbuf, context.cmd, sizeof(context.cmd)); // and copy the completed command to a safer buffer. Old rambly comment below VV
            break;
            // if we are beyond the last byte of the string, reset the commandin variable, reset the position variable, and copy the command to a more protected buffer. Then, change the value of the execute flag to let the main loop know it needs to execute a new command.
        default:
            context.cmdstart = false; // we're no longer mid-command
            context.loc=0; // so we go back to the beginning of the buffer
            context.execute++; // increment the execute flag
            memcpy(context.cmdbuf, context.cmd, sizeof(context.cmd)); // and copy the completed command to a safer buffer. Old rambly comment below VV
            break;
    }
    
}

void pwm_handle(uint8_t* array, bool accessory) { // basically i dont feel like rewriting this so just add a flag for wether or not we're controlling accessories.
    // my brother in christ we need to obtain the pwm speed
    uint16_t chardelmask = 0b1111111111111100; // use an and to disable the last two bits from the datagram
    //then post conversion shift everything right two places (by 1)
    uint8_t d1 = array[2];
    uint8_t d2 = array[3];
    uint16_t datagram = ((uint16_t)d1<<8)|d2;
    uint16_t dirmask = 1 <<1;
    int direction = (datagram & dirmask) !=0;
    uint16_t speed = datagram >> 2;
    speed = speed << 2; // faster x4 multiplication
    uint8_t num = array[1];
    for( int p = 0; p < 8; p++){
        uint8_t bitmask = 1 << p;
        int bitval = (num & bitmask) !=0;
        if (bitval) {
            pwm_set_gpio_level(pins[p], speed);
            gpio_put(dirpins[p], direction);
        }
    }
        
}


int main() {
    startup();
    context.execute = 0;
    uint8_t currentid = 0;
    uint8_t localbuf[4];
    while (run == true) {
        if (context.execute != currentid) {
            currentid = context.execute;
            memcpy(localbuf,context.cmdbuf,sizeof(context.cmdbuf));
            switch (localbuf[0]) {
                case 0x11:
                    pwm_handle(localbuf, false);
                    break;
                case 0x13:
                    pwm_handle(localbuf, true);
                    break;
                default:
                    break;
            }
        }
    }
}