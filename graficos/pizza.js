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

    // Evita divisão por zero
    const porcentagemParada = total > 0 ? (parada / total) * 100 : 0;
    const porcentagemProduzindo = total > 0 ? (produzindo / total) * 100 : 0;

    chartPizza.data.datasets[0].data = [porcentagemParada, porcentagemProduzindo];
    chartPizza.update();
  }
});
