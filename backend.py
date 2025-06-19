from flask import Flask, jsonify, render_template
from flask_cors import CORS
import redis
import json
import os

app = Flask(__name__, template_folder='templates', static_folder='static')
CORS(app)

# Conexão Redis
redis_client = redis.Redis(host=os.getenv("REDIS_HOST", "localhost"), port=6379, decode_responses=True)

@app.route("/")
def home():
    return render_template("Dashboard.html")

@app.route("/sensores")
def sensores():
    print("Rota /sensores acessada. PID:", os.getpid())
    status = redis_client.get("latest_status")
    if not status:
        return jsonify({"message": "Aguardando dados do sensor..."})
    return jsonify(json.loads(status))

@app.route("/status", methods=["GET"])
def get_status():
    status = redis_client.get("latest_status")
    if not status:
        return jsonify({})
    return jsonify(json.loads(status))

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port)
