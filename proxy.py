from flask import Flask, jsonify
from flask_cors import CORS  # Importando CORS
import requests
import os

app = Flask(__name__)
CORS(app)  # Habilita CORS para todas as origens

ESP_IP = "http://192.168.15.7:80/"  # IP do ESP8266

@app.route('/api', methods=['GET'])
def get_sensor_data():
    try:
        response = requests.get(ESP_IP, timeout=5)  # Requisição ao ESP8266
        response.raise_for_status()
        return jsonify(response.json())  # Retorna os dados
    except requests.exceptions.RequestException as e:
        return jsonify({"error": "Falha ao conectar ao ESP8266", "details": str(e)}), 500

if __name__ == '__main__':
    port = int(os.environ.get("PORT", 5000))  # Porta do Railway
    app.run(host="0.0.0.0", port=port)
