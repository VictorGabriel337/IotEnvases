import paho.mqtt.client as mqtt
import json
import redis
import os

# Conectar ao Redis (adaptar para URL da sua instância, ou localhost se estiver testando local)
redis_client = redis.Redis(host=os.getenv("REDIS_HOST", "localhost"), port=6379, decode_responses=True)

def on_message(client, userdata, msg):
    if msg.topic == "machine/status":
        data = json.loads(msg.payload.decode())
        redis_client.set("latest_status", json.dumps(data))
        print("Dados MQTT salvos no Redis:", data)

def mqtt_thread():
    print("Iniciando conexão MQTT...")
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set("Iotenvases", "Iotenvases42")
    mqtt_client.tls_set()
    mqtt_client.tls_insecure_set(True)
    mqtt_client.connect("534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud", 8883)
    mqtt_client.subscribe("machine/status")
    mqtt_client.on_message = on_message
    mqtt_client.loop_forever()

if __name__ == "__main__":
    mqtt_thread()
