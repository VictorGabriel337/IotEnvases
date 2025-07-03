const ctx = document.getElementById('myChart');

// ⚠️ Aqui a gente salva o gráfico numa variável que vamos usar depois
const myChart = new Chart(ctx, {
  type: 'bar',
  data: {
    labels: ['03/07', '04/07', '05/07', '07/07', '08/07', '09/07'],
    datasets: [{
      label: 'Amostra de Dados',
      data: [12, 19, 3, 5, 2, 3],
      backgroundColor: [
        'rgba(255, 99, 132, 0.8)',
        'rgba(54, 162, 235, 0.8)',
        'rgba(255, 206, 86, 0.8)',
        'rgba(75, 192, 192, 0.8)',
        'rgba(153, 102, 255, 0.8)',
        'rgba(255, 159, 64, 0.8)'
      ],
      borderColor: [
        'rgba(255, 99, 132, 1)',
        'rgba(54, 162, 235, 1)',
        'rgba(255, 206, 86, 1)',
        'rgba(75, 192, 192, 1)',
        'rgba(153, 102, 255, 1)',
        'rgba(255, 159, 64, 1)'
      ],
      borderWidth: 1
    }]
  },
  options: {
    responsive: true,
    scales: {
      y: {
        beginAtZero: true
      }
    }
  }
});

// 🧠 Escuta mensagem do iframe pai para atualizar os dados
window.addEventListener('message', (event) => {
  if (event.data.tipo === 'atualizarFi') {
    const fi = event.data.fi;

    // Atualiza os dados e os rótulos se necessário
    myChart.data.datasets[0].data = fi;

    // (Opcional) Atualizar labels com base em quantidade de dados
    myChart.data.labels = fi.map((_, i) => `Classe ${i + 1}`);

    myChart.update();
  }
});
