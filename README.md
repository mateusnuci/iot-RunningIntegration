


# 🏃‍♂️ Detecção de Pisada com ESP32, MPU6050 e MQTT

## 📌 Propósito

Este projeto tem como objetivo **monitorar o tempo de contato do pé com o solo durante uma corrida**, utilizando um **sensor MPU6050** acoplado a um **ESP32**. A partir disso, é possível analisar a mecânica da corrida para fins de **melhoria de desempenho**, **biomecânica esportiva** ou **prevenção de lesões**.

---

## ⚙️ Como Funciona

- O **ESP32** lê os dados do acelerômetro do **MPU6050**.
- Ao detectar um impacto (pisada) e posterior saída do pé do solo, ele **calcula o tempo de contato com o chão**.
- O tempo é enviado via **protocolo MQTT** para um broker (neste caso, o **HiveMQ**).
- Os dados podem ser consumidos por qualquer cliente MQTT, como um **script Python**, que poderá:
  - Fazer análise dos dados em tempo real
  - Exibir visualizações
  - Retornar comandos para acionar dispositivos (ex: buzzer de feedback)

---

## 🔗 Integração com HiveMQ

Este projeto utiliza o [HiveMQ Broker Público](https://www.hivemq.com/demos/websocket-client/) como ponto de comunicação MQTT.
https://www.thingiverse.com/thing:632676

- **Broker:** `broker.hivemq.com`
- **Porta:** `1883`
- **Tópico de envio:** `corrida/pisada`
- **Formato da mensagem:**
  ```json
  { "duracao": 145 }
