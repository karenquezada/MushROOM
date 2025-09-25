// #include <Arduino.h>
// #define blowerPin 23
// #define sensorPin 34        // Pin analógico para el sensor
// #define freq 25000          // frecuencia PWM (25kHz para que no suene)
// #define channel 0
// #define resolution 8        // 0-255
// #define SENSOR_THRESHOLD 28 // Valor umbral para encender el ventilador

// void setup() {
//   ledcSetup(channel, freq, resolution);
//   ledcAttachPin(blowerPin, channel);
//   pinMode(sensorPin, INPUT);
// }

// void loop() {
//   int sensorValue = analogRead(sensorPin);

//   if (sensorValue >= SENSOR_THRESHOLD) {
//     // Encender el ventilador (acelerar y desacelerar)
//     for (int duty = 0; duty <= 255; duty += 5) {
//       ledcWrite(channel, duty);
//       delay(50);
//       // Si el sensor baja del umbral, apagar inmediatamente
//       if (analogRead(sensorPin) < SENSOR_THRESHOLD) {
//         break;
//       }
//     }
//     for (int duty = 255; duty >= 0; duty -= 5) {
//       ledcWrite(channel, duty);
//       delay(50);
//       if (analogRead(sensorPin) < SENSOR_THRESHOLD) {
//         break;
//       }
//     }
//   } else {
//     // Apagar el ventilador
//     ledcWrite(channel, 0);
//     delay(100);
//   }
// }
