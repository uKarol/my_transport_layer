import serial
from crccheck.crc import Crc32, CrcXmodem
from crccheck.checksum import Checksum32	

from btld_types import *

PACKAGE_OK = 0x1
PACKAGE_FAILED = 0x0

trash = 1111111112222222

class message:

    def __init__(self):
        self.msg_id = 0
        self.msg_length = 0
        self.payload = bytearray([0x00])
        self.crc = 0
        self.msg_main_type = UNKNOWN_FRAME
        self.msg_sub_type  = UNKNOWN_FRAME
        self.msg_special   = UNKNOWN_FRAME
        self.msg_data = 0
        self.wait_callback = None
        self.cts_callback = None
        self.transmission_error_callback = None
        self.btld_error_callback = None

    def assign_tl_callback(self, wait, cts):
        self.wait_callback = wait
        self.cts_callback = cts
    
    def parse_msg(self, raw_data):
        begin = raw_data[0]
        self.msg_id = int.from_bytes(raw_data[1:3], byteorder="little")
        self.length = int.from_bytes(raw_data[3:5], byteorder="little") - 10
        self.payload = raw_data[5:(self.length+5)]
        self.crc = int.from_bytes(raw_data[self.length+5: self.length+9], byteorder="little")
        if(self.crc != Crc32.calc(self.payload)):
            return PACKAGE_FAILED
        else:
            return PACKAGE_OK
    
    def decode_msg(self):
        if(self.payload[0] == TRANSMISSION_STATUS_FRAME):
            if(self.payload[1] == ACK):
                self.msg_main_type = TRANSMISSION_STATUS_FRAME
                self.msg_sub_type = ACK
            else:
                if(self.payload[2] == TOO_LONG):
                    print("frame is too long")
                elif(self.payload[2] == TOO_SHORT):
                    print("frame is too short")
                elif(self.payload[2] == BAD_CRC):
                    print("BAD_CRC")
                else: 
                    print("unknown problem")
                
        elif(self.payload[0] == TRANSPORT_LAYER_FRAME):
            if(self.payload[1] == FLOW_CONTROL):
                if(self.payload[2] == CTS ):
                    self.msg_data      = int.from_bytes(self.payload[4:8], byteorder="little")
                    self.cts_callback(self.msg_data-1)
                elif(self.payload[2] == WAIT ):
                    self.msg_data      = int.from_bytes(self.payload[4:8], byteorder="little")
                elif(self.payload[2] == ERROR):
                    print("Error")
                elif(self.payload[2] == FINISH):
                    print("FINISHED")

        elif(self.payload[0] == BOOTLOADER_FRAME):
            if(self.payload[1] == BTLD_ACK ):
                print("BTLD ACK")
            elif(self.payload[1] == BTLD_ERROR):
                print("BTLD ERROR")
            elif(self.payload[1] == BTLD_FINISH):
                print("BTLD_FINISH")
                
        else:
            print("UNKNOWN FRAME")