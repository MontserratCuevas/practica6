# Práctica 6 subida de nota

## Código
```c++
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

#define SS_PIN 5 // Pin del ESP8266/ESP32 conectado al SS del MFRC522
#define RST_PIN 21 // Pin del ESP8266/ESP32 conectado al RST del MFRC522
// #define MFRC522_SS_PIN 10  // Pin de Slave Select para el lector RFID
#define MFRC522_RST_PIN 21 // Pin de reset para el lector RFID
// #define SD_SS_PIN 5        // Pin de Slave Select para la tarjeta SD

#define MOSI_PIN 13 // Pin MOSI
#define MISO_PIN 12 // Pin MISO
#define SCK_PIN 11  // Pin SCK

#define WIFI_SSID "AndroidAP8342"
#define WIFI_PASSWORD "12345678"
// #define SERVER_IP "192.168.1.100" // Cambiar a la dirección IP de tu servidor web
#define SERVER_PORT 80

String obtenerHora();
void enviarDatosWeb(String uid, String hora);
void handleRoot();
//void escribirEnArchivo(String uid, String hora);
//void inicializarArchivo();

WebServer server(80); // Object of WebServer(HTTP port, 80 is defult)
MFRC522 mfrc522(SS_PIN, RST_PIN); // Crear instancia del lector RFID
File archivo;

void setup() {
  Serial.begin(115200);
  SPI.begin();       // Inicializar bus SPI
  mfrc522.PCD_Init(); // Inicializar lector RFID

  // Inicializar conexión WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  delay(100);

  // Inicializar archivo de log
  // inicializarArchivo();
}

void loop() {
  // Verificar si hay una tarjeta RFID presente
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Leer UID de la tarjeta RFID
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      uid.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    uid.toUpperCase();

    // Obtener la hora actual
    String hora = obtenerHora();

    // Escribir datos en el archivo de log
    //escribirEnArchivo(uid, hora);

    // Enviar datos a la página web
    enviarDatosWeb(uid, hora);
  }
}

void enviarDatosWeb(String uid, String hora) {
  // Establecer la conexión con el servidor
  WiFiClient client;
  if (!client.connect(WiFi.localIP(), SERVER_PORT)) {
    Serial.println("Error al conectar con el servidor");
    return;
  }

  // Construir el mensaje HTTP
  String mensaje = "GET /enviar_datos?uid=" + uid + "&hora=" + hora + " HTTP/1.1\r\n";
  mensaje += "Host: " + String(WiFi.localIP()) + "\r\n";
  mensaje += "Connection: close\r\n\r\n";

  // Enviar el mensaje HTTP
  client.print(mensaje);
  delay(100); // Esperar un momento para que se envíe completamente

  // Leer y mostrar la respuesta del servidor
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}

void inicializarArchivo() {
  archivo = SD.open("/fichero.log", FILE_WRITE);
  if (!archivo) {
    Serial.println("Error al abrir el archivo");
    return;
  }
  archivo.println("Inicio de registro:");
  archivo.close();
}

void escribirEnArchivo(String uid, String hora) {
  archivo = SD.open("/fichero.log", FILE_APPEND);
  if (!archivo) {
    Serial.println("Error al abrir el archivo");
    return;
  }
  archivo.println("UID: " + uid + " - Hora: " + hora);
  archivo.close();
}

String obtenerHora() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error al obtener la hora");
    return "";
  }

  char hora[20];
  strftime(hora, sizeof(hora), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(hora);
}

String paginaHTML = "<!DOCTYPE html>\
                      <html>\
                      <head>\
                      <meta charset='UTF-8'>\
                      <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
                      <title>Información RFID</title>\
                      <script>\
                      function actualizarDatos(uid, hora) {\
                        document.getElementById('uid').innerHTML = uid;\
                        document.getElementById('hora').innerHTML = hora;\
                      }\
                      </script>\
                      </head>\
                      <body>\
                      <h1>Información RFID</h1>\
                      <p>UID: <span id='uid'></span></p>\
                      <p>Hora: <span id='hora'></span></p>\
                      </body>\
                      </html>";
                      
void handleRoot() {
  server.send(200, "text/html", paginaHTML);
}

```


## Descripción

Este código usa un lector de tarjetar RFID para leer las tarjetas y enviar la información leída a un servidor web a través de WiFi.

Para llevarlo a cabo hemos añadido la libreria MFRC522 para manejar la tarjeta RFID y hemos usado SPI.h para la comunicación SPI, SD.h para la lectura y la escritura en una tarjeta SD, WiFi.h para la conexión a la WiFi, WebServer.h para la creación de un servidor web y time.h para manejar el tiempo.

Se definen los pines y los parámetros de red, acto seguido se inicializan todas las comunicaciones y se establece la conexión WiFi. También se configura el servidor web y se imprime la dirección IP asignada al dispositivo.

El programa verifica si hay una tarjeta RFID, si la detecta lee su UID, obtiene la hora actual, se escriben los datos en un archivo y se envían al servidor web mediante la función enviarDatosWeb(). Y está en bucle por lo que al enviar los datos volverá a ver si ha encontrado o no una tarjeta y repetirá el proceso.

La función **handleRoot()** maneja estas solicitudes que son solicitudes HTTP recibidas por el servidor web y envía una página HTML al cliente que solicita la página principal.

La página HTML muestra el UID de la tarjeta RFID y la hora en la que se ha leído. 

Por el puerto serie muestra lo siguiente:

```
Concectado a WiFi...
Conectado a WiFi!
Dirección IP: 192.168.1.100
HTTP server started

```
Es decir, se muestran mensajes de depuración durante la conexión WiFi conforme se ha conectado y la dirección IP asignada, y un mensaje al iniciar el servidor web con éxito

### Diagrama de flujos

```mermaid
graph TD;
    Inicio[Inicio] --> Verificar_Tarjeta[¿Tarjeta RFID presente?];
    Verificar_Tarjeta --> |Sí| Leer_UID[Leer UID de la tarjeta];
    Leer_UID --> Obtener_Hora[Obtener hora actual];
    Obtener_Hora --> Enviar_Datos_Web[Enviar datos al servidor web];
    Enviar_Datos_Web --> Verificar_Tarjeta;
    Verificar_Tarjeta --> |No| Fin[Fin del programa];

```

## Conclusión

En conclusión se ha creado un sistema que lee tarjetas RFID, registra los UID de estas tarjetas y las horas en las que se han leído, y presenta estra información en una página web creada a través de la WiFi.

