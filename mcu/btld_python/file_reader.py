import os
from pathlib import Path
class file_reader:

    def __init__(self, path):
        self.path = path
        self.bin_file = None
        self.file_size = 0

    def open_file(self):
        self.bin_file = open(self.path, "rb")
        self.file_size = os.path.getsize(self.path)
        return self.file_size
        
    def read_bytes(self, size):
        return self.bin_file.read(size)


path = Path("C:\\Users\\Karol\\Desktop\\semestr_magisterski\\Praca_mgr\\btld_python\\test.txt")
dziadostwo = file_reader(path)
dziadostwo.open_file()
print(dziadostwo.read_bytes(20))