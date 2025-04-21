#include <RH_ASK.h>  
#include <SPI.h>     
#include <SoftwareSerial.h>  

// Bike Unit Pins
int motorRelay = 7;    
int RF_RX = 9;         
int gsmTX = 2, gsmRX = 3;  
int gpsTX = 4, gpsRX = 5;  

RH_ASK rf_driver;
SoftwareSerial gsm(gsmTX, gsmRX);
SoftwareSerial gps(gpsTX, gpsRX);

void setup() {
  pinMode(motorRelay, OUTPUT);
  digitalWrite(motorRelay, LOW);  // Bike OFF by default

  Serial.begin(9600);
  rf_driver.init();
  gsm.begin(9600);
  gps.begin(9600);
}

void loop() {
  uint8_t buf[30];
  uint8_t buflen = sizeof(buf);

  // 📡 Receive Data from Helmet Unit
  if (rf_driver.recv(buf, &buflen)) {
    buf[buflen] = '\0';  
    String receivedData = String((char*)buf);
    Serial.print("Received Data: ");
    Serial.println(receivedData);

    // 🛠️ Extract Values (Format: H:1 A:250 D:45)
    int helmetWorn = -1, alcoholLevel = -1, distance = -1, accidentdetected=-1;
    
    // 🚀 Condition to Start the Bike
    if (sscanf(receivedData.c_str(), "H:%d A:%d D:%d C:%d", &helmetWorn, &alcoholLevel, &distance, &accidentdetected) == 4){
      Serial.println("Parsed-Helmet: ");
      Serial.println(helmetWorn);
      Serial.println("Parsed-Alcohol: ");
      Serial.println(alcoholLevel);
      Serial.println("Parsed-Distance: ");
      Serial.println(distance);

    }
    if (helmetWorn == 1 && alcoholLevel < 600) {
      digitalWrite(motorRelay, HIGH);
      Serial.println("✅ Bike Started");
    } else {
      digitalWrite(motorRelay, LOW);
      Serial.println("❌ Bike Cannot Start - Safety Conditions Not Met");
    }

    // 🚨 Crash Detection - Stop Bike & Send Alert
    if (accidentdetected == HIGH) {
      digitalWrite(motorRelay, LOW);
      Serial.println("🚨 CRASH DETECTED! Sending Emergency Alert...");
      sendEmergencyAlert();
    }
    else{
      Serial.println("Failed to parse data. Ignoring input");
      digitalWrite(motorRelay, LOW);
    }
  }
}

// 📡 Send Emergency SMS with GPS Location
void sendEmergencyAlert() {
  String gpsLocation = getGPSLocation();
  
  gsm.println("AT+CMGF=1");  // Set SMS Mode
  delay(1000);
  gsm.println("AT+CMGS=\"+918848281200\"");  // Replace with emergency contact number
  delay(1000);
  gsm.print("🚨 Accident Detected! GPS Location: ");
  gsm.print(gpsLocation);
  delay(1000);
  gsm.write(26);  // End SMS
  Serial.println("📩 Emergency SMS Sent!");
}

// 🌍 Get GPS Location
void loop() {
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    Serial.write(c); // Optional: see raw GPS data
    if (gps.encode(c)) {
      if (gps.location.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        Serial.print("📍 Latitude: ");
        Serial.println(latitude, 6);
        Serial.print("📍 Longitude: ");
        Serial.println(longitude, 6);
      } else {
        Serial.println("🚫 Location data invalid");
      }
    }
  }
}

