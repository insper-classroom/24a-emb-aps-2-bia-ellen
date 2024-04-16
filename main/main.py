import serial
import pyautogui

ser = serial.Serial('COM10', 9600)

def compara_string(comando,string):
    return comando.decode()==string

while True:
    data = ser.read(3)
    print("data: ", data)
    if(compara_string(data,'ENT')):
        pyautogui.press('enter')
    
