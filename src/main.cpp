#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include "DHT.h"

//··· CONFIGURACIONES y VARIABLES ···
//Del task screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#define CLK 27 // pin A
#define DT  26 // pin B
#define SW  25 // botón del encoder (pulled-up)

struct Menu {
  const char* nombre;
  const Menu* submenus;
  int numSubmenus;
};

// ---------- SUBMENÚS ----------
const Menu sub_ControlManual[] = {
  {"<- ATRAS", NULL, 0},
  {"Luz", NULL, 0},
  {"Ventilacion", NULL, 0},
  {"Humidificador", NULL, 0},
  {"Calefaccion", NULL, 0}
};

const Menu sub_Perfiles[] = {
  {"<- ATRAS", NULL, 0},
  {"Seleccionar Perfil", NULL, 0},
  {"Editar Parametros", NULL, 0},
  {"Ver Perfil Actual", NULL, 0}
};

const Menu sub_Configuracion[] = {
  {"<- ATRAS", NULL, 0},
  {"Sensores", NULL, 0},
  {"Red WiFi", NULL, 0},
  {"Fecha/Hora", NULL, 0}
};

// Menú principal
const Menu menuPrincipal[] = {
  {"Control Manual", sub_ControlManual, sizeof(sub_ControlManual)/sizeof(Menu)},
  {"Perfiles", sub_Perfiles, sizeof(sub_Perfiles)/sizeof(Menu)},
  {"Configuracion", sub_Configuracion, sizeof(sub_Configuracion)/sizeof(Menu)}
};

// ========================
// Variables de menú
// ========================
const Menu* menuActual = menuPrincipal;
const Menu* menuPadre = NULL;
int menuIndex = 0;
int totalItems = sizeof(menuPrincipal)/sizeof(Menu);

volatile float temperatura = 0.0;
volatile int humedad = 0.0;

// ========================
// Variables para encoder y botón (debounce)
// ========================
int lastCLK = 0;
unsigned long lastEncoderMillis = 0;
const unsigned long encoderDebounceMs = 20;

int lastButtonReading = HIGH;
int buttonState = HIGH;
unsigned long lastButtonDebounceTime = 0;
const unsigned long buttonDebounceMs = 50;

// ========================
// Bitmap pequeño: hongo (8x8)
// ========================
const unsigned char epd_bitmap_pixil_frame_0 [] PROGMEM = { 0xff, 0xff, 0xe7, 0xd3, 0xa1, 0x89, 0xe7, 0xe7 };

// Prototipos
void mostrarMenu();
void dhtTask(void * parameter);
void onSelect();

//Del task bot telegram
// --- Configuración WiFi ---
const char* ssid = "TP-Link_EB99";
const char* password = "cleito2014";

// --- Configuración Bot Telegram ---
#define BOTtoken "8019139155:AAE6T7SerquzDK682b6oW5lbaFhytCc9RuQ"  // Token de BotFather
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOTtoken, secured_client);

const char* chat_ids[] = {
  "6154183820",  // tu chat ID
  "188539796"    // otro usuario
};
const int num_chat_ids = sizeof(chat_ids) / sizeof(chat_ids[0]);

unsigned long botRequestDelay = 1000; // ms

//del dht
#define DHTPIN 4        // Pin GPIO de la ESP32 donde conectas la DATA del DHT22
#define DHTTYPE DHT22   // Tipo de sensor: DHT11 o DHT22

DHT dht(DHTPIN, DHTTYPE);


//··· FUNCIONES ···
// --- Función para enviar notificación a todos ---
void enviarNotificacionATodos(const String& mensaje) {
  for (int i = 0; i < num_chat_ids; i++) {
    bot.sendMessage(chat_ids[i], mensaje, "");
  }
}

// --- Task de menú ---
void menuTask(void * parameter) {
  for(;;) { // loop infinito de la task
    // --------- Lectura encoder (giro) ----------
    int clkState = digitalRead(CLK);
    int dtState = digitalRead(DT);

    if (clkState != lastCLK) {
        if (dtState != clkState) {
            menuIndex++;
            Serial.println("Encoder girado: +1");
        } else {
            menuIndex--;
            Serial.println("Encoder girado: -1");
        }

        // Wrap-around
        if (menuIndex < 0) menuIndex = totalItems - 1;
        if (menuIndex >= totalItems) menuIndex = 0;

        mostrarMenu();
    }
    lastCLK = clkState;

    // --------- Lectura botón ----------
    int reading = digitalRead(SW);
    if (reading != lastButtonReading) {
      lastButtonDebounceTime = millis();
    }

    if ((millis() - lastButtonDebounceTime) > buttonDebounceMs) {
      if (reading != buttonState) {
        buttonState = reading;
        if (buttonState == LOW) { // botón presionado
          Serial.print("Botón presionado en opción: ");
          Serial.println(menuActual[menuIndex].nombre);
          onSelect();
        }
      }
    }
    lastButtonReading = reading;

    vTaskDelay(10 / portTICK_PERIOD_MS); // pequeña espera para liberar CPU
  }
}

// --- Task de Telegram ---
void telegramTask(void * parameter) {
  unsigned long lastTimeBotRan = 0;

  while (true) {
    if (millis() - lastTimeBotRan > botRequestDelay) {
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
          bot.sendMessage(chat_id, "Luces encendidas 💡", "");
        }
        if (text == "/luces_off") {
          bot.sendMessage(chat_id, "Luces apagadas", "");
        }

        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      }

      lastTimeBotRan = millis();
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // pequeña espera para liberar CPU
  }
}

//el dhtTask
void dhtTask(void * parameter) {
  for (;;) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

     if (!isnan(h) && !isnan(t)) {
      humedad = h;
      temperatura = t;
    } else {
      Serial.println(F("Error al leer DHT22"));
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  lastCLK = digitalRead(CLK);

  dht.begin(); // Iniciar sensor DHT22

  // Inicializar pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 no encontrada"));
    for(;;);
  }
  display.clearDisplay();
  display.display();

  mostrarMenu();
  // --- Conexión WiFi ---
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //Serial.println("\nWiFi conectado");

  secured_client.setInsecure(); // Telegram HTTPS, aceptar cualquier certificado

  // Notificación inicial
  enviarNotificacionATodos("Incubadora conectada ✅");

  // --- Crear Task para el bot ---
//   xTaskCreate(
//     telegramTask,    // función de la task
//     "Telegram Task", // nombre
//     4096,            // tamaño stack en bytes
//     NULL,            // parámetro
//     1,               // prioridad
//     NULL             // handle
//   );

    // Crear la task del menú
  xTaskCreate(
    menuTask,
    "Menu Task",
    4096,
    NULL,
    1,
    NULL
  );

    // Crear la task del DHT22
    xTaskCreate(
    dhtTask,
    "DHT Task", 
    2048,
    NULL,
    1,
    NULL
    );
}

void loop() {
    int clkState = digitalRead(CLK);
    int dtState = digitalRead(DT);
    if (clkState != lastCLK) {
        if (dtState != clkState) menuIndex++;
        else menuIndex--;
        if (menuIndex < 0) menuIndex = totalItems-1;
        if (menuIndex >= totalItems) menuIndex = 0;
        mostrarMenu();
    }
    lastCLK = clkState;

  int reading = digitalRead(SW);
  if (reading != lastButtonReading) {
    lastButtonReading = reading;
    if (reading == LOW) {
      Serial.println(menuActual[menuIndex].nombre);
    }
  }

  vTaskDelay(10 / portTICK_PERIOD_MS);
}

void onSelect() {
  const Menu* seleccionado = &menuActual[menuIndex];

  Serial.print("Seleccionado: ");
  Serial.println(seleccionado->nombre);

  // Caso especial: "<- Volver"
  if (menuPadre != NULL && strcmp(seleccionado->nombre, "<- ATRAS") == 0) {
    menuActual = menuPadre;
    totalItems = menuPadre == menuPrincipal ? sizeof(menuPrincipal)/sizeof(Menu) : menuPadre->numSubmenus;
    menuPadre = NULL;
    menuIndex = 0;
    mostrarMenu();
    return;
  }

  if (seleccionado->numSubmenus > 0 && seleccionado->submenus != NULL) {
    menuPadre = menuActual;
    menuActual = seleccionado->submenus;
    totalItems = seleccionado->numSubmenus;
    menuIndex = 0;
    mostrarMenu();
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25);
    display.print("Ejecutando ");
    display.print(seleccionado->nombre);
    display.display();
    delay(700);

    if (menuPadre != NULL) {
      menuActual = menuPrincipal;
      totalItems = sizeof(menuPrincipal)/sizeof(Menu);
      menuPadre = NULL;
      menuIndex = 0;
    }
    mostrarMenu();
  }
}

// ---------------------
// Mostrar menú actual
// ---------------------
void mostrarMenu() {
  display.clearDisplay();

  // Encabezado
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Temperatura: ");
  display.print(temperatura, 1);
  display.print(" C");

  display.setCursor(0, 14);
  display.print("Humedad: ");
  display.print(humedad, 1);
  display.print(" %");

  // Línea separadora con bitmap
  int bmpWidth = 8;
  int bmpHeight = 8;
  int yLine = 14 + 8;
  for (int x = 0; x < SCREEN_WIDTH; x += bmpWidth) {
      display.drawBitmap(x, yLine, epd_bitmap_pixil_frame_0, bmpWidth, bmpHeight, SSD1306_WHITE);
  }

  // Menú debajo de la línea separadora
  int baseY = yLine + bmpHeight + 2;
  for (int i = 0; i < totalItems; i++) {
      display.setCursor(2, baseY + i * 8);
      if (i == menuIndex) display.print("> ");
      else display.print("  ");
      display.print(menuActual[i].nombre);
  }

  display.display();
}