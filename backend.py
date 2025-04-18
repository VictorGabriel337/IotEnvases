from flask import Flask, jsonify ,send_from_directory
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import json
import os

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "https://iotenvases.netlify.app"}})  # Libera o CORS para todas as rotas e origens


@app.route("/")
def home():
    return send_from_directory(os.path.join(app.root_path, 'Envases', 'Dashboard'), 'Dashboard.html')


# @app.route("/sensores", methods=["GET"])
# def get_sensor_data():
#     # Aqui você pode conectar com MQTT, banco de dados ou variáveis mockadas
#     # Por enquanto vamos usar valores simulados (só pra teste)
#     return jsonify({
#         "lowSignalCount": ...,
#         "cadenceTotalTime": ...,       # em segundos
#         "nonCadenceTotalTime": ...    # em segundos
#     })

# @app.route("/sensores")
# def sensores():

#     with status_lock:
#         print("Acessando /sensores")
#         # if not latest_status:
#         #     return jsonify({"message": "Aguardando dados do sensor..."})
#         # on_message()
#         # Aqui, você pode mapear os dados para os formatos esperados pelo frontend
#         latest_status = json.loads(msg.payload.decode())
#         sensor_data = {
            
#             "lowSignalCount": latest_status.get("lowSignalCount", ),
#             "cadenceTotalTime": latest_status.get("cadenceTotalTime", ),  # em segundos
#             "nonCadenceTotalTime": latest_status.get("nonCadenceTotalTime",)  # em segundos
#         }
#         return jsonify(sensor_data)

latest_status = {}
# status_lock = threading.Lock()

def on_message(client, userdata, msg):
    global latest_status
    print("Callback on_message chamado!")
    print("Mensagem MQTT recebida em tópico:", msg.topic)
    print("Payload recebido:", msg.payload.decode())

    if msg.topic == "machine/status":
        latest_status = json.loads(msg.payload.decode())
        print("latest_status atualizado:", latest_status)



@app.route("/sensores" , methods=["GET"])
def sensores():
    with status_lock:
        print("GET /sensores chamado")
        print("Conteúdo de latest_status:", latest_status)
        # if not latest_status:
        #     return jsonify({"message": "Aguardando dados do sensor..."})
        return jsonify(latest_status)
    
    





def mqtt_thread():
    print("Iniciando conexão MQTT...")
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set("Iotenvases", "Iotenvases42")
    mqtt_client.tls_set()
    mqtt_client.connect("534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud", 8883)
    mqtt_client.subscribe("machine/status")
    mqtt_client.on_message = on_message
    mqtt_client.loop_forever()

threading.Thread(target=mqtt_thread).start()

# @app.route("/status", methods=["GET"])
# def get_status():
#     return jsonify(latest_status)



if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
