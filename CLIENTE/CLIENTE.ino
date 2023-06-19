#include <WiFi.h>
#include <PubSubClient.h>

#define pinLed 2

const char* ssid = "IZZI-90C0";
const char* password = "5095512890C0";

char *server = "broker.emqx.io";
int port = 1883;


String resultMQTT = "";

WiFiServer wifiServer(1883);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");

  char payload_string[length + 1];
  memcpy(payload_string, payload, length);
  payload_string[length] = '\0';

  String resultMQTT = String(payload_string);

  Serial.println();
  Serial.print("Tipo de detergente: ");
  Serial.println(resultMQTT);
  
  // Eliminar caracteres no visibles
  resultMQTT.trim();
  Serial.println();
  
  if (resultMQTT.equalsIgnoreCase("cloro")) {
    int i = 0;
    digitalWrite(pinLed, HIGH);
    Serial.println("Llenando");
    while (i < 40) {
      Serial.print(".");
      i++;
      delay(100);
    }
  }
  Serial.println();
  digitalWrite(pinLed, LOW);
}

void reconnect() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESPCliente")) {
      mqttClient.subscribe("detergentes");
    }
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pinLed,OUTPUT);
  // Conexión Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }
  Serial.println("Conexión Wi-Fi establecida");

  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  delay(1000);
}
