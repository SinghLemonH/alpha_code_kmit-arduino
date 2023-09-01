#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 3  //DHT digital pin2
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

const int ledPin = 5;
const int buzzerPin = 4;
const int dustSensorPin = A0;
const float dustThreshold = 0.5;

const int AOUTpin=A1;//the AOUT pin of the CO sensor goes into analog pin A0 of the arduino
int value;

const char* ssid = "Sing za 2.4 G";
const char* password = "75446600";

const char* lineToken = "BH2AEtoWx1RRTD8RWfVJ04FgI9dn50j0i3MNnsVXBWb";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

  dht.begin();
  pinMode(buzzerPin, OUTPUT); // Set buzzer pin as OUTPUT
  pinMode(ledPin, OUTPUT);

void loop() {
  onDustChange();
  onTempChange();
  onHumendityChange();
  onCoChange();
  delay(500);
}

void sendLineNotify(const char* message) {
  WiFiClientSecure client;

  if (client.connect("notify-api.line.me", 443)) {
    String body = "message=";
    body += String(message);

    client.println("POST /api/notify HTTP/1.1");
    client.println("Host: notify-api.line.me");
    client.println("Authorization: Bearer " + String(lineToken));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(body.length()));
    client.println();
    client.println(body);

    Serial.println("Line Notify message sent");
  } else {
    Serial.println("Error connecting to Line Notify server");
  }

  client.stop();
}

void onTempChange()  {
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  //temp = t;
}

void onHumendityChange() {
  float h = dht.readHumidity();
  Serial.print(F("Humidity: "));
  Serial.print(h);
  //humendity = h;
}
/*
  Since Co3 is READ_WRITE variable, onCo3Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onCoChange() {
  Serial.println(value);
  value= analogRead(AOUTpin);//reads the analaog value from the CO sensor's AOUT pin
  float voltage = value * (5.0 / 1023.0);
  float ppm = (voltage - 0.4) * 100 / 0.6;
  //co = ppm;
  
  Serial.print("CO Concentration: ");
  Serial.print(ppm);
  Serial.println(" ppm");

   if (ppm >= 400) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    delay(1500); 
    digitalWrite(buzzerPin, LOW);
  } else {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }
  
  delay(500);
}

void onDustChange() {
  int sensorValue = analogRead(dustSensorPin); 
  float voltage = sensorValue * (5.0 / 1023.0);

  float dustDensity = 0.17 * voltage - 0.1;
  if (dustDensity > dustThreshold) {
    
    digitalWrite(buzzerPin, HIGH);
    sendLineNotify("Waring dust");
    delay(2500);
  } else {
    
    digitalWrite(buzzerPin, LOW);
  }
  
 
  Serial.print("Dust Density: ");
  Serial.print(dustDensity);
  Serial.println(" pcs/0.01cf");
  //dust = dustDensity;
  
  delay(500);
}
