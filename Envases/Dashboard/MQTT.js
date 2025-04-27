const host = "534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud";
const port = 8884; // Porta WSS segura
const username = "Iotenvases";
const password = "Iotenvases42";
const topic = "machine/status";

const clientId = "client-" + Math.random().toString(16).substr(2, 8);

const client = new Paho.MQTT.Client(host, port, "/mqtt", clientId);

client.onConnectionLost = function (responseObject) {
    console.log("Conexão perdida:", responseObject.errorMessage);
  };

client.onMessageArrived = function (message) {
    console.log("Mensagem recebida:", message.payloadString);
    const dados = JSON.parse(message.payloadString);

    // Atualiza os elementos na página
    document.getElementById("cadencia").innerText = dados.lowSignalCount ?? "--";
    document.getElementById("tempoProducao").innerText = dados.cadenceTotalTime ?? "--";
    document.getElementById("tempoParado").innerText = dados.nonCadenceTotalTime ?? "--";
  };

  client.connect({
    useSSL: true,
    userName: username,
    password: password,
    onSuccess: function () {
      console.log("Conectado ao broker MQTT com sucesso!");
      client.subscribe(topic);
    },
    onFailure: function (err) {
      console.error("Erro ao conectar:", err);
    }
  });