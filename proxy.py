from flask import Flask, request, jsonify
import requests

app = Flask(__name__)

# URL do ESP8266 na sua rede local
ESP_URL = "http://192.168.15.7/dados"  # Substitua pelo IP real do ESP

@app.route("/dados", methods=["GET"])
def proxy():
    try:
        # Faz a requisição ao ESP8266
        response = requests.get(ESP_URL, timeout=5)
        data = response.json()  # Converte a resposta para JSON
        return jsonify(data)  # Retorna os dados para o site
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
