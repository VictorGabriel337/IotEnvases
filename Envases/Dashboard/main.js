 // Substituí o IP do ESP8266 aqui
 const backendURL = "https://iotenvases.onrender.com/sensores";
 console.log(" Script carregado!");

 async function fetchSensorData() {
  try {
      const response = await fetch(backendURL);
      if (!response.ok) {
          throw new Error("Erro na resposta do servidor");
      }
      const data = await response.json();
      console.log("Dados recebidos:", data); // Log para verificar os dados

      // Verificar se 'message' está presente nos dados
      if (data.message) {
          document.getElementById("lowCount").textContent = data.message;
          document.getElementById("maqProduzindo").textContent = data.message;
          document.getElementById("maqParada").textContent = data.message;
      } else {
          // Caso contrário, continuar tratando os dados conforme esperado
          if (data.lowSignalCount !== undefined) {
              document.getElementById("lowCount").textContent = ` ${data.lowSignalCount}`;
          }

          if (data.cadenceTotalTime !== undefined) {
              document.getElementById("maqProduzindo").textContent = ` ${formatarTempo(data.cadenceTotalTime)}`;
          }

          if (data.nonCadenceTotalTime !== undefined) {
              document.getElementById("maqParada").textContent = ` ${formatarTempo(data.nonCadenceTotalTime)}`;
          }
      }

  } catch (error) {
      console.error("Erro ao buscar os dados:", error);
      document.getElementById("lowCount").textContent = "Erro ao exibir";
      document.getElementById("maqProduzindo").textContent = "Erro ao exibir";
      document.getElementById("maqParada").textContent = "Erro ao exibir";
  }
}
