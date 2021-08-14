from btld_types import BOOTLOADER_FRAME
import serial
from crccheck.crc import Crc32, CrcXmodem
from crccheck.checksum import Checksum32	

"""
class message:

    def __init__(self):
        self.msg_id = 0
        self.msg_length = 0
        self.payload = bytearray([0x00])
        self.crc = 0
    
    def parse_msg(self, raw_data):
        begin = raw_data[0]
        self.msg_id = int.from_bytes(raw_data[1:2], byteorder="big")
        self.length = int.from_bytes(raw_data[3:4], byteorder="big")
        self.payload = raw_data[5:self.length+5]
        self.crc = int.from_bytes(raw_data[self.length+6, self.length+10], byteorder="big")
        if(self.crc != Crc32.calc(self.payload)):
            print("wrong crc")
        else:
            print("message_ok")
"""

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
        #data2 = bytearray(data.encode())
        crc = Crc32.calc(data)
        message = self.__START_OF_PACKET
        message = message + id.to_bytes(2, byteorder = "big")
        message = message + len(data).to_bytes(2, byteorder = "big")
        message = message + data #bytearray(data.encode())
        message = message + crc.to_bytes(4,byteorder = "big")
        message = message + bytearray([0xCD])
        self.ser.write(message)

    def send_btld_msg(self, command, data, id):
        msg = BOOTLOADER_FRAME.to_bytes(1, byteorder = "big")
        msg = msg + command.to_bytes(1, byteorder = "big")
        msg = msg + data.to_bytes(4, byteorder = "little")
        msg = msg + bytearray([0x00, 0x00])
        self.send_packet(msg, id)

    def read_respone(self):
        return self.ser.read(18)

