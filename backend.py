from flask import Flask, jsonify
import json
import threading
import paho.mqtt.client as mqtt

app = Flask(__name__)

latest_status = {}
status_lock = threading.Lock()

def on_connect(client, userdata, flags, rc):
    print("✅ Conectado ao broker MQTT com código:", rc)
    client.subscribe("machine/status")

def on_message(client, userdata, msg):
    print("📥 Mensagem recebida:", msg.payload.decode())
    try:
        payload = json.loads(msg.payload.decode())
        with status_lock:
            global latest_status
            latest_status = payload
            print("🆕 Status atualizado:", latest_status)
    except Exception as e:
        print("❌ Erro ao processar mensagem:", e)

@app.route("/sensores", methods=["GET"])
def sensores():
    print("GET /sensores chamado")
    with status_lock:
        if not latest_status:
            print("Conteúdo de latest_status: VAZIO")
            return jsonify({"message": "Dados não disponíveis"})
        print("Conteúdo de latest_status:", latest_status)
        return jsonify(latest_status)

def start_mqtt():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("broker.hivemq.com", 1883, 60)  # Ou o seu broker
    client.loop_start()

if __name__ == "__main__":
    print("🚀 Iniciando app e MQTT...")
    start_mqtt()
    app.run(host="0.0.0.0", port=5000)
