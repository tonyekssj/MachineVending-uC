#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include "HTTP.h"

#define pinLed 5

const char* ssid = "IZZI-90C0";
const char* password = "5095512890C0";

const char* serverIP = "192.168.0.22"; // Dirección IP del servidor
const int serverPort = 80; // Puerto del servidor

const char* fail = "FAIL";
const char* ok = "OK";

const char* mqttServer = "192.168.0.37";
const int mqttPort = 1883;

WebServer server(80);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


bool estadoMaquina() {
  bool estado;

  pinMode(pinLed, INPUT);
  Serial.println(digitalRead(pinLed));

  ((digitalRead(pinLed)) == HIGH) ? estado = true : estado = false;

  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, LOW);

  return estado;
}


void setup() {
  Serial.begin(115200);
  pinMode(pinLed, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conexión WiFi establecida");
  Serial.println(WiFi.localIP());

  server.on("/verificarEstado", HTTP_POST, handleVerificarEstado);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado");

  //mqttClient.setServer(mqttServer, mqttPort);
  //mqttClient.setCallback(callback);
}

void loop() {
  server.handleClient();
}
