#include "DHT.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2,3); // RX, TX
unsigned int pm1 = 0;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;

int analogPin = A0; //ประกาศตัวแปร ให้ analogPin ของ MQ2(ตรวจจับควัน) 
int val = 0;

#define DHTPIN 2 //ตัวแปรเซนเซอวัดอุณหภูมิ ความชื้น
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.begin(9600);
  while (!Serial) ;
  mySerial.begin(9600);
  dht.begin();
  
}

void loop() {
  Dustsensor();
  MQ2();
  temperature();
}

void Dustsensor(){
  int index = 0;
  char value;
  char previousValue;
   
  while (mySerial.available()) {
  value = mySerial.read();
  if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4d)){
    Serial.println("Cannot find the data header.");
    break;
  }
   
  if (index == 4 || index == 6 || index == 8 || index == 10 || index == 12 || index == 14) {
    previousValue = value;
  }
  else if (index == 5) {
    pm1 = 256 * previousValue + value;
    Serial.print("{ ");
    Serial.print("\"pm1\": ");
    Serial.print(pm1);
    Serial.print(" ug/m3");
    Serial.print(", ");
  }
  else if (index == 7) {
    pm2_5 = 256 * previousValue + value;
    Serial.print("\"pm2_5\": ");
    Serial.print(pm2_5);
    Serial.print(" ug/m3");
    Serial.print(", ");
  }
  else if (index == 9) {
    pm10 = 256 * previousValue + value;
    Serial.print("\"pm10\": ");
    Serial.print(pm10);
    Serial.print(" ug/m3");
  } 
  else if (index > 15) {
    break;
  }
  index++;
  }
  while(mySerial.available()) mySerial.read();
  Serial.println(" }");
  delay(1000);
  }

void MQ2() {
  val = analogRead(analogPin);  //อ่านค่าสัญญาณ analog MQ2
  Serial.print("val = "); // พิมพ์ข้อมความส่งเข้าคอมพิวเตอร์ "val = "
  Serial.println(val); // พิมพ์ค่าของตัวแปร val
}

void temperature() {
  delay(2000); // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
   
  float h = dht.readHumidity();  // Read temperature as Celsius (the default)
  float t = dht.readTemperature(); // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
   
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
  Serial.println(F("Failed to read from DHT sensor!"));
  return;
  }
   
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
   
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% Temperature: "));
  Serial.print(t);
  Serial.print(F(" C "));
  Serial.print(f);
  Serial.print(F(" F Heat index: "));
  Serial.print(hic);
  Serial.print(F(" C "));
  Serial.print(hif);
  Serial.println(F(" F"));
}
