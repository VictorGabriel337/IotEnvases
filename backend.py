from flask import Flask, jsonify ,send_from_directory
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import json
import os
from datetime import datetime

app = Flask(__name__, static_folder='Envases/Dashboard', static_url_path='')
CORS(app,)  # Libera o CORS para todas as rotas e origens

# variável global protegida por lock
latest_status = {}
status_lock = threading.Lock()

@app.route("/")
def home():
    return send_from_directory(os.path.join(app.root_path, 'Envases', 'Dashboard'), 'Dashboard.html')



latest_status = {}
status_lock = threading.Lock()


def on_connect(client, userdata, flags, rc):
    print("✅ Conectado ao broker MQTT com código:", rc)
    client.subscribe("machine/status")

def on_message(client, userdata, msg):
    print("Callback on_message chamado!")
    print("Mensagem MQTT recebida em tópico:", msg.topic)
    print("Payload recebido:", msg.payload.decode())

    try:
        payload = json.loads(msg.payload.decode())
        with status_lock:
            global latest_status
            latest_status = payload
            print("Dados de status atualizados:", latest_status)
    except json.JSONDecodeError:
        print(" Erro ao decodificar JSON")


@app.route("/sensores", methods=["POST"])
def obter_dados():
    print("GET /sensores chamado")
    with status_lock:
        if not latest_status:
            print("Conteúdo de latest_status: VAZIO")
            return jsonify({"message": "Dados não disponíveis"})
        print("Conteúdo de latest_status:", latest_status)
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
    mqtt_client.loop_start()

threading.Thread(target=mqtt_thread).start()



    


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
    start_mqtt()
    app.run(debug=True)

