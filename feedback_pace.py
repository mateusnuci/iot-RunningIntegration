import paho.mqtt.client as mqtt
import json
from collections import deque
import statistics
import time

# === Configurações do MQTT ===
broker = "broker.hivemq.com"
port = 1883
topic_receber = "corrida/pisada"
topic_enviar = "corrida/feedback"

duracoes = deque(maxlen=5)
tempos = deque(maxlen=5)


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✅ Conectado ao broker MQTT!")
        client.subscribe(topic_receber)
    else:
        print("❌ Falha na conexão. Código:", rc)


def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode()
        dados = json.loads(payload)
        duracao = dados.get("duracao")
        tempo_pisada = dados.get("tempoPisada")

        if duracao is not None and tempo_pisada is not None:
            duracoes.append(duracao)
            tempos.append(tempo_pisada)

            print(f"🔸 Pisada recebida: duração={duracao} ms | tempo={tempo_pisada} ms")

            if len(duracoes) == 5:
                analisar()

        else:
            print("⚠️ Dados incompletos recebidos:", dados)

    except Exception as e:
        print("❌ Erro ao processar mensagem:", e)


def analise():
    media_duracao = statistics.mean(duracoes)
    desv_duracao = statistics.stdev(duracoes) if len(duracoes) > 1 else 0

    intervalos = [j - i for i, j in zip(tempos, list(tempos)[1:])]
    media_frequencia = statistics.mean(intervalos) if intervalos else 0
    desv_frequencia = statistics.stdev(intervalos) if len(intervalos) > 1 else 0

    print("\n=== 🏃‍♂️ Análise das últimas 5 pisadas ===")
    print(f"➡️ Média de contato: {media_duracao:.2f} ms (Desvio: {desv_duracao:.2f})")
    print(f"➡️ Média intervalo: {media_frequencia:.2f} ms (Desvio: {desv_frequencia:.2f})")
    print("==========================================")

    # 🚦 Critério simples: se desvio < 10% da média, pace ok
    tolerancia = 0.10  # 10%
    contato_estavel = desv_duracao < media_duracao * tolerancia
    freq_estavel = desv_frequencia < media_frequencia * tolerancia

    if contato_estavel and freq_estavel:
        print("✅ Pace estável. Enviando feedback positivo (1 bip).")
        client.publish(topic_enviar, "1")
    else:
        print("⚠️ Pace irregular. Enviando alerta (3 bips).")
        client.publish(topic_enviar, "3")


# === Setup do cliente MQTT ===
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(broker, port, 60)

# === Loop MQTT ===
client.loop_forever()
