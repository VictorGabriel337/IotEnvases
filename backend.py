from flask import Flask, jsonify ,send_from_directory
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import json
import os
import mysql.connector
from datetime import datetime
import time

app = Flask(__name__, static_folder='Envases/Dashboard', static_url_path='')
CORS(app,)  # Libera o CORS para todas as rotas e origens


@app.route("/")
def home():
    return send_from_directory(os.path.join(app.root_path, 'Envases', 'Dashboard'), 'Dashboard.html')



latest_status = {}
status_lock = threading.Lock()

def on_message(client, userdata, msg):
    global latest_status
    print("Callback on_message chamado!")
    print("Mensagem MQTT recebida em tópico:", msg.topic)
    print("Payload recebido:", msg.payload.decode())

    if msg.topic == "machine/status":
        try:
            latest_status = json.loads(msg.payload.decode())
            print("Dados de status atualizados:", latest_status)
        except json.JSONDecodeError:
            print("Erro ao decodificar JSON")


@app.route("/sensores", methods=["GET"])
def sensores():
    with status_lock:
        print("GET /sensores chamado")
        print("Conteúdo de latest_status:", latest_status)
        if not latest_status or "lowSignalCount" not in latest_status:
            return jsonify({"message": "Dados não disponíveis"})
        return jsonify(latest_status)
    
    





def mqtt_thread():
    print(" Iniciando conexão MQTT...")
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set("Iotenvases", "Iotenvases42")
    mqtt_client.tls_set()

    try:
        mqtt_client.connect("534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud", 8883)
        print(" Conectado ao broker MQTT com sucesso.")
    except Exception as e:
        print(" Erro ao conectar no MQTT:", e)
        return

    mqtt_client.subscribe("machine/status")
    print(" Subscrito no tópico machine/status")
    mqtt_client.on_message = on_message
    mqtt_client.loop_forever()

threading.Thread(target=mqtt_thread).start()



    


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
    app.run(debug=True)

