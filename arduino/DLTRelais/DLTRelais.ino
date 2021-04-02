#include <WButton.h>
#include <WLed.h>
#include <WRelais.h>
#include <WSerial.h>

WButton button;
WLed led;
WRelais relais1(D1);
WRelais relais2(D2);
WRelais relais3(D6);
WRelais relais4(D7);
WRelais relais5(D8);
WSerial serial;
WTimer timer;

void setup()
{
  serial.setup();
  button.setup();
  led.setup();
  relais1.setup();
  relais2.setup();
  relais3.setup();
  relais4.setup();
  relais5.setup();
  timer.setup();
  timer.start(1000);
  relais1.off();
  relais2.off();
  relais3.off();
  relais4.off();
  relais5.off();
  
  Serial.println("R10");
  Serial.println("R20");
  Serial.println("R30");
  Serial.println("R40");
  Serial.println("R50");
}

void loop()
{
  relais1.loop();
  relais2.loop();
  relais3.loop();
  relais4.loop();
  relais5.loop();

  switch(timer.event())
  {
    case WTimer::Expired:
      Serial.println("WD");     
      timer.start(1000);
      break;
  }
  
  switch(button.event())
  {
    case WButton::Pressed:
      led.on();
      break;
    case WButton::ShortPress:
      relais1.trigger(500);
      Serial.println("R1T");
      break;
    case WButton::LongPress:
      relais2.toggle();
      if(relais2.isOn())
        Serial.println("R21");
      else
        Serial.println("R20");
      break;
    case WButton::Released:
      led.off();
      break;
  }

  switch(serial.event())
  {
    case WSerial::Connected:
      if(relais1.isOn())
        Serial.println("R11");
      else
        Serial.println("R10");
      if(relais2.isOn())
        Serial.println("R21");
      else
        Serial.println("R20");
      if(relais3.isOn())
        Serial.println("R31");
      else
        Serial.println("R30");
      if(relais4.isOn())
        Serial.println("R41");
      else
        Serial.println("R40");
      if(relais5.isOn())
        Serial.println("R51");
      else
        Serial.println("R50");
      break;
    case WSerial::Disconnected:
      break;
    case WSerial::Line:
      String text = serial.line();
      if(text[0]=='R')
      {
        if(text[1]>='1' || text[1]<='5')
        {          
          if(text[2]=='0')
          {
            if(text[1]=='1')
              relais1.off();
            else if(text[1]=='2')
              relais2.off();
            else if(text[1]=='3')
              relais3.off();
            else if(text[1]=='4')
              relais4.off();
            else if(text[1]=='5')
              relais5.off();              
          }      
          else if(text[2]=='1')
          {
            if(text[1]=='1')
              relais1.on();
            else if(text[1]=='2')
              relais2.on();
            else if(text[1]=='3')
              relais3.on();
            else if(text[1]=='4')
              relais4.on();
            else if(text[1]=='5')
              relais5.on();             
          }      
          else if(text[2]=='T')
          {
            if(text[1]=='1')
              relais1.trigger(500);
            else if(text[1]=='2')
              relais2.trigger(500);
            else if(text[1]=='3')
              relais3.trigger(500);
            else if(text[1]=='4')
              relais4.trigger(500);
            else if(text[1]=='5')
              relais5.trigger(500);           }      
        }
      }
      break;
  }
  
}
