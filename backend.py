from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import json
import os
from datetime import datetime

app = Flask(__name__, static_folder='Envases/Dashboard', static_url_path='')
CORS(app)

# variável global protegida por lock
latest_status = {}
status_lock = threading.Lock()

@app.route("/")
def home():
    return send_from_directory(os.path.join(app.root_path, 'Envases', 'Dashboard'), 'Dashboard.html')


def on_connect(client, userdata, flags, rc):
    print("Conectado ao broker MQTT com código:", rc)
    client.subscribe("machine/status")

def on_message(client, userdata, msg):
    global latest_status
    print("Callback on_message chamado!")
    print("Mensagem MQTT recebida em tópico:", msg.topic)
    print("Payload recebido:", msg.payload.decode())

    try:
        payload = json.loads(msg.payload.decode())
        with status_lock:
            latest_status.clear()  # Garantir que estamos limpando a variável antes de atualizar
            latest_status.update(payload)  # Atualizar dados com segurança
            print("Dados de status atualizados:", latest_status)
    except json.JSONDecodeError:
        print("Erro ao decodificar JSON")


@app.route("/sensores", methods=["GET"])
def obter_dados():
    print("GET /sensores chamado")
    with status_lock:
        if not latest_status:
            print("Conteúdo de latest_status: VAZIO")
            return jsonify({"message": "Dados não disponíveis"}), 404  # Retornando código 404 se não houver dados
        print("Conteúdo de latest_status:", latest_status)
        return jsonify(latest_status)


def mqtt_thread():
    print("Iniciando conexão MQTT...")
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set("Iotenvases", "Iotenvases42")
    mqtt_client.tls_set()

    try:
        mqtt_client.connect("534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud", 8883)
        print("Conectado ao broker MQTT com sucesso.")
    except Exception as e:
        print("Erro ao conectar no MQTT:", e)
        return

    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    mqtt_client.loop_start()  # Inicia o loop para receber mensagens

# Inicializar a thread MQTT
threading.Thread(target=mqtt_thread, daemon=True).start()

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
