#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>

#define pinLed 2

const char* ssid = "IZZI-90C0";
const char* password = "5095512890C0";

const char* server = "broker.emqx.io";
int port = 1883;

const char* URL = "http://192.168.0.13/machine_vending/cliente/insertarDatosESP.php";
const char* data = "data=Cloro";

WiFiClient wifiClient;
HTTPClient http;
PubSubClient mqttClient(wifiClient);

void sendData() {
  http.begin(URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpCode = http.POST(data);
  String payload = http.getString();
  
  Serial.print("URL : "); Serial.println(URL);
  Serial.print("DATA : "); Serial.println(data);
  Serial.print("httpCode : "); Serial.println(httpCode);
  Serial.print("payload : "); Serial.println(payload);
  Serial.println("-------------------------------------------------");

  http.end();
}

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
    Serial.println();
    sendData();
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
  pinMode(pinLed, OUTPUT);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }
  Serial.println("Conexión Wi-Fi establecida");
  Serial.println(WiFi.localIP());
  
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
