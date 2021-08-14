import serial
from crccheck.crc import Crc32, CrcXmodem
from crccheck.checksum import Checksum32	

from btld_types import *

PACKAGE_OK = 0x1
PACKAGE_FAILED = 0x0

class Decode:

    def decode_msg(self, msg):
        if(msg[0] == FRAME_TYPE.TRANSMISSION_STATUS_FRAME):
            if(msg[1] == TRANSMISSION_FRAME.ACK):
                print("ACK")
            else:
                if(msg[2] == FRAME_ERROR_TYPE.TOO_LONG):
                    print("frame is too long")
                elif(msg[2] == FRAME_ERROR_TYPE.TOO_SHORT):
                    print("frame is too short")
                elif(msg[2] == FRAME_ERROR_TYPE.BAD_CRC):
                    print("BAD_CRC")
                
        elif(msg[0] == FRAME_TYPE.TRANSPORT_LAYER_FRAME):
            pass
        elif(msg[0] == FRAME_TYPE.TRANSPORT_LAYER_FRAME):
            pass
        else:
            print("UNKNOWN FRAME")
class message:

    def __init__(self):
        self.msg_id = 0
        self.msg_length = 0
        self.payload = bytearray([0x00])
        self.crc = 0
    
    def parse_msg(self, raw_data):
        begin = raw_data[0]
        self.length = int.from_bytes(raw_data[1:3], byteorder="big")
        self.msg_id = int.from_bytes(raw_data[3:5], byteorder="big")
        self.payload = raw_data[5:(self.length+5)]
        self.crc = int.from_bytes(raw_data[self.length+5: self.length+9], byteorder="big")
        print("msg length" + str(self.length))
        print(self.payload)
        print(hex(begin)) 
        print("msg length" + str(self.msg_id))
        print(self.crc)
        if(self.crc != Crc32.calc(self.payload)):
            print("wrong crc")
            return PACKAGE_FAILED
        else:
            print("message_ok")
            return PACKAGE_OK

class serializer:

    def __init__(self):
        self.__START_OF_PACKET = bytearray([0xAB])
        

    def open_serial_port( self, port_id, baud_rate ):
	    try:
		    self.ser = serial.Serial(port_id, baud_rate)
	    except serial.serialutil.SerialException:
		    print("cannot open serial port")
	    except ValueError:
		    print("value out of range")

    def send_packet(self, data, id):
        data2 = bytearray(data.encode())
        crc = Crc32.calc(data2)
        message = self.__START_OF_PACKET
        message = message + len(data).to_bytes(2, byteorder = "big")
        message = message + id.to_bytes(2, byteorder = "big")
        message = message + bytearray(data.encode())
        message = message + crc.to_bytes(4,byteorder = "big")
        message = message + bytearray([0xCD])
        return message
        #self.ser.write(message)
    
    def read_respone(self, lng):
        print( self.ser.read(lng))

    def send_crashed_packet(self, data, error_code):
        crc = 0
        if( error_code == NO_START_BYTE ):
            message = bytearray([0xAA])
            message = message + len(data).to_bytes(1, byteorder = "big")
            message = message + crc.to_bytes(4,byteorder = "big")
            message = message + bytearray(data.encode())
            message = message + bytearray([0xCD])
        elif( error_code == TOO_LONG ):
            message = bytearray([0xAB])
            message = message + (len(data)-1).to_bytes(1, byteorder = "big")
            message = message + crc.to_bytes(4,byteorder = "big")
            message = message + bytearray(data.encode())
            message = message + bytearray([0xCD])
        elif( error_code == TOO_SHORT ):
            message = bytearray([0xAB])
            message = message + (len(data)+1).to_bytes(1, byteorder = "big")
            message = message + crc.to_bytes(4,byteorder = "big")
            message = message + bytearray(data.encode())
            message = message + bytearray([0xCD])
        
        #self.ser.write(message)

my_serializer = serializer()
msg = message()

print("start")

pck = my_serializer.send_packet("abcdefgh", 1)
msg.parse_msg(pck)
pck = my_serializer.send_packet("123456789", 1)
msg.parse_msg(pck)
pck = my_serializer.send_packet("00000", 1)
msg.parse_msg(pck)