// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>
// #include <ArduinoJson.h>

// const char* ssid = "TU_WIFI";
// const char* password = "CLAVE_WIFI";

// #define BOTtoken "8019139155:AAE6T7SerquzDK682b6oW5lbaFhytCc9RuQ"  // Token de BotFather
// //#define CHAT_ID "6154183820"  // Tu ID personal (para recibir mensajes)
// String chat_id = String(bot.messages[0].chat_id);

// // Crear clientes
// WiFiClientSecure client;
// UniversalTelegramBot bot(BOTtoken, client);

// unsigned long lastTimeBotRan = 0;
// const int botRequestDelay = 1000; // ms

// void setup() {
//   Serial.begin(115200);
//   WiFi.begin(ssid, password);

//   // Conexión WiFi
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi conectado");

//   client.setInsecure(); // Telegram usa HTTPS, pero aceptamos cualquier certificado
//   bot.sendMessage(CHAT_ID, "Incubadora conectada ✅", "");
// }

// void loop() {
//   if (millis() - lastTimeBotRan > botRequestDelay)  {
//     int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

//     while(numNewMessages) {
//       Serial.println("Mensaje recibido");

//       String text = bot.messages[0].text;
//       String chat_id = String(bot.messages[0].chat_id);

//       if (text == "/start") {
//         bot.sendMessage(chat_id, "Hola 🍄 Soy tu incubadora", "");
//       }
//       if (text == "/estado") {
//         bot.sendMessage(chat_id, "Temp: 23.5°C\nHumedad: 90%", "");
//       }
//       if (text == "/luces_on") {
//         // Aquí encenderías LEDs
//         bot.sendMessage(chat_id, "Luces encendidas 💡", "");
//       }
//       if (text == "/luces_off") {
//         // Aquí apagarías LEDs
//         bot.sendMessage(chat_id, "Luces apagadas", "");
//       }

//       numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//     }
//     lastTimeBotRan = millis();
//   }
// }
