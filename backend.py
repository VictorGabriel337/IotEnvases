from flask import Flask, jsonify
import requests

app = Flask(__name__)

ESP8266_IP = 'http://192.168.15.7'  # ou seu IP público / DDNS

@app.route('/dados', methods=['GET'])
def get_dados():
    try:
        response = requests.get(f'{ESP8266_IP}', timeout=3)
        return jsonify(response.json())
    except Exception as e:
        return jsonify({'erro': str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True)
