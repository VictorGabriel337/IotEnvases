from flask import Flask, jsonify
from flask_cors import CORS  # Importa o CORS

app = Flask(__name__)
CORS(app)  # Permite CORS para todas as rotas

ESP_IP = "http://192.168.15.7:80/"  # IP do ESP8266 na rede local

@app.route('/api', methods=['GET'])
def get_sensor_data():
    try:
        response = requests.get(ESP_IP, timeout=5)  # Requisição ao ESP8266
        response.raise_for_status()  # Levanta erro se houver falha
        return jsonify(response.json())  # Retorna os dados recebidos para a aplicação
    except requests.exceptions.RequestException as e:
        return jsonify({"error": "Falha ao conectar ao ESP8266", "details": str(e)}), 500

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000)
