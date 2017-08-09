#include <arduino.h>
#include "userDef.h"

int vol,volCache;
unsigned long time_cache=0;

int keyRead()
{
  uint16_t temp;
  Serial.println(analogRead(A6));
  if(temp<PUSH_BUTTON)
  {
    delay(50);
    temp = analogRead(A6);
    if(temp<PUSH_BUTTON)
      return 1;
  }
  if(temp>PUSH_BUTTON && temp<LEFT_BUTTON)
  {
    delay(50);
    temp = analogRead(A6);
    if(temp>PUSH_BUTTON && temp<LEFT_BUTTON)
      return 2;
  }
  if(temp>LEFT_BUTTON && temp<RIGHT_BUTTON)
  {
    delay(50);
    temp = analogRead(A6);
    if(temp>LEFT_BUTTON && temp<RIGHT_BUTTON)
      return 3;
  }
  return 0;
}

int keyGet()
{
   int key = 0;
   vol = keyRead();  //检测输入动作
   if(vol==0){
      if(volCache == 1){
          key = 1;
      }else if(volCache > 1){
          key = volCache + 3; 
      }else{
          key = 0; 
      } 
      time_cache=millis();
  }
  volCache = vol;
  if(millis() > time_cache + 1000){
      key = vol + 1;
      volCache = 0;
  }  
  return key;
}




