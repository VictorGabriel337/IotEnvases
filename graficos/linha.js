const ctx = document.getElementById('linha');

new Chart(ctx, {
  type: 'line',
  data: {
    labels: ['Jun', 'Jul', 'Ago', 'Set', 'Nov', 'Dez'],
    datasets: [{
      label: 'Eficiencia',
      data: [12, 19, 3, 5, 2, 3],
      borderWidth: 1
    }]
  },
  options: {
    scales: {
      y: {
        beginAtZero: true
      }
    }
  }
});