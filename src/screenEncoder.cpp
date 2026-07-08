// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// // ========================
// // Configuración OLED
// // ========================
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// // ========================
// // Pines del Rotary Encoder
// // ========================
// #define CLK 27 // pin A
// #define DT  26 // pin B
// #define SW  25 // botón del encoder (pulled-up)

// // ========================
// // Estructura de menú
// // ========================
// struct Menu {
//   const char* nombre;
//   const Menu* submenus;
//   int numSubmenus;
// };

// // ---------- SUBMENÚS ----------
// const Menu sub_ControlManual[] = {
//   {"<- ATRAS", NULL, 0},
//   {"Luz", NULL, 0},
//   {"Ventilacion", NULL, 0},
//   {"Humidificador", NULL, 0},
//   {"Calefaccion", NULL, 0}
// };

// const Menu sub_Perfiles[] = {
//   {"<- ATRAS", NULL, 0},
//   {"Seleccionar Perfil", NULL, 0},
//   {"Editar Parametros", NULL, 0},
//   {"Ver Perfil Actual", NULL, 0}
// };

// const Menu sub_Configuracion[] = {
//   {"<- ATRAS", NULL, 0},
//   {"Sensores", NULL, 0},
//   {"Red WiFi", NULL, 0},
//   {"Fecha/Hora", NULL, 0}
// };

// // Menú principal
// const Menu menuPrincipal[] = {
//   {"Control Manual", sub_ControlManual, sizeof(sub_ControlManual)/sizeof(Menu)},
//   {"Perfiles", sub_Perfiles, sizeof(sub_Perfiles)/sizeof(Menu)},
//   {"Configuracion", sub_Configuracion, sizeof(sub_Configuracion)/sizeof(Menu)}
// };

// // ========================
// // Variables de menú
// // ========================
// const Menu* menuActual = menuPrincipal;
// const Menu* menuPadre = NULL;
// int menuIndex = 0;
// int totalItems = sizeof(menuPrincipal)/sizeof(Menu);

// // ========================
// // Variables simuladas de sensores
// // ========================
// float temperatura = 24.3;
// int humedad = 91;

// // ========================
// // Variables para encoder y botón (debounce)
// // ========================
// int lastCLK = 0;
// unsigned long lastEncoderMillis = 0;
// const unsigned long encoderDebounceMs = 20;

// int lastButtonReading = HIGH;
// int buttonState = HIGH;
// unsigned long lastButtonDebounceTime = 0;
// const unsigned long buttonDebounceMs = 50;

// // ========================
// // Bitmap pequeño: hongo (8x8)
// // ========================
// const unsigned char epd_bitmap_pixil_frame_0 [] PROGMEM = { 0xff, 0xff, 0xe7, 0xd3, 0xa1, 0x89, 0xe7, 0xe7 };

// // Prototipos
// void mostrarMenu();
// void onSelect();

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("Setup terminado");
//   pinMode(CLK, INPUT_PULLUP);
//   pinMode(DT, INPUT_PULLUP);
//   pinMode(SW, INPUT_PULLUP);

//   lastCLK = digitalRead(CLK);

//   // Inicializar pantalla OLED
//   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
//     Serial.println(F("SSD1306 no encontrada"));
//     for(;;);
//   }
//   display.clearDisplay();
//   display.display();

//   // Mostrar la pantalla inicial
//   mostrarMenu();
// }

// void loop() {
//   // --------- Lectura encoder (giro) con debounce muy simple ----------
// int clkState = digitalRead(CLK);
// int dtState = digitalRead(DT);

// if (clkState != lastCLK) {
//     if (dtState != clkState) {
//         menuIndex++;
//         Serial.println("Encoder girado: +1");
//     } else {
//         menuIndex--;
//         Serial.println("Encoder girado: -1");
//     }

//     // Wrap-around
//     if (menuIndex < 0) menuIndex = totalItems - 1;
//     if (menuIndex >= totalItems) menuIndex = 0;

//     mostrarMenu();
// }
// lastCLK = clkState;


//   // --------- Lectura botón (debounce clásico) ----------
//   int reading = digitalRead(SW);
//   if (reading != lastButtonReading) {
//     lastButtonDebounceTime = millis();
//   }

//   if ((millis() - lastButtonDebounceTime) > buttonDebounceMs) {
//     if (reading != buttonState) {
//       buttonState = reading;
//       if (buttonState == LOW) { // botón presionado (activo LOW)
//         Serial.print("Botón presionado en opción: ");
//         Serial.println(menuActual[menuIndex].nombre);
//         onSelect();
//       }
//     }
//   }
//   lastButtonReading = reading;
// }

// // ---------------------
// // Acción al seleccionar un ítem de menú
// // ---------------------
// void onSelect() {
//   const Menu* seleccionado = &menuActual[menuIndex];

//   Serial.print("Seleccionado: ");
//   Serial.println(seleccionado->nombre);

//   // Caso especial: "<- Volver"
//   // -----------------------
//   if (menuPadre != NULL && strcmp(seleccionado->nombre, "<- ATRAS") == 0) {
//     menuActual = menuPadre;
//     totalItems = menuPadre == menuPrincipal ? sizeof(menuPrincipal)/sizeof(Menu) : menuPadre->numSubmenus;
//     menuPadre = NULL;
//     menuIndex = 0; // opcional: puedes restaurar índice previo
//     mostrarMenu();
//     return;
//   }

//   if (seleccionado->numSubmenus > 0 && seleccionado->submenus != NULL) {
//     // 👉 Entrar al submenú
//     Serial.println("Entrando a submenú...");
//     menuPadre = menuActual;
//     menuActual = seleccionado->submenus;
//     totalItems = seleccionado->numSubmenus;
//     menuIndex = 0;
//     mostrarMenu();
//   } else {
//     // 👉 No hay submenús: acción o volver atrás
//     Serial.println("Ejecutando acción del ítem...");
//     display.clearDisplay();
//     display.setTextSize(1);
//     display.setTextColor(SSD1306_WHITE);
//     display.setCursor(10, 25);
//     display.print("Ejecutando ");
//     display.print(seleccionado->nombre);
//     display.display();
//     delay(700);

//     // Volver al menú anterior si existe
//     if (menuPadre != NULL) {
//       Serial.println("Volviendo al menú principal...");
//       menuActual = menuPrincipal;
//       totalItems = sizeof(menuPrincipal)/sizeof(Menu);
//       menuPadre = NULL;
//       menuIndex = 0;
//     }
//     mostrarMenu();
//   }
// }


// // ---------------------
// // Mostrar menú actual
// // ---------------------
// void mostrarMenu() {
//   display.clearDisplay();


//   // Encabezado
//   display.setTextSize(1);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(0, 0);
//   display.print("Temperatura: ");
//   display.print(temperatura, 1);
//   display.print(" C");

//   display.setCursor(0, 14);
//   display.print("Humedad: ");
//   display.print(humedad);
//   display.print(" %");
//     int bmpWidth = 8;
//   int bmpHeight = 8;
//   int yLine = 14 + 8; // misma Y que antes de la barra, ajusta si quieres
//   for (int x = 0; x < SCREEN_WIDTH; x += bmpWidth) {
//       display.drawBitmap(x, yLine, epd_bitmap_pixil_frame_0, bmpWidth, bmpHeight, SSD1306_WHITE);
//   }

//   // ------------------------
//   // Menú debajo de la línea separadora
//   // ------------------------
//   int baseY = yLine + bmpHeight + 2; // debajo de la línea de bitmaps
//   for (int i = 0; i < totalItems; i++) {
//       display.setCursor(2, baseY + i * 8);
//       if (i == menuIndex) {
//           display.print("> ");
//       } else {
//           display.print("  ");
//       }
//       display.print(menuActual[i].nombre);
//   }

//   display.display();
// }
