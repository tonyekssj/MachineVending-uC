#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>

#define pinLed 21
#define pinEstado 5
#define buttonMoneda 18
#define buttonCompletar 19

int buttonState1 = LOW;
int lastButtonState1 = LOW;

int buttonState2 = LOW;
int lastButtonState2 = LOW;

bool monedaInsertada = false;

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;

const char* ssid = "IZZI-90C0";
const char* password = "5095512890C0";

const char* serverIP = "192.168.0.13"; // Dirección IP del servidor
const int serverPort = 80; // Puerto del servidor

const char* fail = "FAIL";
const char* ok = "OK";

const char* serverBroker = "broker.emqx.io";
int port = 1883;

WiFiClient wifiClient;
WebServer server(80);

PubSubClient mqttClient(wifiClient);

void reconnect() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESPServer")) 
      mqttClient.subscribe("detergentes");  
  }
}

bool estadoMaquina() {
  bool estado;

  Serial.println(digitalRead(pinEstado));

  ((digitalRead(pinEstado)) == HIGH) ? estado = true : estado = false;

  return estado;
}

void sendRequestGET() {
  WiFiClient client;
  if (client.connect(serverIP, serverPort)) {
    Serial.println("Conexión establecida con el servidor");
    bool estado = estadoMaquina();
    if (estado) {
      // Enviar una solicitud GET al servidor para escribir en el archivo
      String request = String("GET /machine_vending/admin/escribir_archivo.php?data=" + String(ok) + " HTTP/1.1\r\n") +
                       "Host: " + String(serverIP) + "\r\n\r\n";
      client.print(request);
    } else {
      // Enviar una solicitud GET al servidor para escribir en el archivo
      String request = String("GET /machine_vending/admin/escribir_archivo.php?data=" + String(fail) + " HTTP/1.1\r\n") +
                       "Host: " + String(serverIP) + "\r\n\r\n";
      client.print(request);
    }
    // Esperar la respuesta del servidor
    while (client.connected()) {
      if (client.available()) {
        String response = client.readStringUntil('\r');
        Serial.println(response);
      }
    }
  }
}

void handleVerificarEstado() {
  String message = server.arg("ADMIN"); // Obtén el valor del parámetro "mensaje" enviado en la solicitud POST
  Serial.print("Estado de Maquina");
  sendRequestGET();

  server.sendHeader("Access-Control-Allow-Origin", "*"); // Permite solicitudes desde cualquier origen
  server.send(200, "text/plain", "OK"); // Responde con un mensaje de "OK" al cliente
}

void handleCloro() {
  String message = server.arg("VENTA"); // Obtén el valor del parámetro "mensaje" enviado en la solicitud POST
  Serial.print("Estado de Maquina: ");
  Serial.println(message);

  server.sendHeader("Access-Control-Allow-Origin", "*"); // Permite solicitudes desde cualquier origen
  server.send(200, "text/plain", "OK"); // Responde con un mensaje de "OK" al cliente
}

void handleNotFound() {
  server.send(404, "text/plain", "404 Not Found");
}

void setup() {
  Serial.begin(115200);
  pinMode(pinLed, OUTPUT);
  
  pinMode(pinEstado, INPUT);
  pinMode( buttonMoneda, INPUT_PULLUP);
  pinMode( buttonCompletar, INPUT_PULLUP);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conexión WiFi establecida");
  Serial.println(WiFi.localIP());

  mqttClient.setServer(serverBroker, port);
  Serial.println("Servidor MQTT iniciado");

  server.on("/verificarEstado", HTTP_POST, handleVerificarEstado);
  server.on("/cloroCompra", HTTP_POST, handleCloro);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado");
  
}

void loop() {
  int reading1 = digitalRead(buttonMoneda);
  int reading2 = digitalRead(buttonCompletar);
  
  server.handleClient();

  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  
  if (reading1 != lastButtonState1) {
    lastDebounceTime1 = millis();
  }

  if (reading2 != lastButtonState2) {
    lastDebounceTime2 = millis();
  }

  if ((millis() - lastDebounceTime1) > 50) {
    if (reading1 != buttonState1) {
      buttonState1 = reading1;

      if (buttonState1 == LOW && monedaInsertada==false) {
        digitalWrite(pinLed, HIGH);
        monedaInsertada=true;
        Serial.println("Se ha insertado [MONEDA]");
      }
    }
  }

  if ((millis() - lastDebounceTime2) > 50) {
    if (reading2 != buttonState2) {
      buttonState2 = reading2;

      if (buttonState2 == LOW && monedaInsertada==true ) {
        digitalWrite(pinLed, LOW);
        monedaInsertada=false;
        mqttClient.publish("detergentes","cloro");
        Serial.println("Pago completado");
        Serial.println();
      }
    }
  }

  lastButtonState1 = reading1;
  lastButtonState2 = reading2;

  delay(100);
  
}
