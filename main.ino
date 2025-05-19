#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

// === MPU ===
Adafruit_MPU6050 mpu;

bool emContato = false;
bool emContagemContato = false;

unsigned long tempoInicioContato = 0;
unsigned long tempoFimContato = 0;
unsigned long tempoUltimaDeteccao = 0;

float limiarImpacto = 16.0;
float limiarSaida = 6.0;
float limiarDesaceleracaoZ = -1.0;

unsigned long intervaloDebounce = 300;
unsigned long tempoMinimoContato = 80;

// === WiFi + MQTT ===
const char* ssid = "FIAP-WIFI";
const char* password = "FiapBXx&J5";
const char* mqtt_server = "broker.hivemq.com";  // Pode usar seu próprio broker

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.print("Conectando-se ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP32_Pisada")) {
      Serial.println("Conectado.");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  if (!mpu.begin()) {
    Serial.println("MPU6050 não conectado!");
    while (1);
  }
  Serial.println("MPU6050 iniciado.");
  delay(100);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float acelX = a.acceleration.x;
  float acelY = a.acceleration.y;
  float acelZ = a.acceleration.z;

  float modulo = sqrt(acelX * acelX + acelY * acelY + acelZ * acelZ);
  unsigned long agora = millis();

  bool impactoValido = false;
  if (modulo > limiarImpacto && acelZ < limiarDesaceleracaoZ && (agora - tempoUltimaDeteccao > intervaloDebounce)) {
    impactoValido = true;
  }

  if (impactoValido && !emContato) {
    emContato = true;
    emContagemContato = true;
    tempoInicioContato = agora;
    tempoUltimaDeteccao = agora;

    Serial.print("Impacto detectado em ");
    Serial.print(tempoInicioContato);
    Serial.println(" ms");
  }

  if (emContagemContato && modulo < limiarSaida) {
    tempoFimContato = agora;
    unsigned long duracaoContato = tempoFimContato - tempoInicioContato;

    if (duracaoContato >= tempoMinimoContato) {
      String mensagem = String("{\"duracao\":") + duracaoContato + String("}");
      client.publish("corrida/pisada", mensagem.c_str());
      Serial.print("Tempo de contato com o solo: ");
      Serial.print(duracaoContato);
      Serial.println(" ms | Enviado via MQTT");
    }

    emContato = false;
    emContagemContato = false;
  }

  delay(5);
}
