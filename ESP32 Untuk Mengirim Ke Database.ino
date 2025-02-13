#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#define WIFI_SSID "PRIYOSUPONO"
#define WIFI_PASSWORD "Markadi1928" 
#define API_KEY "AIzaSyAvv-W78FUETk6HyCFko0ML1EWesyn_KBc"
#define DATABASE_URL "https://tes-ta-c3dc7-default-rtdb.firebaseio.com" //pilih yg usa
#define USER_EMAIL ""
#define USER_PASSWORD ""
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData fbdo;

//Goggle Sheet
//#include "TRIGGER_WIFI.h"
//#include "TRIGGER_GOOGLESHEETS.h"
////#define DHTPIN D4
////#define DHTTYPE DHT11
////DHT dht11(DHTPIN, DHTTYPE);
//char column_name_in_sheets[ ][20] = {"value1","value2","value3","value4","value5","value6"};
//String Sheets_GAS_ID = "AKfycbxl3z_C6Up7y1zardMfViBKdtbGHZH2rBYy6aFVgJYa8_hMcPRYxmDLVDM385maNpx1XA";
//int No_of_Parameters = 5;


void setup() {
    Serial2.begin(9600);
    Serial.begin(9600);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED){
          Serial.print(".");
          delay(100);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.token_status_callback = tokenStatusCallback; 
    config.signer.tokens.legacy_token = "AIzaSyAvv-W78FUETk6HyCFko0ML1EWesyn_KBc";
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Firebase.RTDB.setMaxRetry(&fbdo, 3);
    Firebase.RTDB.setMaxErrorQueue(&fbdo, 30);
    fbdo.setResponseSize(1024);

//Google Sheet
//while (!Serial);
//Serial2.begin();
////dht11.begin();
//WIFI_Connect("RUMAH KITA LT.2B","allsellku24");
//Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
}
String data_received = "";
void loop() {
    if(Serial2.available()>0){
       delay(2);
       data_received = Serial2.readStringUntil('\n'); 
       data_received.trim();
       Serial.println(data_received);
       if(Firebase.ready()){      
          FirebaseJson json;        
          json.set("Nitrogen",     parsestring(data_received,',',0).toFloat());
          json.set("Fosfor",       parsestring(data_received,',',1).toFloat());
          json.set("Kalium",       parsestring(data_received,',',2).toFloat());
          json.set("Kelembaban",   parsestring(data_received,',',3).toFloat());
          json.set("Suhu",         parsestring(data_received,',',4).toFloat());
          json.set("pH",           parsestring(data_received,',',5).toFloat());
          if(Firebase.RTDB.setJSONAsync(&fbdo,"/kesuburan",&json)){
             Serial.println("Upload ok");
          }else{
            Serial.println("Upload failed");
          }
       }       
    }
}

String parsestring(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length();
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

//Google Sheet
//Data_to_Sheets(No_of_Parameters,  nitrogen,  phosphorus, potassium, humidity, temp, ph);
//Serial.println();
//delay(5000);
