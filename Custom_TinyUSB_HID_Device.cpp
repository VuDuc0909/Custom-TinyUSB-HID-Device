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

/*****************************
 *   COMMON SECTION
 *****************************/ 

  #include <Custom_TinyUSB_HID_Device.h>
  
  #define RID_KEYBOARD 1
  #define RID_MOUSE 2
  #define RID_CONSUMER_CONTROL 3
  

  // HID report descriptor using TinyUSB's template
  // Single Report (no ID) descriptor
  uint8_t const desc_hid_report[] =
  {
    TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
    TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(RID_MOUSE) ),
    TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(RID_CONSUMER_CONTROL) )
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

  void TinyMouse_::move (int8_t x, int8_t y, int8_t wheel) {
    if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
    while(!usb_hid.ready()) delay(1);
    usb_hid.mouseReport(RID_MOUSE,_buttons,x,y,wheel,0);
  }
  
  void TinyMouse_::end(void) 
  {
  }
  
  void TinyMouse_::click(uint8_t b)
  {
    _buttons = b;
    move(0,0,0);
    _buttons = 0;
    move(0,0,0);
  }
  
  void TinyMouse_::buttons(uint8_t b)
  {
    if (b != _buttons)
    {
      _buttons = b;
      move(0,0,0);
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
    
    uint8_t i;
    _keyReport.modifiers = 0;
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
        setWriteError();
        return 0;
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
     uint8_t i;
    _keyReport.modifiers = 0;
    
    // Test the key report to see if k is present.  Clear it if it exists.
    // Check all positions in case the key is present more than once (which it shouldn't be)
    for (i=0; i<6; i++) {
      if (0 != k && _keyReport.keys[i] == k) {
        _keyReport.keys[i] = 0x00;
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

  

