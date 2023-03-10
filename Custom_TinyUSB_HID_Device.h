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
 *  NOTE2: This code is base on TinyUSB Mouse and Keyboard. 
 *    For detail of this lib see
 *    https://github.com/cyborg5/TinyUSB_Mouse_and_Keyboard
 *
 *  NOTE2: This code is derived from the standard Arduino Mouse.h, Mouse.cpp,
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
      void move(signed char x, signed char y, signed char wheel = 0, signed char scroll = 0); 
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

#define GAMEPAD_A       1
#define GAMEPAD_SOUTH   1

#define GAMEPAD_B       2
#define GAMEPAD_EAST    2

#define GAMEPAD_C       4

#define GAMEPAD_X       8
#define GAMEPAD_NORTH   8

#define GAMEPAD_Y       16
#define GAMEPAD_WEST    16

#define GAMEPAD_Z       32
#define GAMEPAD_TL      64
#define GAMEPAD_TR      128
#define GAMEPAD_TL2     256
#define GAMEPAD_TR2     512
#define GAMEPAD_SELECT  1024
#define GAMEPAD_START   2048
#define GAMEPAD_MODE    4096
#define GAMEPAD_THUMBL  8192
#define GAMEPAD_THUMBR  16384


  class TinyGamepad_ 
  {
  private:
    hid_gamepad_report_t    gp;
   void sendPadControl(hid_gamepad_report_t*   gp);
  public:
    TinyGamepad_(void);
    void begin(void);
    void end(void);
  };
  
  extern TinyGamepad_ Gamepad;

