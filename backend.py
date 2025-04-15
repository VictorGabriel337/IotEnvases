from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import json
import os
import time

app = Flask(__name__)
CORS(app)

latest_status = {}
status_lock = threading.Lock()

@app.route("/")
def home():
    return send_from_directory(os.path.join(app.root_path, 'Envases', 'Dashboard'), 'Dashboard.html')

@app.route("/sensores")
def sensores():
    with status_lock:
        print("Acessando /sensores")
        if not latest_status:
            return jsonify({"message": "Aguardando dados do sensor..."})
        return jsonify(latest_status)

def on_message(client, userdata, msg):
    global latest_status
    if msg.topic == "machine/status":
        payload = msg.payload.decode()
        print("Mensagem recebida via MQTT:", payload)
        try:
            status = json.loads(payload)
            with status_lock:
                latest_status = status
        except Exception as e:
            print("Erro ao fazer parse do JSON:", e)

def mqtt_thread():
    print("Iniciando conexão MQTT...")
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set("Iotenvases", "Iotenvases42")
    mqtt_client.tls_set()
    
    # reconectar se cair
    while True:
        try:
            mqtt_client.connect("534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud", 8883)
            mqtt_client.subscribe("machine/status")
            mqtt_client.on_message = on_message
            mqtt_client.loop_forever()
        except Exception as e:
            print("Erro na conexão MQTT:", e)
            time.sleep(5)  # espera e tenta de novo

# Inicia a thread MQTT antes de rodar o app
threading.Thread(target=mqtt_thread, daemon=True).start()

@app.route("/status", methods=["GET"])
def get_status():
    return jsonify(latest_status)

if __name__ == "__main__":
    print("Iniciando backend Flask...")
    app.run(host="0.0.0.0", port=5000)
