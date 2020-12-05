#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "" 
#define FIREBASE_AUTH ""
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// pin
#define SENSOR D0

// system //
int isSystemOpened = 0;
bool hasItSetUp = false;
int breachNo = 0;

// sensor //
int SensorValue = 0;
bool objectDetected = false;

// firebase
String securityLogs = "sensorLogs/securityBreach";

// date
const long utcOffsetInSeconds = 10800; // 60*60*3(Turkey Time Zone);
char daysOfTheWeek[7][12] = {"Pazar", "Pazartesi", "Sali", "Carsamba", "Persembe", "Cuma", "Cumartesi"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  // serial connection
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  // start firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // pin setting
  pinMode(SENSOR,INPUT);
}

void loop() {
  isSystemOpened = Firebase.getFloat("isSystemOpened");

  if(isSystemOpened){
    if(!hasItSetUp){
      hasItSetUp = true;
      breachNo = Firebase.getInt("breachNo");
      Serial.println("Sistem aktiflestirildi.");
    }

    SensorValue = digitalRead(SENSOR);

    if(objectDetected == true && SensorValue == 0){
      objectDetected = false;  
    }

    if(objectDetected == false && SensorValue == 1){
      breachNo++;
      Firebase.setInt("breachNo", breachNo);

      String dateString = securityLogs + breachNo + "/date";
      Firebase.setString(dateString, getDateAndHour());

      Serial.print("Güvenlik İhlali : ");
      Serial.println(getDateAndHour());

      String confirmationString = securityLogs + breachNo + "/confirmation";
      Firebase.setBool(confirmationString, false);
      
      if (Firebase.failed()) {
        Serial.print("Firebase Hatası : Kayit firebase ortamina aktarilamadi.");
        Serial.println(Firebase.error());  
      }
      
      objectDetected = true;
      delay(500);
    }
  } else if(hasItSetUp){
    hasItSetUp = false;
    Serial.println("Sistem kapatildi.");
  }

  delay(1000);
}

String getDateAndHour(){
  String dateAndHour = "";
  
  timeClient.update();
  dateAndHour = daysOfTheWeek[timeClient.getDay()];
  dateAndHour = dateAndHour + ", ";
  dateAndHour = dateAndHour + timeClient.getHours();
  dateAndHour = dateAndHour + ':';
  dateAndHour = dateAndHour + timeClient.getMinutes();
  dateAndHour = dateAndHour + ':';
  dateAndHour = dateAndHour + timeClient.getSeconds();
  return dateAndHour;
}
