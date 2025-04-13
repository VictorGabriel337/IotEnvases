from flask import Flask, jsonify , render_template
import paho.mqtt.client as mqtt
import threading
from flask_cors import CORS

app = Flask(__name__)

# Variáveis globais para guardar os dados mais recentes
dados_sensores = {
    "lowSignalCount": 0,
    "cadenceTotalTime": 0,
    "nonCadenceTotalTime": 0
}

# MQTT config
MQTT_BROKER = "534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud"
MQTT_PORT = 8883
MQTT_TOPIC = "machine/status"
MQTT_USERNAME = "Iotenvases"  
MQTT_PASSWORD = "Iotenvases42"   



# Rota para o front-end
@app.route('/')
def index():
    return render_template('index.html')  # Exemplo, altere conforme necessário


def on_connect(client, userdata, flags, rc):
    print("Conectado ao MQTT:", rc)
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    import json
    try:
        payload = json.loads(msg.payload.decode())
        print("Payload recebido:", payload)
        dados_sensores.update(payload)
    except Exception as e:
        print("Erro ao processar payload:", e)

def iniciar_mqtt():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.loop_forever()

# Iniciar MQTT em outra thread
threading.Thread(target=iniciar_mqtt, daemon=True).start()

@app.route('/sensores', methods=['GET'])
def get_dados():
    return jsonify(dados_sensores)

if __name__ == '__main__':
    app.run(debug=True)
