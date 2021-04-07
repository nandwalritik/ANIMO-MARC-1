#include<UniversalTelegramBot.h>
#include<ArduinoJson.h>
#include <Servo.h> 
#include <ESP8266WiFi.h>
#include<WiFiClientSecure.h>

#define botToken "YOUR_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"

Servo servo;
WiFiClientSecure client;
UniversalTelegramBot bot(botToken,client);
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

char* ssid = "WIFI_SSID";
char* password = "WIFI_PASSWORD";
int echo=15,trig=16;
int input1=0,input2=2;    // LEFT Motor
int input3=14,input4=12;  // RIGHT Motor
int en1=5,en2=4;

int distance;



void handleMessages(int numNewMessages){
  Serial.println("Handle New messages");
  Serial.println(String(numNewMessages));
  for(int i=0;i<numNewMessages;i++){
    if (bot.messages[i].type ==  F("callback_query")) {
      String text = bot.messages[i].text;
      Serial.print("Call back button pressed with text: ");
      Serial.println(text);

      if (text == F("F")) {
        moveForward();
      } else if (text == F("B")) {
        moveBackward();
      } else if( text == F("L")){
        turnLeft();
      } else if(text == F("R")){
        turnRight();
      }else if(text == F("S")){
        stopBot();
      }
      }else{
        String chat_id = String(bot.messages[i].chat_id);
        Serial.println(chat_id);
        if(chat_id != CHAT_ID){
          bot.sendMessage(chat_id,"Unauthorized user");
          continue;
        } 
        String text = bot.messages[i].text;
        Serial.println(text);
        String from_name = bot.messages[i].from_name;
        if (text == "/start") {
          String welcome = "Welcome, " + from_name + ".\n";
          welcome += "Use the following commands to control your outputs.\n\n";
          welcome += "/F to Move Forward \n";
          welcome += "/B to Move Backward\n";
          welcome += "/L to Left Turn\n";
          welcome += "/R to Right Turn\n";
          bot.sendMessage(chat_id, welcome, "");
        }
        if (text == "/options")
        {
            String keyboardJson = F("[[{ \"text\" : \"Forward\", \"callback_data\" : \"F\" },{ \"text\" : \"Backward\", \"callback_data\" : \"B\" }],");
            keyboardJson += F("[{ \"text\" : \"Left\", \"callback_data\" : \"L\" },{ \"text\" : \"Right\", \"callback_data\" : \"R\" }],");
            keyboardJson += F("[{ \"text\" : \"Stop\", \"callback_data\" : \"S\" }]]");
            bot.sendMessageWithInlineKeyboard(chat_id, "Commands", "", keyboardJson);
        }
      }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servo.attach(D7);
  servo.write(90);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");  
  }
  
  Serial.println("WiFi connected");
  Serial.println("Server Started");
  Serial.print("IP Address of network: ");
  Serial.println(WiFi.localIP());
  
  #ifdef ESP8266
    client.setInsecure();
  #endif
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(input1,OUTPUT);
  pinMode(input2,OUTPUT);
  pinMode(input3,OUTPUT);
  pinMode(input4,OUTPUT);
}


void loop() {
//  if (millis() > lastTimeBotRan + botRequestDelay)  {
//    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    int distanceR = 0;
    int distanceL =  0;
    delay(40);
    Serial.print(distance);
    if(distance<=15)
    {
      stopBot();
      delay(100);
      moveBackward();
      delay(300);
      stopBot();
      delay(200);
      distanceR = lookRight();
      delay(200);
      distanceL = lookLeft();
      delay(200);
    
      if(distanceR>=distanceL)
      {
        rotateRight();
        stopBot();
      }else
      {
        rotateLeft();
        stopBot();
      }
     }
    else{
      moveForward();
//      while(numNewMessages) {
//        Serial.println("got response");
//        handleMessages(numNewMessages);
//        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//      }
    }
    distance = readPing();
    
//    lastTimeBotRan = millis();
//  }
}

void moveForward(){
  digitalWrite(input1,HIGH);
  digitalWrite(input2,LOW);
  digitalWrite(input3,HIGH);
  digitalWrite(input4,LOW);
}

void moveBackward(){
  digitalWrite(input1,LOW);
  digitalWrite(input2,HIGH);
  digitalWrite(input3,LOW);
  digitalWrite(input4,HIGH);
}

void rotateRight(){
  digitalWrite(input1,HIGH);
  digitalWrite(input2,LOW);
  digitalWrite(input3,LOW);
  digitalWrite(input4,HIGH);
  delay(500);
}

void rotateLeft(){
  digitalWrite(input1,LOW);
  digitalWrite(input2,HIGH);
  digitalWrite(input3,HIGH);
  digitalWrite(input4,LOW);
  delay(500);
}

void turnLeft(){
  digitalWrite(input1,LOW);
  digitalWrite(input2,LOW);
  digitalWrite(input3,HIGH);
  digitalWrite(input4,LOW);
}

void turnRight(){
  digitalWrite(input1,HIGH);
  digitalWrite(input2,LOW);
  digitalWrite(input3,LOW);
  digitalWrite(input4,LOW);
}
void stopBot(){
  digitalWrite(input1,LOW);
  digitalWrite(input2,LOW);
  digitalWrite(input3,LOW);
  digitalWrite(input4,LOW);
}
int readPing() { 
  delay(70);
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  long duration = pulseIn(echo,HIGH);
  int cm = duration*0.034/2;
  Serial.println(cm);
  return cm;
}
int lookLeft(){
  servo.write(170); 
  delay(500);
  int d = readPing();
  delay(100);
  servo.write(91); 
  return d;
}
int lookRight(){
  servo.write(15); 
  delay(500);
  int d = readPing();
  delay(100);
  servo.write(91); 
  return distance;
}
