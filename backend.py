from flask import Flask, jsonify
from flask_cors import CORS
import paho.mqtt.client as mqtt
import threading
import json
from datetime import datetime
import time

app = Flask(__name__)
CORS(app)  # Habilita CORS para todas as rotas

# Estrutura inicial com valores padrão
latest_status = {
    "cadenceStatus": "Aguardando dados...",
    "lowSignalCount": 0,
    "cadenceTotalTime": 0,
    "nonCadenceTotalTime": 0,
    "lastUpdate": None
}
status_lock = threading.Lock()

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conectado ao broker MQTT com sucesso!")
        client.subscribe("machine/status")
        print("Inscrito no tópico 'machine/status'")
    else:
        print(f"Falha na conexão MQTT. Código: {rc}")

def on_message(client, userdata, msg):
    global latest_status
    try:
        payload = msg.payload.decode()
        data = json.loads(payload)
        
        print(f"\n--- Dados recebidos do MQTT ---")
        print(f"Tópico: {msg.topic}")
        print(f"Conteúdo: {data}")
        
        with status_lock:
            # Atualiza apenas os campos recebidos, mantendo os padrões se não existirem
            latest_status = {
                "cadenceStatus": data.get("cadenceStatus", latest_status["cadenceStatus"]),
                "lowSignalCount": int(data.get("lowSignalCount", latest_status["lowSignalCount"])),
                "cadenceTotalTime": int(data.get("cadenceTotalTime", latest_status["cadenceTotalTime"])),
                "nonCadenceTotalTime": int(data.get("nonCadenceTotalTime", latest_status["nonCadenceTotalTime"])),
                "lastUpdate": datetime.now().isoformat()
            }
            
            print("--- Dados atualizados no sistema ---")
            print(latest_status)
            
    except json.JSONDecodeError:
        print(f"Erro: Payload não é JSON válido. Recebido: {payload}")
    except Exception as e:
        print(f"Erro ao processar mensagem MQTT: {str(e)}")

@app.route("/sensores")
def get_sensores():
    with status_lock:
        # Cria cópia do dicionário para resposta
        response_data = latest_status.copy()
        
        # Garante que os valores numéricos sejam inteiros
        response_data["lowSignalCount"] = int(response_data["lowSignalCount"])
        response_data["cadenceTotalTime"] = int(response_data["cadenceTotalTime"])
        response_data["nonCadenceTotalTime"] = int(response_data["nonCadenceTotalTime"])
        
        print(f"\n--- Enviando para o frontend ---")
        print(response_data)
        
        return jsonify(response_data)

@app.route("/")
def home():
    return "Backend IoT Envases - Use a rota /sensores para obter dados"

def mqtt_client_thread():
    while True:
        try:
            client = mqtt.Client()
            client.username_pw_set("Iotenvases", "Iotenvases42")
            client.tls_set()
            
            client.on_connect = on_connect
            client.on_message = on_message
            
            print("\nTentando conectar ao HiveMQ...")
            client.connect("534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud", 8883, 60)
            
            # Inicia o loop em uma thread separada para permitir reconexão
            client.loop_forever()
            
        except Exception as e:
            print(f"Erro na conexão MQTT: {str(e)}")
            print("Tentando reconectar em 10 segundos...")
            time.sleep(10)
        finally:
            try:
                client.disconnect()
            except:
                pass

if __name__ == "__main__":
    # Inicia a thread MQTT como daemon (encerra quando o main thread encerrar)
    mqtt_thread = threading.Thread(target=mqtt_client_thread, daemon=True)
    mqtt_thread.start()
    
    # Configurações adicionais do Flask para melhor desempenho
    app.run(
        host="0.0.0.0",
        port=5000,
        threaded=True,
        debug=False  # Defina como True apenas para desenvolvimento
    )