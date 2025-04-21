from flask import Flask, jsonify
from flask_cors import CORS
import paho.mqtt.client as mqtt
import json

app = Flask(__name__)
CORS(app)

# Variável global para guardar os últimos dados
latest_data = {
    "cadenceStatus": "Desconhecido",
    "lowSignalCount": 0,
    "cadenceTotalTime": 0,
    "nonCadenceTotalTime": 0
}

# Callback do MQTT
def on_connect(client, userdata, flags, rc):
    print("Conectado ao broker MQTT com código:", rc)
    client.subscribe("machine/status")

def on_message(client, userdata, msg):
    global latest_data
    try:
        payload = json.loads(msg.payload.decode())
        latest_data = payload
        print("Dados recebidos do MQTT:", latest_data)
    except Exception as e:
        print("Erro ao processar mensagem:", e)

# Configuração do cliente MQTT
mqtt_client = mqtt.Client()
mqtt_client.username_pw_set("Iotenvases", "Iotenvases42")
mqtt_client.tls_set()  # Importante para conexão com HiveMQ Cloud (porta 8883)
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect("broker.hivemq.com", 1883)

# Inicia o loop do MQTT em segundo plano
mqtt_client.loop_start()

# Rota para o front-end buscar os dados
@app.route("/status", methods=["GET"])
def get_status():
    return jsonify(latest_data)

if __name__ == "__main__":
    app.run(debug=True)
