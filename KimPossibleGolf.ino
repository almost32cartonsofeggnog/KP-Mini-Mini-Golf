#include <SoftwareSerial.h>
#include <time.h>
#include <Servo.h>

//setting pins
#define ARDUINO_RX 11  //should connect toLesson plan  TX of the Serial MP3 Player module
#define ARDUINO_TX 10  //connect to RX of the module
#define startButton 6 
#define trapPin 7
#define doorPin 8

SoftwareSerial mp3(ARDUINO_RX, ARDUINO_TX);

static int8_t Send_buf[8] = {0};

int gamemodeToggle = 2;

Servo trapDoor;
Servo cornerDoor;

long startTime = 0;
long moveTime = 10000;
long currentTime = 0;

int doorState = 0;
int trapState = 0;

#define CMD_PLAY_W_INDEX  0X03




void setup()
{
  mp3.begin(9600); //setup mp3
  cornerDoor.attach(doorPin);//setup servos
  trapDoor.attach(trapPin);
  Serial.begin(9600);  //for testing
  pinMode(startButton, INPUT); //define button pin
  trapDoor.write(95); //sets trap door in line with ground
}



void loop()
{
  long currentTime = millis();
  //reset - when pressed, toggle gamemode start/sleep
  if (digitalRead(startButton) == HIGH) {
    gamemodeToggle++;
    if(gamemodeToggle > 2){
      gamemodeToggle = 0;
    }
    delay(1000);    //button press buffer - long press is reset, short is sleep mode
  }


  //play song (0)
  if (gamemodeToggle == 0) {
    sendCommand(CMD_PLAY_W_INDEX, 0, 2);
    delay(1000);
    gamemodeToggle ++;
    startTime = currentTime; //sets the first time for warning & servos
    moveTime = currentTime + random(0, 6000) + 8000;
    doorState = (90 * random(0,4)) +15;//sets the first angle for door
    trapState = random(0,4); //sets first state for trapdoor - random number between 0-4, if less than 1 then open
    Serial.print(startTime); //for testing
    Serial.print("  -  ");
    Serial.println(moveTime);
    Serial.println(doorState);
    Serial.println(trapState);
  }


  //sleepmode (2)

  //play mode (1)
  if (gamemodeToggle == 1){
    Serial.print(currentTime); //for testing
    Serial.print("  -  ");
    Serial.println(moveTime);
    
    //wait for time and then run sound & servos
    if(currentTime >= moveTime){
      sendCommand(CMD_PLAY_W_INDEX, 0, 1); //warning beeps
      delay(4000); //warning wait
      cornerDoor.write(doorState); //rotates bueno nacho
      Serial.println(doorState);  //for testing
      if(trapState < 1){
        trapDoor.write(140);//down
      }
      else{
        trapDoor.write(95);//stay up
      }
      doorState = (90 * random(0,4)) +15;//resets the angle for door
      trapState = random(0,4); //resets first state for trapdoor - random number between 0-4, if less than 1 then ope
      startTime = currentTime; //resets timing for warning & servo action
      moveTime = currentTime + random(0, 6000) + 9000; //reset time
    }
  }

Serial.println(gamemodeToggle);
   
   
}



/********************************************************************************/
/*Function: Send command to the MP3                                             */
/*Parameter: byte command                                                       */
/*Parameter: byte dat1 parameter for the command                                */
/*Parameter: byte dat2 parameter for the command                                */

void sendCommand(byte command){
  sendCommand(command, 0, 0);
}

void sendCommand(byte command, byte dat1, byte dat2){
  delay(20);
  Send_buf[0] = 0x7E;    //
  Send_buf[1] = 0xFF;    //
  Send_buf[2] = 0x06;    // Len
  Send_buf[3] = command; //
  Send_buf[4] = 0x01;    // 0x00 NO, 0x01 feedback
  Send_buf[5] = dat1;    // datah
  Send_buf[6] = dat2;    // datal
  Send_buf[7] = 0xEF;    //
  for (uint8_t i = 0; i < 8; i++)
  {
    mp3.write(Send_buf[i]) ;
  }
}
