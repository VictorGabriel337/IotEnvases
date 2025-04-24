from flask import Flask, jsonify
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import ssl

app = Flask(__name__, static_folder='Envases/Dashboard', static_url_path='')
CORS(app)

# Configurações MQTT (HiveMQ)
MQTT_HOST = "534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud"
MQTT_PORT = 8883
MQTT_TOPIC = "machine/status"
MQTT_USERNAME = "Iotenvases"
MQTT_PASSWORD = "Iotenvases42"

# Variável para armazenar o último status recebido
last_status = {"status": "Aguardando dados..."}

# Callback quando conecta no broker
def on_connect(client, userdata, flags, rc):
    print("Conectado ao MQTT com código: " + str(rc))
    client.subscribe(MQTT_TOPIC)

# Callback quando recebe mensagem
def on_message(client, userdata, msg):
    global last_status
    try:
        payload = msg.payload.decode()
        print(f"Mensagem recebida no tópico {msg.topic}: {payload}")
        last_status = {"status": payload}
    except Exception as e:
        print("Erro ao processar mensagem MQTT:", e)

# Função para rodar o MQTT client em uma thread separada
def mqtt_thread():
    client = mqtt.Client()
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.tls_set_context(ssl.create_default_context())
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(MQTT_HOST, MQTT_PORT, 60)
    client.loop_forever()

# Endpoint GET para fornecer o status mais recente
@app.route("/status", methods=["GET"])
def get_status():
    return jsonify(last_status)

# Inicia a thread MQTT e o servidor Flask
if __name__ == "__main__":
    threading.Thread(target=mqtt_thread).start()
    app.run(host="0.0.0.0", port=5000, debug=True)
