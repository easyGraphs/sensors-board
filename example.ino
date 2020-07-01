#include "EasyGraphsESP8266.h"

#include <BH1750.h>
BH1750 lightMeter;

#include <BMP180advanced.h>
BMP180advanced BMP(BMP180_ULTRALOWPOWER);

#include "ClosedCube_HDC1080.h"
ClosedCube_HDC1080 hdc1080;

#include "Adafruit_CCS811.h"
Adafruit_CCS811 ccs;
float co2, tvoc;

#define DEVICE_TOKEN  "your_device_token"
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

EasyGraphs easygraphs(DEVICE_TOKEN, true);

ADC_MODE (ADC_VCC);

void send() {

  float battery;

  for(int i=0; i < 4; i++){
    battery += ESP.getVcc();
  }

  battery = (battery / 4);

  easygraphs.addParameter("battery", battery);
  easygraphs.addParameter("temprature", hdc1080.readTemperature());
  easygraphs.addParameter("humidity", hdc1080.readHumidity());
  easygraphs.addParameter("co2", co2);
  easygraphs.addParameter("tvoc", tvoc);

  easygraphs.addParameter("lux", lightMeter.readLightLevel());

  easygraphs.addParameter("Pa", BMP.getPressure());
  easygraphs.addParameter("hPa", BMP.getPressure_hPa());
  easygraphs.addParameter("mmHg", BMP.getPressure_mmHg());
  easygraphs.addParameter("inHg", BMP.getPressure_inHg());

  easygraphs.initWIFI(WIFI_SSID, WIFI_PASSWORD);
  easygraphs.publish();
}

void setup() {

  pinMode(14, INPUT_PULLUP); //CLR - button
  pinMode(12, OUTPUT); //CCS811 enable chip pin

  Serial.begin(9600);
  Wire.setClockStretchLimit(500);

  hdc1080.begin(0x40);

  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
  lightMeter.readLightLevel();

  BMP.begin();

  digitalWrite(12, LOW); //CCS811 enable command

  if(!ccs.begin()){
    Serial.println("Failed to start CCS811 sensor!");
  }
  ccs.setDriveMode(CCS811_DRIVE_MODE_1SEC);
}

void loop() {

  if(digitalRead(14) == LOW){
    Serial.println("CLR Button trigger");
  }

  Serial.println("");

  if(ccs.available()){
    if(!ccs.readData()){
      co2  = ccs.geteCO2();
      tvoc = ccs.getTVOC();

      Serial.print("CO2: ");
      Serial.println(co2);

      Serial.print("ppm, TVOC: ");
      Serial.println(tvoc);
    }
    else{
      Serial.println("CCS811 ERROR!");
    }
  }

  Serial.print("temprature:");
  Serial.println(hdc1080.readTemperature());

  Serial.print("humidity:");
  Serial.println(hdc1080.readHumidity());

  Serial.print("lux:");
  Serial.println(lightMeter.readLightLevel());

  Serial.print("Pa:");
  Serial.println(BMP.getPressure());
  Serial.print("hPa:");
  Serial.println(BMP.getPressure_hPa());
  Serial.print("mmHg:");
  Serial.println(BMP.getPressure_mmHg());
  Serial.print("inHg:");
  Serial.println(BMP.getPressure_inHg());

  // You can publish all these sensor data to external API service.
  // send();

  delay(1000);
}
