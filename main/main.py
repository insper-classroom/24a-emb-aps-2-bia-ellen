import serial
import pyautogui

ser = serial.Serial('COM10', 9600)

def extrai_dado(data):
    axis = int(data[0])
    value = int(data[1:4])
    return axis,value

def move_mouse(axis, value):
    if axis == 0:    # X-axis
        pyautogui.move(value, 0)
    elif axis == 1:  # Y-axis
        pyautogui.move(0, value)

try:
    while True:
        data = ser.read(4)

        if data[0]=='2':
            btn = data[1:4]
            print("data: ", btn)
            if compara_string(btn,'ENT'):
                pyautogui.press('enter')
            elif compara_string(btn, 'ESC'):
                pyautogui.press('esc')
            elif compara_string(btn, 'GED'):
                pyautogui.keyDown('a')
            elif compara_string(btn, 'GES'):
                pyautogui.keyUp('a')
            elif compara_string(btn, 'GDD'):
                pyautogui.keyDown('d')
            elif compara_string(btn, 'GDS'):
                pyautogui.keyUp('d')
            elif compara_string(btn, 'BDS'):
                pyautogui.keyDown('b')
            elif compara_string(btn, 'BSB'):
                pyautogui.keyUp('b')
        
        else:
            axis, value = extrai_dado(data)
            move_mouse(axis,value)

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()