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
        pyautogui.keyDown('shiftleft')
    elif compara_string(data, 'GES'):
        pyautogui.keyUp('shiftleft')
    elif compara_string(data, 'GDD'):
        pyautogui.keyDown('shiftright')
    elif compara_string(data, 'GDS'):
        pyautogui.keyUp('shiftright')
    elif compara_string(data, 'VIS'):
        pyautogui.press('c')

    
