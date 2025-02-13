#define RE 5
#define DE 4


byte values[11];
#include <AltSoftSerial.h>
AltSoftSerial sensor;
//------------------------//
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

#define lembab A0
#define PH_PIN A1
#define DS_PIN 12
const int AirValue = 456; //558 //471
const int WaterValue = 138;  //180 //131

float kelembaban = 0;
int persen = 0;
float persen1 = 0;
int suhu = 0;
int ph1 = 0;
float tegangan = 0;
float ph = 0.0;

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallas(&oneWire);
DeviceAddress insideThermometer;

#ifdef USE_SOFTSERIAL
#include <SoftwareSerial.h>
SoftwareSerial debug(6, 7);
#endif
uint8_t selected;

#include <SimpleKalmanFilter.h>
SimpleKalmanFilter KalmanFilter1(2, 2, 0.01);
SimpleKalmanFilter KalmanFilter2(2, 2, 0.01);
SimpleKalmanFilter KalmanFilter3(2, 2, 0.01);

void setup(){
  Serial.begin(9600);  
  Serial.println("LABEL,TANGGAL,Waktu,N,P,K,Suhu,Kelembaban,pH");
  dallas.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("     MONITORING     ");
  lcd.setCursor(0,1);
  lcd.print("   KESUBURAN TANAH  ");
  delay(2000); 

  sensor.begin(4800);
  #ifdef USE_SOFTSERIAL
  debug.begin(9600);
  #endif
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("NITROGEN:");
  lcd.setCursor(0,1);
  lcd.print("FOSFOR:");
  lcd.setCursor(0,2);
  lcd.print("KALIUM:");
  lcd.setCursor(0,3);
  lcd.print("Ph:");
  lcd.setCursor(7,3);
  lcd.print("S:");
  lcd.setCursor(14,3);
  lcd.print("K:");
}
float temp=0.0f;
double avg_ph= 0.0;
int cout = 0;
byte val1, val2, val3;
#define timer_wait_state 50

void loop(){
  dallas.setResolution(9);
  dallas.requestTemperatures();  
  temp = dallas.getTempCByIndex(0);
  suhu = (0.5065*temp) + 23.867;
//  suhu = KalmanFilter1.updateEstimate(suhu);
  for(int i=8;i<11;i++){
      lcd.setCursor(i,1);
      lcd.print(" ");
  }
  
  kelembaban = analogRead(lembab);
  persen1 = kelembaban - 2,073;
  persen = abs (persen1/ - 28.589);
  for(int i=16;i<20;i++){
      lcd.setCursor(i,1);
      lcd.print(" ");
  }
  
 
  val1 = nitrogen();
  for(int j=0; j < 1000; j++){
    ((0.215*ph1)-10.337);
    delayMicroseconds(100);
  }
  for(int i=10;i<13;i++){
      lcd.setCursor(i,3);
      lcd.print(" ");
  }
  
  val2 = phosphorous();
  for(int j=0; j < 1000; j++){
    ((0.215*ph1)-10.337);
    delayMicroseconds(100);
  }
  for(int i=10;i<13;i++){
      lcd.setCursor(i,3);
      lcd.print(" ");
  }

  val3 = potassium();
  for(int j=0; j < 1000; j++){
    ((0.215*ph1)-10.337);
    delayMicroseconds(100);
  }
  for(int i=10;i<13;i++){
      lcd.setCursor(i,3);
      lcd.print(" ");
  }

  ph1 = analogRead(PH_PIN);
  tegangan = ph1 * (5.0 / 1023.0);
  ph = (-0.1338*tegangan) + 6.7775;//avg_ph/6000.0;
//  ph = KalmanFilter3.updateEstimate(ph);
  for(int i=2;i<4;i++){
      lcd.setCursor(i,1);
      lcd.print(" ");
  }

  lcd.setCursor(16,3);
  lcd.print(persen);
  lcd.setCursor(9,3);
  lcd.print(suhu);
  lcd.setCursor(12,3);
  lcd.print("C");
  lcd.setCursor(10,0);
  lcd.print(val1);
  lcd.setCursor(13,0);
  lcd.print("mg/kg");
  lcd.setCursor(10,1);
  lcd.print(val2);
  lcd.setCursor(13,1);
  lcd.print("mg/kg");
  lcd.setCursor(10,2);
  lcd.print(val3);
  lcd.setCursor(13,2);
  lcd.print("mg/kg");
  lcd.setCursor(3,3);
  lcd.print(ph,1);
  Serial.print("DATA, DATE, TIME,");
  Serial.print(val1);
  Serial.print(",");
  Serial.print(val2);
  Serial.print(",");
  Serial.print(val3);
  Serial.print(",");
  Serial.print(suhu);
  Serial.print(",");
  Serial.print(persen);
  Serial.print(",");
  Serial.print(ph,1);
  Serial.println();
  delay(6000); 
}

//------------
const byte nitro[]  = { 0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c };
const byte phos[]   = { 0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc };
const byte kal[]    = { 0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0 };
const byte phcode[]     = {0x01,0x03,0x00,0x06,0x00,0x01,0x64,0x0B};
const byte thcode[]     = {0x01,0x03,0x00,0x12,0x00,0x02,0x64,0x0E};


byte phvalue[11];
float PH(){
  boolean b;  
  sensor.flushInput();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);
  for (uint8_t i = 0; i < sizeof(thcode); i++) sensor.write(thcode[i]);
  sensor.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(30);
  if(sensor.available()>0){
     int i = 0;
     delay(10);
     Serial.print("Response :: ");
     while(sensor.available()>0){
     //for(byte i = 0; i < 7; i++){
         //phvalue[i] = sensor.read();
         Serial.print(sensor.read(),HEX);Serial.print(" ");
         i++;
     }
     if(phvalue[0]== 0x01 && phvalue[1]== 0x03 && phvalue[2]== 0x02){
        b = true; 
     }
     Serial.println();
  }   
  return (((phvalue[3] * 256.0) + phvalue[4])/100);
}

byte nitrogen(){
  sensor.flushInput();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);
  for (uint8_t i = 0; i < sizeof(nitro); i++) sensor.write(nitro[i]);
  sensor.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(timer_wait_state);
  for (byte i = 0; i < 7; i++) 
  {
    values[i] = sensor.read();
  }
  if(values[0]==0x01 && values[1]==0x03 && values[2]==0x02){ // && values[5]==0xb9 && values[6]==0x9c){
    return values[4];
  } else {
    return val1;
  }
}

byte phosphorous(){
  sensor.flushInput();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);
  for (uint8_t i = 0; i < sizeof(phos); i++) sensor.write(phos[i]);
  sensor.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(timer_wait_state);
  for (byte i = 0; i < 7; i++) 
  {
    values[i] = sensor.read();
  }
  if(values[0]==0x01 && values[1]==0x03 && values[2]==0x02){ //&& values[5]==0xb9 && values[6]==0x9c){
    return values[4];
  } else {
    return val2;
  }
}

byte potassium(){
  sensor.flushInput();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);
  for (uint8_t i = 0; i < sizeof(kal); i++) sensor.write(kal[i]);
  sensor.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(timer_wait_state);
  for (byte i = 0; i < 7; i++) 
  {
    values[i] = sensor.read();
  }
  if(values[0]==0x01 && values[1]==0x03 && values[2]==0x02){ //&& values[5]==0x79 && values[6]==0x9f){
    return values[4];
  } else {
    return val3;
  }
}
