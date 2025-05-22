🏃‍♂️ Projeto Detecção de Pace com ESP32 + MQTT + Python
🚀 Propósito
Este projeto tem como objetivo detectar o tempo de contato dos pés com o solo durante uma corrida, utilizando um sensor MPU6050 conectado a um ESP32.

O ESP envia os dados de cada passada via MQTT (broker HiveMQ) para um script Python que:

Calcula a média do tempo de contato e a frequência das passadas (intervalo entre elas).

Verifica se o atleta está mantendo um pace consistente.

Retorna um sinal via MQTT:

✅ 1 bip no buzzer — se o pace está regular.

⚠️ 3 bips — se está irregular.

🔗 Integrações
🛰️ ESP32 com sensor MPU6050 → detecta as pisadas.

☁️ Broker MQTT (HiveMQ) → transmissão dos dados.

🐍 Python Script → análise estatística e controle de feedback (buzzer no ESP).

🏗️ Estrutura do Projeto
bash
Copiar
Editar
.
├── esp32_pisada.ino        # Código Arduino para ESP32
├── feedback_pace.py        # Script Python (recebe, calcula e envia sinais)
├── requirements.txt        # Dependências Python
└── README.md               # Este arquivo
🔧 Instalação do Script Python
Clone o projeto:

bash
Copiar
Editar
git clone https://github.com/seu-usuario/seu-repo.git
cd seu-repo
Crie um ambiente virtual (opcional, mas recomendado):

bash
Copiar
Editar
python -m venv venv
venv\Scripts\activate        # Windows
source venv/bin/activate     # Linux/Mac
Instale as dependências:

bash
Copiar
Editar
pip install -r requirements.txt
Execute o script:

bash
Copiar
Editar
python feedback_pace.py
📦 requirements.txt
txt
Copiar
Editar
paho-mqtt
🌐 Configurações MQTT
Broker público: broker.hivemq.com

Tópicos usados:

📥 Recebe dados do ESP: corrida/pisada

📤 Envia sinal de feedback: corrida/feedback

💡 Funcionamento do Feedback
Condição	Ação no ESP32
Pace consistente (baixo desvio)	🔔 1 Bip
Pace irregular (alto desvio)	🔔🔔🔔 3 Bips

🛠️ Tecnologias
📡 ESP32 + C++ (Arduino)

📐 MPU6050 (Acelerômetro)

🔗 MQTT (HiveMQ)

🐍 Python + paho-mqtt

🤝 Créditos
Desenvolvido por Mateus Nuci, Vitor Santos e Luigi Licciardi. 🚀
Esse projeto é experimental e educativo, visando explorar IoT, computação embarcada e análise de dados em tempo real.

