#include <FastLED.h>
#include <dht.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>

    String apiKey = "5GA4EQSNI2MEY61Z";
String ssid = "Shahid";
String password = "abc12345";

String RTS_Thingspeak = "AT+CIPSTART=\"TCP\",\"184.106.153.149\",80";
String Put_Temp_Thingspeak_Original = "GET /update?api_key=5GA4EQSNI2MEY61Z&field1=";
String RTS_Ubidots = "AT+CIPSTART=\"TCP\",\"translate.ubidots.com\",9012";
String Put_Fan_Ubidots_Original = "room1-esp|POST|A1E-g6uclt8J9VOCJtH1fN1U7adZPgHAgV|room1-iot=>fan:";
String Put_Light_Ubidots_Original = "room1-esp|POST|A1E-g6uclt8J9VOCJtH1fN1U7adZPgHAgV|room1-iot=>light:";
String Get_Fan_Ubidots = "room1-esp|LV|A1E-g6uclt8J9VOCJtH1fN1U7adZPgHAgV|room1-iot:fan|end";
String Get_Light_Ubidots = "room1-esp|LV|A1E-g6uclt8J9VOCJtH1fN1U7adZPgHAgV|room1-iot:light|end";

boolean DEBUG=true;

int tempPin = A0;
int Switch1IN = 2;
int Switch2IN = 3;
SoftwareSerial esp(4,5);
int Switch1OUT = 6;
int Switch2OUT = 7;

String ON = "2";
String OFF = "1";
String CIPSEND;
char data1,data2;
bool flag1,flag2,flag3 = false;
bool timer = false;


    
    #define LED_PIN     7 // led strip
    #define NUM_LEDS    15
    CRGB leds[NUM_LEDS];
    
    dht DHT;    //turbidity sensor 
    int DHT11[4] =  {8,9,10,11};
    //ambient light sensor
    Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
    int SensorID[4] = {1,2,3,4};
      
void setup() {


  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);

  attachInterrupt(0,Interrupt1,CHANGE);
  attachInterrupt(1,Interrupt2,CHANGE);
  Timer1.initialize(160000000);
  Timer1.attachInterrupt(Interrupt3);

  Serial.begin(115200);
  esp.begin(115200);

  esp.println("AT+RST");
  //showResponse(1000);

  esp.println("AT+CWMODE=1");
  //showResponse(1000);

  esp.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");
  //showResponse(5000);

  if (DEBUG)  Serial.println("Setup completed");

 
  for(int i=0;i<4;i++){
    pinMode(SensorID[i],OUTPUT);
    delay(100);
    digitalWrite(SensorID[i],LOW);
    delay(100);
  }
  Serial.begin(9600);
  for(int i=0;i<4;i++){
    digitalWrite(SensorID[i],HIGH);
    delay(100);
    if(!tsl.begin())
    {
      Serial.print("TSL2561 Not Connected");
    }
    configureSensor();
    delay(100);
    digitalWrite(SensorID[i],LOW);
    delay(100);
  }
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);

  
  
}

void loop() {


  
  if(flag1){
    flag1 = false;
    Serial.println("Swtch1 Main ISR");
    switch_1_changed();
  }
  if(flag2){
    flag2 = false;
    Serial.println("Swtch2 Main ISR");
    switch_2_changed();
  }
  if(flag3){
    flag3 = false;
    put_temp_on_thingspeak();
  }

  get_fan_from_ubidots();
  if(data1=='1'){
    digitalWrite(6,LOW);
    Serial.println("Fan OFF");
  }
  else if(data1=='2'){
    digitalWrite(6,HIGH);
    Serial.println("Fan ON");
  }
  get_light_from_ubidots();
  if(data2=='1'){
    digitalWrite(7,LOW);
    Serial.println("Light OFF");
  }
  else if(data2=='2'){
    digitalWrite(7,HIGH);
    Serial.println("Light ON");
  }
  delay(100);
  
  //ambient sensor
   for(int i=0;i<4;i++){
      Serial.print("Sensor Number ");
      Serial.print(i+1);
      Serial.print(" Value : ");
      Serial.println(getLightValue(i));
  }
  delay(5000);
  //turbidity sensor 
    Serial.print("Temperature : ");
  Serial.println(Temperature_Read(DHT11[0]));
  delay(2000);
  Serial.print("Humidity : ");
  Serial.println(Humidity_Read(DHT11[0]));
  delay(2000);
  
  // led strip
  leds[0] = CRGB(0, 0,255);
      FastLED.show();
      delay(500);  
      leds[1] = CRGB(0, 0,255);
      FastLED.show();
      delay(500);
      leds[2] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
      leds[3] = CRGB(0, 0,255);
      FastLED.show();
      delay(500);
      leds[4] = CRGB(0, 0,255);
      FastLED.show();
      delay(500);
      leds[5] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
      leds[6] = CRGB(0, 0,255);
      FastLED.show();
      delay(500);
      leds[7] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
      leds[8] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
      leds[9] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
      leds[10] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
      leds[11] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
      leds[12] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
      leds[13] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
      leds[14] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
      leds[15] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);leds[7] = CRGB(255, 0,0);
      FastLED.show();
      delay(500);
}

void configureSensor(void)
{
  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); 
}

float getLightValue(int ID){
  digitalWrite(SensorID[ID],HIGH);
  delay(100);
  sensors_event_t event;
  tsl.getEvent(&event);
  Serial.print(event.light); Serial.println(" lux");
  delay(250);
  digitalWrite(SensorID[ID],LOW);
  delay(100); 
}

void Interrupt1() {
  flag1 = true;
  Serial.println("Swtch1 ISR");
}

void Interrupt2() {
  flag2 = true;
  Serial.println("Swtch2 ISR");
}

void Interrupt3() {
  if(timer) {
    timer = false;
    flag3 = true;
  }
  else {
    timer = true;
  }
}

void put_temp_on_thingspeak() {
  int val = 0;
  val=analogRead(tempPin);
  float mv= val * (5000/1024.0);
  float cel = mv/10;
  char buf[16];
  String strTemp = dtostrf(cel, 4, 1, buf);
  Serial.println(strTemp);
  
  esp.println(RTS_Thingspeak);
  if(esp.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  String Put_Temp_Thingspeak = Put_Temp_Thingspeak_Original;
  Put_Temp_Thingspeak += String(strTemp);
  Put_Temp_Thingspeak += "\r\n\r\n";
  CIPSEND = "AT+CIPSEND=";
  CIPSEND += String(Put_Temp_Thingspeak.length());
  esp.println(CIPSEND);
  if(esp.find(">")){
    esp.print(Put_Temp_Thingspeak);
  }
  else{
    esp.println("AT+CIPCLOSE");
    Serial.println("AT+CIPCLOSE");
  }
}

void put_fan_on_ubidots(String state) {
  esp.println(RTS_Ubidots);
  if(esp.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  String Put_Fan_Ubidots = Put_Fan_Ubidots_Original;
  Put_Fan_Ubidots += String(state);
  Put_Fan_Ubidots += "|end";
  CIPSEND = "AT+CIPSEND=";
  CIPSEND += String(Put_Fan_Ubidots.length());
  esp.println(CIPSEND);
  if(esp.find(">")){
    esp.print(Put_Fan_Ubidots);
  }
  else{
    esp.println("AT+CIPCLOSE");
    Serial.println("AT+CIPCLOSE");
  }
  esp.println("AT+CIPCLOSE");
}

void put_light_on_ubidots(String state) {
  esp.println(RTS_Ubidots);
  if(esp.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  String Put_Light_Ubidots = Put_Light_Ubidots_Original;
  Put_Light_Ubidots += String(state);
  Put_Light_Ubidots += "|end";
  CIPSEND = "AT+CIPSEND=";
  CIPSEND += String(Put_Light_Ubidots.length());
  esp.println(CIPSEND);
  if(esp.find(">")){
    esp.print(Put_Light_Ubidots);
  }
  else{
    esp.println("AT+CIPCLOSE");
    Serial.println("AT+CIPCLOSE");
  }
  esp.println("AT+CIPCLOSE");
}

void get_fan_from_ubidots() {
  esp.println(RTS_Ubidots);
  if(esp.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  CIPSEND = "AT+CIPSEND=";
  CIPSEND += String(Get_Fan_Ubidots.length());
  esp.println(CIPSEND);
  if(esp.find(">")){
    esp.print(Get_Fan_Ubidots);
    if(esp.available()){
      data1=esp.read();
      long t=millis();
      while(data1!='|'){
        if(t+10000<millis()) break;
        data1=esp.read();
      }
      data1=esp.read();  
    }
  }
  else{
    esp.println("AT+CIPCLOSE");
    Serial.println("AT+CIPCLOSE");
  }
  esp.println("AT+CIPCLOSE");
}

void get_light_from_ubidots() {
  esp.println(RTS_Ubidots);
  if(esp.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  CIPSEND = "AT+CIPSEND=";
  CIPSEND += String(Get_Light_Ubidots.length());
  esp.println(CIPSEND);
  if(esp.find(">")){
    esp.print(Get_Light_Ubidots);
    if(esp.available()){
      data2=esp.read();
      long t=millis();
      while(data2!='|'){
        if(t+5000<millis()) break;
        data2=esp.read();
      }
      data2=esp.read();  
    }
  }
  else{
    esp.println("AT+CIPCLOSE");
    Serial.println("AT+CIPCLOSE");
  }
  esp.println("AT+CIPCLOSE");
}

void switch_1_changed(){
  if(digitalRead(2)==HIGH){
    digitalWrite(6,HIGH);
    put_fan_on_ubidots(ON);
  }
  else{
    digitalWrite(6,LOW);
    put_fan_on_ubidots(OFF);
  }
}

void switch_2_changed(){
  if(digitalRead(3)==HIGH){
    digitalWrite(7,HIGH);
    put_light_on_ubidots(ON);
  }
  else{
    digitalWrite(7,LOW);
    put_light_on_ubidots(OFF);
  }
}


float Temperature_Read(int ID){
  int chk = DHT.read11(ID);
  return DHT.temperature;
}
float Humidity_Read(int ID){
  int chk = DHT.read11(ID);
  return DHT.humidity;
}
