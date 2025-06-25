const ctx = document.getElementById('pizza');
let chartPizza = new Chart(ctx, {
  type: 'doughnut',
  data: {
    labels: ['Parada', 'Produzindo'],
    datasets: [{
      data: [0.5, 0.5], // valores iniciais (50/50)
      backgroundColor: ['#36A2EB', '#FF6384'],
      borderWidth: 1
    }]
  },
  options: {
    responsive: true,
    plugins: {
      legend: {
        position: 'bottom',
        labels: {
          boxWidth: 20,
          padding: 15,
          usePointStyle: true
        }
      },
      tooltip: {
        callbacks: {
          label: function(context) {
            const label = context.label || '';
            const dataset = context.chart.data.datasets[0];
            const totalRaw = dataset._rawDataTotal || 0;
            const originalValues = dataset._originalData || [0, 0];
            const index = context.dataIndex;
            const valorSegundos = originalValues[index];

            const minutos = Math.floor(valorSegundos / 60);
            const segundos = Math.floor(valorSegundos % 60);

            return `${label}: ${minutos}m ${segundos}s`;
          }
        }
      }
    },
    layout: {
      padding: {
        top: 10,
        bottom: 10
      }
    }
  }
});

window.addEventListener('message', (event) => {
  if (event.data.type === 'atualizarPizza') {
    const parada = event.data.pMenor;
    const produzindo = event.data.pMaior;

    const total = parada + produzindo;

    const porcentagemParada = total > 0 ? (parada / total) * 100 : 0;
    const porcentagemProduzindo = total > 0 ? (produzindo / total) * 100 : 0;

    // Salva os valores originais em segundos para o tooltip
    chartPizza.data.datasets[0]._originalData = [parada, produzindo];
    chartPizza.data.datasets[0]._rawDataTotal = total;

    chartPizza.data.datasets[0].data = [porcentagemParada, porcentagemProduzindo];
    chartPizza.update();
  }
});
