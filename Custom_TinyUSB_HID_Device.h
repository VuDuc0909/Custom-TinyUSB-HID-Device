/*
 * This module simulates the standard Arduino "Mouse.h" and
 * "Keyboard.h" API or other HID device for use with the TinyUSB HID API. Instead of doing
 *  #include <HID.h>
 *  #include <Mouse.h>
 *  #include <Keyboard.h>
 *  
 *  Simply do
 *  
 *  #include <Custom_TinyUSB_HID_Device.h>
 *  
 *  For details on Arduino Mouse.h see
 *   https://www.arduino.cc/reference/en/language/functions/usb/mouse/
 *  For details on Arduino Keyboard.h see
 *   https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
 *
 *  NOTE1: This code is base on TinyUSB Mouse and Keyboard. 
 *    For detail of this lib see
 *    https://github.com/cyborg5/TinyUSB_Mouse_and_Keyboard
 *
 *  NOTE2: For keycode use in this lib see in this link (tinyusb lib):
 *  https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
 *
 *  NOTE3: This code is derived from the standard Arduino Mouse.h, Mouse.cpp,
 *    Keyboard.h, and Keyboard.cpp code. The copyright on that original code
 *    is as follows.
 *   
 *  Copyright (c) 2015, Arduino LLC
 *  Original code (pre-library): Copyright (c) 2011, Peter Barrett
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
 #include <Arduino.h>
 #include <Adafruit_TinyUSB.h>

/*****************************
 *   MOUSE SECTION
 *****************************/ 
  #define MOUSE_LEFT 1
  #define MOUSE_RIGHT 2
  #define MOUSE_MIDDLE 4
  #define MOUSE_BACKWARD 8
  #define MOUSE_FORWARD 16

  /*
   * This class contains the exact same methods as the Arduino Mouse.h class.
   */
  class TinyMouse_
  {
    private:
      uint8_t _buttons;
      void buttons(uint8_t b);
    public:
      TinyMouse_(void);
      void begin(void);
      void end(void);
      void click(uint8_t b = MOUSE_LEFT);
      void move(signed char x, signed char y, signed char wheel = 0, signed char scroll = 0); //scroll meaning horizontal scroll
      void press(uint8_t b = MOUSE_LEFT);   // press LEFT by default
      void release(uint8_t b = MOUSE_LEFT); // release LEFT by default
      bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default
  };
  
  extern TinyMouse_ Mouse;

/*****************************
 *   KEYBOARD SECTION
 *****************************/ 
  
  //  Low level key report: up to 6 keys and shift, ctrl etc at once
  typedef struct
  {
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[6];
  } KeyReport;
  
  /*
   * This class contains the exact same methods as the Arduino Keyboard.h class.
   */
  
  class TinyKeyboard_ 
  {
  private:
    KeyReport _keyReport;
    void sendReport(KeyReport* keys);
  public:
    TinyKeyboard_(void);
    void begin(void);
    void end(void);
    size_t press(uint8_t k);
    size_t release(uint8_t k);
    void releaseAll(void);
    void write(char ch);
    void writeSequence(const char* str, uint16_t delayTime);
    void writeSequence(String str, uint16_t delayTime);
  };
  
  extern TinyKeyboard_ Keyboard;

/*****************************
 *   CONSUMER SECTION
 *****************************/ 

  class TinyConsumer_ 
  {
  private:
    void sendCommand(uint8_t cmd);
  public:
    TinyConsumer_(void);
    void begin(void);
    void end(void);
    void press(uint8_t k);
    void release();
    void send(uint8_t k);
  };
  
  extern TinyConsumer_ Consumer;

/*****************************
 *   GAMEPAD SECTION
 *****************************/ 

  class TinyGamepad_ 
  {
  private:
    hid_gamepad_report_t    _gamepadReport;
   void sendPadControl(hid_gamepad_report_t*   _gamepadReport);
  public:
    TinyGamepad_(void);
    void begin(void);
    void end(void);
    void press(uint16_t buttons);
    void release(uint16_t buttons);
    void releaseAll(void);

    void setHat(uint8_t hat);

    void setAxes(int16_t x = 0, int16_t y = 0, int16_t z = 0, int16_t Rz = 0, int16_t Rx = 0, int16_t Ry = 0);
    void setLeftStick(int16_t x = 0, int16_t y = 0);
    void setRightStick(int16_t z = 0, int16_t Rz = 0);
    void setTriggers(int16_t Rx = 0, int16_t Ry = 0);
  };
  
  extern TinyGamepad_ Gamepad;

