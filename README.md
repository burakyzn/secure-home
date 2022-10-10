# ✏️ Home Security System (IoT)

It is a smart home security system. It can report security breaches and provides a web panel to manage breaches.
NodeMCU, Firebase and an MZ-80 infrared sensor were used in this project.

# ⚙️ Install

Clone the project to your computer.
```
> git clone https://github.com/burakyzn/iot-home-security-system.git
```

You need to open the HomeSecuritySystem.ino file with the Arduino IDE.
In this file you opened, replace the following lines of code.
``` c
#define FIREBASE_HOST "" // firebase host address
#define FIREBASE_AUTH "" // auth key
#define WIFI_SSID "" // your wifi name
#define WIFI_PASSWORD "" // your wifi pass
const String IFTTT_PATH_NAME = ""; // your ifttt trigger address
```

Replace this line with your country's time zone.
```c
const long utcOffsetInSeconds = 10800; // 60*60*3(Turkey Time Zone);
char daysOfTheWeek[7][12] = {"Pazar", "Pazartesi", "Sali", "Carsamba", "Persembe", "Cuma", "Cumartesi"};
```

NodeMCU and infrared sensor should be connected together as follows before uploading the code. Then you can upload the code to NodeMCU.

![image](https://user-images.githubusercontent.com/44683436/120934812-7d40da00-c708-11eb-834c-bc0ffa854fa4.png)

After then, you need to move the web folder to a location of your choice (or put it on a server) and open index.html.

When an object passes in front of the sensor, this will appear on the com screen. The time information will be saved in firebase realtime database and will appear on the web panel.

![image](https://user-images.githubusercontent.com/44683436/120935052-a3b34500-c709-11eb-9ebf-be69969157c9.png)

![image](https://user-images.githubusercontent.com/44683436/120935095-ccd3d580-c709-11eb-9251-d378cc91bea7.png)

![image](https://user-images.githubusercontent.com/44683436/120934985-6484f400-c709-11eb-884a-1b0639c6021a.png)
