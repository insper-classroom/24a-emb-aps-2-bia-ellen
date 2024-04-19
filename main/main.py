import serial
import pyautogui

ser = serial.Serial('COM10', 9600)

def compara_string(comando,string):
    return comando.decode()==string

while True:
    data = ser.read(3)
    print("data: ", data)
    if compara_string(data,'ENT'):
        pyautogui.press('enter')
    elif compara_string(data, 'ESC'):
        pyautogui.press('esc')
    elif compara_string(data, 'GED'):
        pyautogui.keyDown('a')
    elif compara_string(data, 'GES'):
        pyautogui.keyUp('a')
    elif compara_string(data, 'GDD'):
        pyautogui.keyDown('d')
    elif compara_string(data, 'GDS'):
        pyautogui.keyUp('d')
    elif compara_string(data, 'BDS'):
        pyautogui.keyDown('b')
    elif compara_string(data, 'BSB'):
        pyautogui.keyUp('b')

    
