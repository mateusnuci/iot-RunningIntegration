#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

// === MPU ===
Adafruit_MPU6050 mpu;

// === Buzzer ===
#define BUZZER_PIN  13 // Escolha o pino que você conectou o buzzer

// === Estados ===
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
const char* ssid = "REDE";
const char* password = "SENHA";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

// === Função de callback para mensagens recebidas ===
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  String mensagem;
  for (unsigned int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  Serial.print("Conteúdo: ");
  Serial.println(mensagem);

  if (String(topic) == "corrida/feedback") {
    if (mensagem == "ok") {
      tocarBuzzer(1); // Pace correto
    } else if (mensagem == "erro") {
      tocarBuzzer(3); // Pace incorreto
    }
  }
}

// === Função para tocar o buzzer ===
void tocarBuzzer(int vezes) {
  for (int i = 0; i < vezes; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect("ESP32_Pisada")) {
      Serial.println("Conectado.");
      client.subscribe("corrida/feedback");  // Inscreve-se no tópico de feedback
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando em 5 segundos.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

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
  if (modulo > limiarImpacto && acelZ < limiarDesaceleracaoZ &&
      (agora - tempoUltimaDeteccao > intervaloDebounce)) {
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
      String mensagem = String("{\"duracao\":") + duracaoContato +
                        String(",\"tempoPisada\":") + tempoInicioContato + String("}");
      client.publish("corrida/pisada", mensagem.c_str());
      Serial.print("Tempo de contato: ");
      Serial.print(duracaoContato);
      Serial.println(" ms | Enviado via MQTT");
    }

    emContato = false;
    emContagemContato = false;
  }

  delay(5);
}
