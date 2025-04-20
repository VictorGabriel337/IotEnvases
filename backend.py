from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import json
import os
import mysql.connector
from datetime import datetime

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "https://iotenvases.netlify.app"}})

latest_status = {}
status_lock = threading.Lock()

# Função para salvar os dados no banco MySQL
def salvar_dados_no_banco(data):
    try:
        conexao = mysql.connector.connect(
            host="127.0.0.1",
            port=3306,
            user="root",
            password="victorgabriel337",  # Substitua pela sua senha do MySQL
            database="iotenvases"
        )
        cursor = conexao.cursor()

        query = """
            INSERT INTO status_maquina (low_signal_count, cadence_total_time, non_cadence_total_time, data_hora)
            VALUES (%s, %s, %s, %s)
        """
        valores = (
            data.get("lowSignalCount"),
            data.get("cadenceTotalTime"),
            data.get("nonCadenceTotalTime"),
            datetime.now()
        )

        cursor.execute(query, valores)
        conexao.commit()
        cursor.close()
        conexao.close()
        print("✅ Dados inseridos no banco com sucesso.")
    except mysql.connector.Error as err:
        print("Erro ao salvar no banco:", err)

# Função chamada quando a mensagem MQTT é recebida
def on_message(client, userdata, msg):
    global latest_status
    print("Callback on_message chamado!")
    print("Mensagem MQTT recebida em tópico:", msg.topic)
    print("Payload recebido:", msg.payload.decode())

    if msg.topic == "machine/status":
        latest_status = json.loads(msg.payload.decode())
        salvar_dados_no_banco(latest_status)
        print("latest_status atualizado e salvo no banco:", latest_status)

# Thread do cliente MQTT
def mqtt_thread():
    print("Iniciando conexão MQTT...")
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set("Iotenvases", "Iotenvases42")
    mqtt_client.tls_set()
    mqtt_client.connect("534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud", 8883)
    mqtt_client.subscribe("machine/status")
    mqtt_client.on_message = on_message
    mqtt_client.loop_forever()

# Inicia a thread do MQTT
threading.Thread(target=mqtt_thread).start()

# Rota principal (opcional)
@app.route("/")
def home():
    return send_from_directory(os.path.join(app.root_path, 'Envases', 'Dashboard'), 'Dashboard.html')

# Rota que retorna os dados atuais
@app.route("/sensores", methods=["GET"])
def sensores():
    with status_lock:
        print("GET /sensores chamado")
        print("Conteúdo de latest_status:", latest_status)
        return jsonify(latest_status)

# Inicia o servidor Flask
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
