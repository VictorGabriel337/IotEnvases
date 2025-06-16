from flask import Flask, jsonify, render_template
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import json
import os
import ssl

app = Flask(__name__, template_folder='templates', static_folder='static')
CORS(app)  # Libera o CORS para todas as rotas e origens

latest_status = {}
status_lock = threading.Lock()

@app.route("/")
def home():
    return render_template("Dashboard.html")

@app.route("/sensores")
def sensores():
    with status_lock:
        print("Acessando /sensores")
        print("Dado atual:", latest_status)
        if not latest_status:
            return jsonify({"message": "Aguardando dados do sensor..."})
        return jsonify(latest_status)

@app.route("/status", methods=["GET"])
def get_status():
    with status_lock:
        return jsonify(latest_status)

def on_message(client, userdata, msg):
    global latest_status
    if msg.topic == "machine/status":
        with status_lock:
            try:
                latest_status = json.loads(msg.payload.decode())
                print("Mensagem recebida via MQTT:", latest_status)
            except json.JSONDecodeError:
                print("Erro ao decodificar JSON da mensagem MQTT.")

def mqtt_thread():
    print("Iniciando conexão MQTT...")
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set("Iotenvases", "Iotenvases42")
    mqtt_client.tls_set(tls_version=ssl.PROTOCOL_TLS)
    mqtt_client.tls_insecure_set(True)  # TEMPORÁRIO – cuidado em produção

    mqtt_client.on_message = on_message

    try:
        print("Antes de conectar...")
        mqtt_client.connect("534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud", 8883)
        print("Conectado ao broker!")
        mqtt_client.subscribe("machine/status")
        print("Inscrito no tópico MQTT.")
        mqtt_client.loop_forever()
    except Exception as e:
        print("Erro ao conectar ao MQTT:", e)

# Executa o thread MQTT em background
threading.Thread(target=mqtt_thread, daemon=True).start()

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))  # Porta fornecida pelo Render
    app.run(host='0.0.0.0', port=port)
