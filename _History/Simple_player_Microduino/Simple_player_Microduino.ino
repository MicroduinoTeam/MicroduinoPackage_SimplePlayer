 #include <JQ6500.h>
 #include "JQ6500_def.h"
//用户自定义部分------------------------
#include "key.h"
#include "Ui.h"
#include "audio.h"

#define DRAW_LOOP 100	//主界面刷新时间间隔
//Core和CoreUSB默认使用软串口
SoftwareSerial mySerial(2, 3); // RX, TX
JQ6500 AUDIO(&mySerial);     //音频控制类实例,使用软串口（2，3）
//Core+默认使用Serial1
//JQ6500 AUDIO(&Serial1);

int musicVol=20;               //初始音量20
int musicMode = MODE_ALL;      //初始播放模式--全部循环   
boolean music_status = false;  //歌曲播放状态
int fileNum = 0;               //查询当前播放歌曲编号
int fileNumCache = 0;          //播放歌曲编号缓存
int totalNum = 0;              //歌曲总数量
int totalTime = 0;             //当前播放歌曲总时间
int playTime = 0;              //当前播放歌曲播放时间

String nameCache = "";

void setup() 
{
  Serial.begin(9600);

  config = eeprom_READ();      //从EEPROM读取配置文件
  musicMode = config.EEPROM_music_mode;     //读取播放模式
  musicVol = config.EEPROM_music_vol;       //读取播放音量
  musicMode = constrain(musicMode, MODE_ALL, MODE_ONE_STOP);
  musicVol = constrain(musicVol, 0, 30);

  AUDIO.init(DEVICE_TF, musicMode, musicVol);   //播放控制类初始化，设置播放设备为TF卡
  delay(1000);
  totalNum = AUDIO.queryTF();         //读取TF卡总文件数量
  totalNum = AUDIO.queryTF();
  totalNum = AUDIO.queryTF();
  AUDIO.choose(1);                  //默认从第一首曲目开始
  AUDIO.pause();                    //
  music_status = true;
}

void loop() 
{ 
  int key = keyGet();                 //获得按键指令

  if(key == 1){                       //按键按下，选择播放或暂停
     music_status=!music_status;      //播放或暂停
     music_status? AUDIO.play():AUDIO.pause();   
  }else if(key == 2){                 //按键长按，设置播放模式
     musicMode++;
     if(musicMode > 4)
        musicMode = 0;
     AUDIO.setMode(musicMode);            //设置播放模式
     eeprom_WRITE(musicMode, musicVol);   //保存到EEPROM中
  }else if(key == 3||key == 4){           //按键左拨或右拨（长拨）
     (key-3)? musicVol++ : musicVol--;    //音量+或音量-
     if(musicVol>30)
         musicVol=30;
     else if(musicVol<1)
         musicVol=1;
     AUDIO.volumn(musicVol);               //设置播放音量
     eeprom_WRITE(musicMode, musicVol);    //保存到EEPROM中
  }else if(key == 5||key == 6){     //按键左拨或右
     (key-5)? AUDIO.next() : AUDIO.prev();    //播放下一首或上一首
  }
  fileNum = AUDIO.queryTFFile();          //查询当前播放歌曲的编号
  if(fileNum != fileNumCache){            //发现播放歌曲更换
      fileNumCache = fileNum;           
      nameCache = AUDIO.queryName();      //查询当前播放歌曲名称
      totalTime = AUDIO.queryTotalTime();     //查询当前播放歌曲总时间
  }
  playTime = AUDIO.queryPlayTime();       //查询当前播放歌曲播放时间
  UI(fileNum,totalNum,musicMode,musicVol,music_status,nameCache,playTime,totalTime);
  delay(DRAW_LOOP);
}


