void sendRequestGET() {
  WiFiClient client;
  if (client.connect(serverIP, serverPort)) {
    Serial.println("Conexión establecida con el servidor");
    bool estado = estadoMaquina();
    if (estado) {
      // Enviar una solicitud GET al servidor para escribir en el archivo
      String request = String("GET /8266/admin/escribir_archivo.php?data=" + String(ok) + " HTTP/1.1\r\n") +
                       "Host: " + String(serverIP) + "\r\n\r\n";
      client.print(request);
    } else {
      // Enviar una solicitud GET al servidor para escribir en el archivo
      String request = String("GET /8266/admin/escribir_archivo.php?data=" + String(fail) + " HTTP/1.1\r\n") +
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

void handleNotFound() {
  server.send(404, "text/plain", "404 Not Found");
}