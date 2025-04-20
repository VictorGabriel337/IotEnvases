from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import json
import os
import time  # Importe o módulo time

app = Flask(__name__)
CORS(app)

@app.route("/")
def home():
    return send_from_directory(os.path.join(app.root_path, 'Envases', 'Dashboard'), 'Dashboard.html')

latest_status = {}
status_lock = threading.Lock()

def on_message(client, userdata, msg):
    global latest_status
    print(f"Callback on_message chamado! Tópico: {msg.topic}, Payload: {msg.payload.decode()}")
    if msg.topic == "machine/status":
        try:
            latest_status = json.loads(msg.payload.decode())
            print("Dados de status atualizados:", latest_status)
        except json.JSONDecodeError:
            print("Erro ao decodificar JSON:", msg.payload.decode())

@app.route("/sensores", methods=["GET"])
def sensores():
    with status_lock:
        print("GET /sensores chamado. Conteúdo de latest_status:", latest_status)
        if not latest_status or not all(key in latest_status for key in ["lowSignalCount", "cadenceTotalTime", "nonCadenceTotalTime"]):
            print("Dados incompletos ou não disponíveis.")
            return jsonify({"message": "Dados não disponíveis"}), 204  # Retorna 204 No Content
        print("Dados sendo enviados:", latest_status)
        return jsonify(latest_status)

def mqtt_thread():
    print("Iniciando conexão MQTT...")
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set("Iotenvases", "Iotenvases42")
    mqtt_client.tls_set()
    try:
        mqtt_client.connect("534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud", 8883)
        mqtt_client.subscribe("machine/status")
        mqtt_client.on_message = on_message
        mqtt_client.loop_forever()
    except Exception as e:
        print(f"Erro na conexão MQTT: {e}")

threading.Thread(target=mqtt_thread).start()

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)