window.onload = function () {
    if (typeof Paho === 'undefined') {
        console.error("Biblioteca Paho MQTT não carregada.");
        return;
    }

    // Função para formatar tempo (segundos -> Xm YYs)
    function formatarTempo(segundos) {
        const minutos = Math.floor(segundos / 60);
        const restoSegundos = segundos % 60;
        return `${minutos}m ${restoSegundos.toString().padStart(2, '0')}s`;
    }

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
        try {
            const dados = JSON.parse(message.payloadString);

            // Atualiza os elementos HTML
            document.getElementById("cadencia").innerText = dados.lowSignalCount ?? "--";
            document.getElementById("tempoProducao").innerText = formatarTempo(dados.cadenceTotalTime ?? 0);
            document.getElementById("tempoParado").innerText = formatarTempo(dados.nonCadenceTotalTime ?? 0);
        } catch (err) {
            console.error("Erro ao processar a mensagem:", err);
        }
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
};