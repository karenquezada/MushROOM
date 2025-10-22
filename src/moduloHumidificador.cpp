#include <Arduino.h>
#include "DHT.h" 

// 2. Definición de Pines y Tipo de Sensor
#define DHTPIN 14        // Pin para la DATA del sensor DHT22 (usamos GPIO 14)
#define HUMIDIFIER_PIN 18 // Pin para el MOSFET del Humidificador
#define BLOWER_PIN 23    // Pin para el MOSFET del Blower
#define DHTTYPE DHT22    // Tipo de sensor (DHT22 o DHT11)

// 3. Constante de Umbral de Humedad
// Modifica este valor 'X' según tu necesidad (por ejemplo, 60.0% de HR)
const float HUMIDITY_THRESHOLD = 60.0; 

// 4. Inicialización del sensor DHT
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("--- Humidificador Inteligente Iniciado ---");

  // Configurar los pines de control como salidas
  pinMode(HUMIDIFIER_PIN, OUTPUT);
  pinMode(BLOWER_PIN, OUTPUT);

  // Asegurarse de que todo esté apagado al inicio
  digitalWrite(HUMIDIFIER_PIN, LOW);
  digitalWrite(BLOWER_PIN, LOW);

  // Inicializar el sensor DHT
  dht.begin();
  Serial.print("Umbral de Humedad (X): ");
  Serial.print(HUMIDITY_THRESHOLD);
  Serial.println(" %");
}

void loop() {
  // El sensor DHT22 necesita al menos 2 segundos entre lecturas.
  // Un retraso muy corto puede devolver lecturas erróneas.
  delay(2000); 

  // Lectura de la humedad
  float h = dht.readHumidity();

  // Opcional: Lectura de la temperatura
 // float t = dht.readTemperature();

  // 5. Verificación de Lectura
  if (isnan(h)) {
    Serial.println("¡Error! Fallo en la lectura del sensor DHT. Reintentando...");
    return; // Saltar el ciclo si hay error
  }

  // Mostrar datos en el Monitor Serial
  Serial.print("Humedad actual: ");
  Serial.print(h);
  // 6. Lógica de Control (IF/ELSE)
  if (h < HUMIDITY_THRESHOLD) {
    // Si la humedad es MENOR que el umbral (necesita humidificar)
    
    // Encender Humidificador y Blower (HIGH en el pin del MOSFET)
    digitalWrite(HUMIDIFIER_PIN, HIGH);
    digitalWrite(BLOWER_PIN, HIGH); 
    
    Serial.println("-> ACTIVADO: Humedad por debajo del umbral. Humidificador y Blower ON.");

  } else {
    // Si la humedad es IGUAL o MAYOR que el umbral (ya está húmedo)

    // Apagar Humidificador y Blower (LOW en el pin del MOSFET)
    digitalWrite(HUMIDIFIER_PIN, LOW);
    digitalWrite(BLOWER_PIN, LOW); 
    
    Serial.println("-> DESACTIVADO: Humedad alcanzada. Humidificador y Blower OFF.");
  }
}