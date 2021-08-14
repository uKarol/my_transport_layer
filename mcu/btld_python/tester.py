from sender import *
from decoder import *
from file_reader import *

class btld_receiver():
    
    def __init__(self, path):
        self.serializer = serializer()
        self.message = message()
        self.message.assign_tl_callback(None, self.send_next_bytes )
        self.reader = file_reader(path)
        self.file_size = self.reader.open_file()
        

    def connect(self, com, baud):
        self.serializer.open_serial_port(com, baud)
        print("start")
    
    def send_and_read(self, command, data, id):
        self.serializer.send_btld_msg(command, data, id)
        self.read_and_decode_response()

    def send_next_bytes(self, size):
        data = self.reader.read_bytes(size)
        self.send_data(data, 1)

    def send_data(self, packet, id):
        my_receiver.serializer.send_packet(packet, id)
        self.read_and_decode_response()

    def read_and_decode_response(self):
        raw_data = self.serializer.read_respone()
        self.message.parse_msg(raw_data)
        self.message.decode_msg()

path = Path("C:\\Users\\Karol\\Desktop\\semestr_magisterski\\Praca_mgr\\testowy_kod\\testowy\Debug\\testowy.bin")    
my_receiver = btld_receiver(path)

my_receiver.connect("COM14", 115200)
my_receiver.send_and_read(BTLD_ACTIVATE, 3, 0)
my_receiver.read_and_decode_response()
my_receiver.send_and_read(BTLD_PROGRAM_SIZE, my_receiver.file_size, 0)
my_receiver.read_and_decode_response()
while(my_receiver.message.msg_sub_type!=FINISH):
    my_receiver.read_and_decode_response()