#include <SoftwareSerial.h>
#include<Servo.h>
#include <Adafruit_ESP8266.h>

Servo myservo;
#define RX 10
#define TX 11
String AP = "utsa08";       
String PASS = "Riya@123"; 
String API = "TT5WOSOXLDP25XTJ";   
String HOST = "api.thingspeak.com";
String PORT = "80";
String field[] = {"field1", "field2"};
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
float valSensor = 1;
SoftwareSerial esp8266(RX,TX); 

int pos = 0;

/* sensor var */
 
int no_of_bins = 2;
int trigPin[2] = {6,8};
int echoPin[2] = {7,9};
float duration;
int speed_of_sound = 350;   //Speed of sound in m/s
float distance;               //Variable to store distance in meters
int height_of_bin = 21;
float percentage;

  
void setup() {
  Serial.begin(115200);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  int i;
  for(i = 0; i < no_of_bins; i++)
  {
      pinMode(trigPin[i], OUTPUT);
      pinMode(echoPin[i], INPUT);
  }
  myservo.attach(9);
}
void loop() {
 int i;
 String getData = "GET /update?api_key="+ API;
 for(i = 0; i < no_of_bins; i++)
 {
    valSensor = getSensorData(i);
    getData = getData + "&" + field[i] + "=" + String(valSensor);
 }
 
 /*******Sprinkler*********/
 if(valSensor>30){
for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
 }
 
 /**************************/
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 
 /******send message to pickup *****/
 if(valsensor > 50){
   esp8266.println("Dustbin full");
}
 /*********************************/
float getSensorData(int bin_no){
       
         digitalWrite(trigPin[bin_no], LOW);
         delayMicroseconds(2000);
         digitalWrite(trigPin[bin_no], HIGH);
         delayMicroseconds(15);
         digitalWrite(trigPin[bin_no], LOW);
    
         duration = pulseIn(echoPin[bin_no], HIGH);
         duration = duration/1000000.0;           //Convert duration from microseconds to seconds
    
         /*Serial.print("Duration = ");
         Serial.println(duration);*/
         
         distance = duration * speed_of_sound;
         distance = distance*100;               //Convert distance from meters to centimeters
         distance = distance/2;
    
         Serial.print("Distance measured by sensor ");
         Serial.print(bin_no+1);
         Serial.print(" in centimeters = ");
         Serial.println(distance);

         percentage = (distance/height_of_bin)*100;
    
         delay(1000);

         return percentage;
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("Success");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
