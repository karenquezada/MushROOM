// #include "DHT.h"

// // Configuración del pin y tipo de sensor
// #define DHTPIN 4       // Pin GPIO de la ESP32 donde conectas la DATA del DHT22
// #define DHTTYPE DHT22  // Tipo de sensor: DHT11 o DHT22

// DHT dht(DHTPIN, DHTTYPE);

// void setup() {
//   Serial.begin(115200);
//   Serial.println(F("Iniciando DHT22..."));
//   dht.begin();
// }

// void loop() {
//   // Espera entre lecturas
//   delay(2000);

//   float h = dht.readHumidity();    // Humedad
//   float t = dht.readTemperature(); // Temperatura en °C
//   float f = dht.readTemperature(true); // Temperatura en °F

//   // Verificar si la lectura es válida
//   if (isnan(h) || isnan(t)) {
//     Serial.println(F("Error al leer el sensor DHT22!"));
//     return;
//   }

//   Serial.print(F("Humedad: "));
//   Serial.print(h);
//   Serial.print(F("%  Temperatura: "));
//   Serial.print(t);
//   Serial.print(F("°C  / "));
//   Serial.print(f);
//   Serial.println(F("°F"));
// }

