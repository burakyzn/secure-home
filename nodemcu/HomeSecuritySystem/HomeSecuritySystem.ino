#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "" // firebase host address
#define FIREBASE_AUTH "" // auth key
#define WIFI_SSID "" // your wifi name
#define WIFI_PASSWORD "" // your wifi pass

// pin
#define SENSOR D0

// system 
int isSystemOpened = 0;
bool hasItSetUp = false;
int breachNo;

// sensor 
int SensorValue = 0;
bool objectDetected = false;

// firebase
String securityLogs = "sensorLogs/securityBreach";

// date
const long utcOffsetInSeconds = 10800; // 60*60*3(Turkey Time Zone);
char daysOfTheWeek[7][12] = {"Pazar", "Pazartesi", "Sali", "Carsamba", "Persembe", "Cuma", "Cumartesi"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// ifttt setting
const int IFTTT_PORT = 80;
const char IFTTT_HOST_NAME[] = "maker.ifttt.com";
const String IFTTT_PATH_NAME = ""; // your ifttt trigger address

// client 
WiFiClient  client;

void setup() {
  // serial connection
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println();
  Serial.print("Baglaniyor");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Baglanti gerceklestirildi : ");
  Serial.println(WiFi.localIP());

  // start firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // pin setting
  pinMode(SENSOR,INPUT);
}

void loop() {
  // get system open-close information
  isSystemOpened = Firebase.getFloat("isSystemOpened");

  // if the system is open continue from this section
  if(isSystemOpened){
    if(!hasItSetUp){ // opened for the first time?
      hasItSetUp = true;
      breachNo = Firebase.getInt("breachNo"); // get breach number
      Serial.println("Sistem aktiflestirildi.");
    }

    SensorValue = digitalRead(SENSOR); // read the sensor value

    // the object was in front of the sensor, but now isn't it?
    if(objectDetected == true && SensorValue == 0){ 
      objectDetected = false;  
    }

    // is the object now in front of the sensor?
    if(objectDetected == false && SensorValue == 1){
      sendBreachData();
      sendMail();
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
  // getting time from ntp server
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

void sendBreachData(){
  // writing data to the Firebase Realtime Database
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
}

void sendMail(){
  // send email to user
  if(client.connect(IFTTT_HOST_NAME, IFTTT_PORT)) {
    client.println("GET " + IFTTT_PATH_NAME + " HTTP/1.1");
    client.println("Host: " + String(IFTTT_HOST_NAME));
    client.println("Connection: close");
    client.println();
    client.stop();
    Serial.println("Mail gonderildi.");
  } else {
    Serial.println("Baglanti Hatasi : Ag baglantisi gerceklestirilemiyor.");
  }
}
