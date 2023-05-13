from smbus2 import SMBus, i2c_msg

def setDirection(motor, direction):
    with SMBus(1) as bus:
        msg = i2c_msg.write(0x17, [0x11,0x01,])
