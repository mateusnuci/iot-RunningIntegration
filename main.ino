#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

bool emContato = false;
bool emContagemContato = false;
unsigned long tempoInicioContato = 0;
unsigned long tempoFimContato = 0;
unsigned long tempoUltimaDeteccao = 0;

float limiarImpacto = 16.0;  
float limiarSaida = 6.0;    
unsigned long intervaloDebounce = 400;  

void setup() {
  Serial.begin(9600);
  if (!mpu.begin()) {
    Serial.println("MPU6050 não conectado!");
    while (1);
  }
  Serial.println("MPU6050 iniciado.");
  delay(100);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float acelX = a.acceleration.x;  
  float acelY = a.acceleration.y; 
  float acelZ = a.acceleration.z;

  float module = sqrt(acelX * acelX + acelY * acelY + acelZ * acelZ);
  unsigned long agora = millis();

  // INÍCIO DO CONTATO COM O SOLO
  if (acelX > limiarImpacto && !emContato && (agora - tempoUltimaDeteccao > intervaloDebounce)) {
    emContato = true;
    emContagemContato = true;
    tempoInicioContato = agora;
    tempoUltimaDeteccao = agora;


    Serial.print("Impacto detectado em ");
    Serial.print(tempoInicioContato);
    Serial.println(" ms");
    Serial.println("Modulo XYZ: ");
    Serial.println(module);
    Serial.println("Modulo X: ");
    Serial.println(acelX);
    
  }

  // FIM DO CONTATO COM O SOLO
  if (emContagemContato && acelX < limiarSaida) {
    tempoFimContato = agora;
    emContagemContato = false;

    unsigned long duracaoContato = tempoFimContato - tempoInicioContato;
    Serial.print("Tempo de contato com o solo: ");
    Serial.print(duracaoContato);
    Serial.println(" ms");

    emContato = false;
  }

  delay(5);
}
