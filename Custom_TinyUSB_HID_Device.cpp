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

/*****************************
 *   COMMON SECTION
 *****************************/ 

  #include <Custom_TinyUSB_HID_Device.h>
  
  #define RID_KEYBOARD 1
  #define RID_MOUSE 2
  #define RID_CONSUMER_CONTROL 3
  #define RID_GAMEPAD 4
  

  // HID report descriptor using TinyUSB's template
  // Single Report (no ID) descriptor
  uint8_t const desc_hid_report[] =
  {
    TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
    TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(RID_MOUSE) ),
    TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(RID_CONSUMER_CONTROL) ),
    TUD_HID_REPORT_DESC_GAMEPAD(HID_REPORT_ID(RID_GAMEPAD))
  };

  Adafruit_USBD_HID usb_hid;
  
/*****************************
 *   MOUSE SECTION
 *****************************/ 

  TinyMouse_::TinyMouse_(void) {
  }
  
  void TinyMouse_::begin(void) {
    _buttons = 0;
    usb_hid.setPollInterval(2);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();
    while( !USBDevice.mounted() ) delay(1);
  }

  void TinyMouse_::move (int8_t x, int8_t y, int8_t wheel, int8_t scroll) {
    if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
    while(!usb_hid.ready()) delay(1);
    usb_hid.mouseReport(RID_MOUSE,_buttons,x,y,wheel,scroll);
  }
  
  void TinyMouse_::end(void) 
  {
  }
  
  void TinyMouse_::click(uint8_t b)
  {
    _buttons = b;
    move(0,0,0,0);
    _buttons = 0;
    move(0,0,0,0);
  }
  
  void TinyMouse_::buttons(uint8_t b)
  {
    if (b != _buttons)
    {
      _buttons = b;
      move(0,0,0,0);
    }
  }
  
  void TinyMouse_::press(uint8_t b) 
  {
    buttons(_buttons | b);
  }
  
  void TinyMouse_::release(uint8_t b)
  {
    buttons(_buttons & ~b);
  }
  
  bool TinyMouse_::isPressed(uint8_t b)
  {
    if ((b & _buttons) > 0) 
      return true;
    return false;
  }
  
  TinyMouse_ Mouse;//create an instance of the Mouse object
  
 /*****************************
 *   KEYBOARD SECTION
 *****************************/ 

  TinyKeyboard_::TinyKeyboard_(void) {
  }
  
  void TinyKeyboard_::begin(void)
  {
    usb_hid.setPollInterval(2);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();
    while( !USBDevice.mounted() ) delay(1);
  }
  
  void TinyKeyboard_::end(void)
  {
  }
  
  void TinyKeyboard_::sendReport(KeyReport* keys)
  {
    if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
    while(!usb_hid.ready()) delay(1);
    usb_hid.keyboardReport(RID_KEYBOARD,keys->modifiers,keys->keys);
    delay(2);
  }

  // press() adds the specified key (printing, non-printing, or modifier)
  // to the persistent key report and sends the report.  Because of the way 
  // USB HID works, the host acts like the key remains pressed until we 
  // call release(), releaseAll(), or otherwise clear the report and resend.
  size_t TinyKeyboard_::press(uint8_t k) 
  {
    if (k == 0x00) return 0 ;
    // Distinguish between modifier keys and regular keys
    if ((k >= HID_KEY_CONTROL_LEFT) && (k <= HID_KEY_GUI_RIGHT)) {
        _keyReport.modifiers |= (1 << (k & 0x0F));
    } else {
      uint8_t i;
      // Add k to the key report only if it's not already present
      // and if there is an empty slot.
      if (_keyReport.keys[0] != k && _keyReport.keys[1] != k && 
        _keyReport.keys[2] != k && _keyReport.keys[3] != k &&
        _keyReport.keys[4] != k && _keyReport.keys[5] != k) {
      
        for (i=0; i<6; i++) {
          if (_keyReport.keys[i] == 0x00) {
            _keyReport.keys[i] = k;
            break;
          }
        }
        if (i == 6) {
          return 0;
        }    
      }
    }
    sendReport(&_keyReport);
    return 1;
  }
  
  // release() takes the specified key out of the persistent key report and
  // sends the report.  This tells the OS the key is no longer pressed and that
  // it shouldn't be repeated any more.
  size_t TinyKeyboard_::release(uint8_t k)
  {
    if (k == 0x00)
      return 0;
    // Distinguish between modifier keys and regular keys
    if ((k >= HID_KEY_CONTROL_LEFT) && (k <= HID_KEY_GUI_RIGHT))
    {
      _keyReport.modifiers &= ~(1 << (k & 0x0F));
    }
    else
    {
      uint8_t i;

      // Test the key report to see if k is present.  Clear it if it exists.
      // Check all positions in case the key is present more than once (which it shouldn't be)
      for (i = 0; i < 6; i++)
      {
        if (0 != k && _keyReport.keys[i] == k)
        {
          _keyReport.keys[i] = 0x00;
        }
      }
    }
    sendReport(&_keyReport);
    return 1;
  }

  void TinyKeyboard_::releaseAll(void)
  {
    _keyReport.keys[0] = 0;
    _keyReport.keys[1] = 0; 
    _keyReport.keys[2] = 0;
    _keyReport.keys[3] = 0; 
    _keyReport.keys[4] = 0;
    _keyReport.keys[5] = 0; 
    _keyReport.modifiers = 0;
    sendReport(&_keyReport);
  }

uint8_t const hid_ascii_to_keycode[128][2] =  { HID_ASCII_TO_KEYCODE };

void TinyKeyboard_::write(char ch) {
    // Check for valid ASCII value
    if (ch <= 0x7F) {
        // Apply shift modifier if required
        if (hid_ascii_to_keycode[(uint8_t)ch][0] == 1) {
            _keyReport.modifiers = KEYBOARD_MODIFIER_LEFTSHIFT;
        }
        _keyReport.keys[0] = hid_ascii_to_keycode[(uint8_t)ch][1];
        sendReport(&_keyReport);

        _keyReport.keys[0] = 0;
       sendReport(&_keyReport);
    }
}

void TinyKeyboard_::writeSequence(const char* str, uint16_t delayTime) {
    char current, next;
    while (*str != '\0') {
        current = *str;
        next = *(str++);

        write(current);
        delay(delayTime);
    }
}

void TinyKeyboard_::writeSequence(String str, uint16_t delayTime) {
    writeSequence(str.c_str(), delayTime);
}

  TinyKeyboard_ Keyboard;//create an instance of the Keyboard object

 /*****************************
 *   CONSUMER SECTION
 *****************************/ 

  TinyConsumer_::TinyConsumer_(void) {
  }
  
  void TinyConsumer_::begin(void)
  {
    usb_hid.setPollInterval(3);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();
    while( !USBDevice.mounted() ) delay(1);
  }
  
  void TinyConsumer_::end(void)
  {
  }

 void TinyConsumer_::sendCommand(uint8_t cmd)
  {
    if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
    while(!usb_hid.ready()) delay(1);
    usb_hid.sendReport16(RID_CONSUMER_CONTROL, cmd);
    delay(2);
  }

 void TinyConsumer_::press(uint8_t c) 
  {
    sendCommand(c);
  }
  
  void TinyConsumer_::release()
  {
    sendCommand(0);
  }
  
  void TinyConsumer_::send(uint8_t c)
  {
    press(c);  // Keydown
    release();            // Keyup
  }
TinyConsumer_ Consumer;//create an instance of the Consumer object


 /*****************************
 *   GAMEPAD SECTION
 *****************************/ 

  TinyGamepad_::TinyGamepad_(void) {
  }
  
  void TinyGamepad_::begin(void)
  {
    usb_hid.setPollInterval(4);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();
    while( !USBDevice.mounted() ) delay(1);
  }
  
  void TinyGamepad_::end(void)
  {
  }

   void TinyGamepad_::sendPadControl(hid_gamepad_report_t*  _gamepadReport) {
    if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
    while(!usb_hid.ready()) delay(1);
      usb_hid.sendReport(RID_GAMEPAD, &_gamepadReport, sizeof(_gamepadReport));
    delay(2);
  }

  void TinyGamepad_::press(uint16_t buttons){
    // Check if currently pressed buttons already include desired buttons to be pressed
    if ((_gamepadReport.buttons | buttons) != _gamepadReport.buttons) {
        _gamepadReport.buttons |= buttons;
        sendPadControl(&_gamepadReport);
    }
  }

  void TinyGamepad_::release(uint16_t buttons){
     // Check if desired buttons to be released are currently pressed
    if ((_gamepadReport.buttons & ~buttons) != _gamepadReport.buttons) {
        _gamepadReport.buttons &= ~buttons;
        sendPadControl(&_gamepadReport);
    }
  }

  void TinyGamepad_::releaseAll(void){
    _gamepadReport.buttons = 0;
    sendPadControl(&_gamepadReport);
  }

  void TinyGamepad_::setHat(uint8_t hat){
    if (hat <= GAMEPAD_HAT_UP_LEFT) {
      _gamepadReport.hat = hat;
      sendPadControl(&_gamepadReport);
    }
  }

  void TinyGamepad_::setAxes(int16_t x, int16_t y, int16_t z, int16_t Rz, int16_t Rx, int16_t Ry){
    if(x == -32768) { x = -32767; }
    if(y == -32768) { y = -32767; }
    if(z == -32768) { z = -32767; }
    if(Rz == -32768) { Rz = -32767; }
    if(Rx == -32768) { Rx = -32767; }
    if(Ry == -32768) { Ry = -32767; }
    _gamepadReport.x = x;
    _gamepadReport.y = y;
    _gamepadReport.z = z;
    _gamepadReport.rz = Rz;
    _gamepadReport.rx = Rx;
    _gamepadReport.ry = Ry;
    sendPadControl(&_gamepadReport);
  }

  void TinyGamepad_::setLeftStick(int16_t x, int16_t y){
    if(x == -32768) { x = -32767; }
    if(y == -32768) { y = -32767; }
    _gamepadReport.x = x;
    _gamepadReport.y = y;
    sendPadControl(&_gamepadReport);
  }

  void TinyGamepad_::setRightStick(int16_t z, int16_t Rz){
    if(z == -32768) { z = -32767; }
    if(Rz == -32768) { Rz = -32767; }
    _gamepadReport.z = z;
    _gamepadReport.rz = Rz;
    sendPadControl(&_gamepadReport);
  }

  void TinyGamepad_::setTriggers(int16_t Rx, int16_t Ry){
    if(Rx == -32768) { Rx = -32767; }
    if(Ry == -32768) { Ry = -32767; }
    _gamepadReport.rx = Rx;
    _gamepadReport.ry = Ry;
    sendPadControl(&_gamepadReport);
  }

TinyGamepad_ Gamepad;//create an instance of the Gamepad object

  

