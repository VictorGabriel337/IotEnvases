from flask import Flask, jsonify, render_template
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import json
import os

app = Flask(__name__, template_folder='templates', static_folder='static')
CORS(app)

latest_status = {}
status_lock = threading.Lock()

@app.route("/")
def home():
    return render_template("Dashboard.html")

@app.route("/sensores")
def sensores():
    with status_lock:
        if not latest_status:
            return jsonify({"message": "Aguardando dados do sensor..."})
        return jsonify(latest_status)

@app.route("/status")
def get_status():
    return jsonify(latest_status)

def on_message(client, userdata, msg):
    global latest_status
    if msg.topic == "machine/status":
        with status_lock:
            latest_status = json.loads(msg.payload.decode())
        print("Mensagem recebida:", latest_status)

@app.before_first_request
def start_mqtt_thread():
    print("Iniciando thread MQTT via before_first_request...")
    threading.Thread(target=mqtt_thread, daemon=True).start()
    try:
        mqtt_client = mqtt.Client()
        mqtt_client.username_pw_set("Iotenvases", "Iotenvases42")
        mqtt_client.tls_set()
        mqtt_client.tls_insecure_set(True)
        mqtt_client.connect("534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud", 8883)
        mqtt_client.subscribe("machine/status")
        mqtt_client.on_message = on_message
        mqtt_client.loop_forever()
    except Exception as e:
        print("Erro na thread MQTT:", e)

 # Inicia a thread MQTT somente quando rodar local ou via Render com Python puro
    # threading.Thread(target=mqtt_thread, daemon=True).start()
if __name__ == '__main__':
   

    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port)
