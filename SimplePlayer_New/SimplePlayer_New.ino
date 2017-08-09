/*
   Microduino_开源音乐播放器示例程序
   套件WIKI地址：https://wiki.microduino.cn/index.php/%E5%BC%80%E6%BA%90%E9%9F%B3%E4%B9%90%E6%92%AD%E6%94%BE%E5%99%A8/zh
   功能：简易的MP3，使用Microduino_AudioPro、Microduino_SD模块、Sensor Joystick传感器、OLED屏等，详细清单见维基界面
   本例程适配新版 Arduino IDE 1.6.9 for Microduino
   更新时间：2017-08-03   by CWJ
*/

#include <Microduino_Key.h>      //按键库
#include <Microduino_AudioPro.h> //AudioPro库
#include <SD.h>                  //SD库
#include "Ui.h"                  //OLED显示库文件

AudioPro_FilePlayer musicPlayer =  AudioPro_FilePlayer(SD);  //AudioPro_FilePlayer实例化

AnalogKey keyAnalog[5] {(A0), (A0), (A0), (A0), (A0)};   //AnalogKey实例化
enum KeyName {
  UP, DOWN, LEFT, RIGHT, PRESS
};

bool music_status;              //定义音乐播放状态变量（暂停/播放）
int musicVol;                   //定义用户设置音量值变量（音量从小到大：0~20）
int setVol;                     //定义实际设置音量值变量（音量从小到大：127~0，单位-dB）

uint8_t musicNum = 1; //歌曲序号
uint8_t fileNum = 0;  //文件数量
String musicName;     //音乐名（英文，若为中文会识别成错误字符）

uint32_t oledTimer = millis();

//**********播放第num号音乐
void playNum(uint8_t num) {
  if (!musicPlayer.paused() || !musicPlayer.stopped()) {
    musicPlayer.stopPlaying();  //必要，否则SD类得不到关闭，内存溢出
  }

  musicName = musicPlayer.getMusicName(num);
  Serial.print(F("Playing:"));
  if (!musicPlayer.playMP3(musicName)) {
    Serial.println(F("ERROR"));
  }
  else {
    Serial.print(F("OK \t File: "));
    Serial.println(musicName);
  }
}

void setup() {
  //**********串口初始化
  Serial.begin(115200);

  //**********AudioPro初始化
  pinMode(SD_PIN_SEL, OUTPUT);
  digitalWrite(SD_PIN_SEL, HIGH);
  delay(500);
  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));

  //**********SD初始化
  if (!SD.begin(SD_PIN_SEL)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  //**********初始音量设置
  musicVol = 16;   //设置初始音量
  setVol = map(musicVol, 0, 20, 127, 0); //将用户设置的音量映射到实际设置的音量范围
  musicPlayer.setVolume(setVol, setVol); //设置音量
  Serial.print("Volume:");   //串口打印信息
  Serial.println(musicVol);  //串口打印用户设置的音量值

  //**********获取音乐文件信息，并通过串口打印出来
  Serial.println(F("Enter Index of File to play"));
  fileNum = musicPlayer.getMusicNum();  //可以获取SD卡中曲目列表以及数量
  Serial.print(F("Music Files : "));
  Serial.println(fileNum);
  for (uint8_t a = 0; a < fileNum; a++) {
    Serial.print("\t File[");
    Serial.print(a);
    Serial.print("]: ");
    Serial.println(musicPlayer.getMusicName(a));
  }
  musicName = "pressToStart!";

  //**********摇杆初始化
  for (uint8_t a = 0; a < 5; a++) {
    keyAnalog[a].begin(INPUT);
  }
  delay(200);
}


void loop() {
  //**********摇杆向上拨动，音量增大
  switch (keyAnalog[UP].readEvent(700 - 50, 700 + 50)) {
    case SHORT_PRESS:  { //短按，每按一次音量加1
        musicVol++;
        if (musicVol > 20) {
          musicVol = 20;
        }
        setVol = map(musicVol, 0, 20, 127, 1);
        musicPlayer.setVolume(setVol, setVol);
        Serial.print(F("Volume changed to:"));
        Serial.print(musicVol);
        Serial.println(F("[dB]"));
      }
      break;
    case LONG_PRESS: { //长按，音量不断增大
        musicVol++;
        if (musicVol > 20) {
          musicVol = 20;
        }
        setVol = map(musicVol, 0, 20, 127, 1);
        musicPlayer.setVolume(setVol, setVol);
        Serial.print(F("Volume changed to:"));
        Serial.print(musicVol);
        Serial.println(F("[dB]"));
        delay(100);
        break;
      }
  }
  //**********摇杆向下拨动，音量减小
  switch (keyAnalog[DOWN].readEvent(330 - 50, 330 + 50)) {
    case SHORT_PRESS: { //短按，每按一次音量减1
        musicVol--;
        if (musicVol < 1) {
          musicVol = 0;
        }
        setVol = map(musicVol, 0, 20, 127, 1);
        musicPlayer.setVolume(setVol, setVol);
        Serial.print(F("Volume changed to:"));
        Serial.print(musicVol);
        Serial.println(F("[dB]"));
      }
      break;
    case LONG_PRESS:  //长按，音量不断减小
      musicVol--;
      if (musicVol < 1) {
        musicVol = 0;
      }
      setVol = map(musicVol, 0, 20, 127, 1);
      musicPlayer.setVolume(setVol, setVol);
      Serial.print(F("Volume changed to:"));
      Serial.print(musicVol);
      Serial.println(F("[dB]"));

      delay(100);
      break;
  }
  //**********摇杆向左拨动，播放上一曲
  switch (keyAnalog[LEFT].readEvent(512 - 50, 512 + 50)) {
    case SHORT_PRESS:
      musicNum--;
      if (musicNum < 1 ) {
        musicNum = fileNum;
      }
      playNum(musicNum - 1);
      break;
  }
  //**********摇杆向右拨动，播放下一曲
  switch (keyAnalog[RIGHT].readEvent(860 - 50, 860 + 50)) {
    case SHORT_PRESS:
      musicNum++;
      if (musicNum > fileNum) {
        musicNum = 1;
      }
      playNum(musicNum - 1);
      break;
  }
  //**********摇杆按下，切换暂停或播放状态
  switch (keyAnalog[PRESS].readEvent(0, 50)) {
    case SHORT_PRESS:
      if (musicPlayer.stopped()) {
        Serial.println(F("Playing!"));
        playNum(musicNum - 1);
      }
      else if (! musicPlayer.paused()) {
        Serial.println("Paused");
        musicPlayer.setAmplifier(false);	//关闭运放
        musicPlayer.pausePlaying(true);		//暂停
      } else {
        Serial.println("Resumed");
        musicPlayer.setAmplifier(true);		//开启运放
        musicPlayer.pausePlaying(false);	//取消暂停
      }
      break;
    case LONG_PRESS:
      Serial.println(F("Stopping"));
      musicPlayer.stopPlaying();
      delay(500);
      break;
  }

  if (millis() - oledTimer > 200 ) //200ms执行一次
  {
    oledTimer = millis();
    music_status = (!musicPlayer.paused() && !musicPlayer.stopped());
    //OLED屏显示音乐播放信息
    UI(musicNum, fileNum, musicPlayer.getMonoMode(), musicVol,
       music_status, musicName, musicPlayer.decodeTime());
  }

  delay(20); //延时20ms
}

