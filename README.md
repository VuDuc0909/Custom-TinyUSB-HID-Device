# Custom TinyUSB HID Device

This library provides an Arduino Mouse.h and Keyboard.h compatible API for TinyUSB stack mouse, keyboard, consumer control and gamepad.

Copy all files to your "Arduino/libraries". See the "examples" folder for sample sketches.

This module simulates the standard Arduino "Mouse.h" API and Arduino "Keyboard.h" API for use with the TinyUSB HID API. Instead of doing

    #include <HID.h>
    #include <Mouse.h>
    #include <Keyboard.h>

Simply do

    #include <Custom_TinyUSB_HID_Device.h>

## Redesigned from initial release to be a single library
 

