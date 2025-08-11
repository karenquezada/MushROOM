#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <FastLED.h>

const char* ssid = "TP-Link_EB99";
const char* password = "cleito2014";

#define BOTtoken "8019139155:AAE6T7SerquzDK682b6oW5lbaFhytCc9RuQ"  // Token de BotFather
// Crear cliente seguro y bot
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOTtoken, secured_client);


//#define CHAT_ID "6154183820"  // Tu ID personal (para recibir mensajes)
String chat_id = String(bot.messages[0].chat_id);

const char* chat_ids[] = {
  "6154183820",    // Tu chat ID
  "188539796",    // pepe
};

const int num_chat_ids = sizeof(chat_ids) / sizeof(chat_ids[0]);

unsigned long lastTimeBotRan = 0;
const int botRequestDelay = 1000; // ms

void enviarNotificacionATodos(const String& mensaje) {
  for (int i = 0; i < num_chat_ids; i++) {
    bot.sendMessage(chat_ids[i], mensaje, "");
  }
} 

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Conexión WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  enviarNotificacionATodos("Incubadora conectada ✅");


  secured_client.setInsecure(); // Telegram usa HTTPS, pero aceptamos cualquier certificado
  bot.sendMessage(chat_id, "Incubadora conectada ✅", "");
}

void loop() {
  if (millis() - lastTimeBotRan > botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("Mensaje recibido");

      String text = bot.messages[0].text;
      String chat_id = String(bot.messages[0].chat_id);

      if (text == "/start") {
        bot.sendMessage(chat_id, "Hola 🍄 Soy tu incubadora", "");
      }
      if (text == "/estado") {
        bot.sendMessage(chat_id, "Temp: 23.5°C\nHumedad: 90%", "");
      }
      if (text == "/luces_on") {
        // Aquí encenderías LEDs
        bot.sendMessage(chat_id, "Luces encendidas 💡", "");
      }
      if (text == "/luces_off") {
        // Aquí apagarías LEDs
        bot.sendMessage(chat_id, "Luces apagadas", "");
      }

      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
