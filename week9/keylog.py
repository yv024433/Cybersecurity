#!/usr/bin/env python3
 
from pynput import keyboard
 
def get_key_name(key):
    if isinstance(key, keyboard.KeyCode):
        return key.char
    else:
        return str(key)

def on_release(key):
    key_name = get_key_name(key)
    print('Pressed: {}'.format(key_name))
 
    if key_name == 'Key.esc':
        print('Exiting...')
        return False
 
with keyboard.Listener(on_release = on_release) as listener:
    listener.join()