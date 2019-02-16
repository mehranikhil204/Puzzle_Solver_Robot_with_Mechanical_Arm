import serial
import time
ser = serial.Serial(port='COM7',baudrate=9600)
x=[8,7]
ser.write('8,7')
