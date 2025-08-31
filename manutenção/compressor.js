
  const menuItems = document.querySelectorAll('.menu div');
  const rightPanel = document.querySelector('.right-panel');

  // Cria um container dinâmico acima do menu
  let dynamicContent = rightPanel.querySelector('.dynamic-content');
  if(!dynamicContent){
      dynamicContent = document.createElement('div');
      dynamicContent.classList.add('dynamic-content');
      rightPanel.insertBefore(dynamicContent, rightPanel.querySelector('.menu'));
  }

  // Conteúdo original (indicadores e barras)
  const originalContent = `
     <h2>Controle Sensores</h2>
        <canvas id="paramChart" width="400" height="70"></canvas>
     <br>
      <div class="stats">
      
          <div class="circle-card">
              <div class="circle" style="--percent:72">72%</div>
              <p>Pressão</p>
              <p>0.0Bar</p>
          </div>
          <div class="circle-card">
              <div class="circle" style="--percent:86">86%</div>
              <p>Temperatura</p>
              <p>0ºC</p>
          </div>
          <div class="circle-card">
              <div class="circle" style="--percent:45">45%</div>
              <p>Vibração</p>
              <p>0.00mm/s</p>
          </div>
      </div>

      <div class="progress-section">
          <div class="bar">
              <span>Óleo</span>
              <div class="progress"><div style="width:90%"></div></div>
          </div>
          <div class="bar">
              <span>Filtro</span>
              <div class="progress"><div style="width:60%"></div></div>
          </div>
          <div class="bar">
              <span>Eficiência</span>
              <div class="progress"><div style="width:75%"></div></div>
          </div>
      </div>

      <div class="main-progress">
          <span>Geral</span>
          <div class="progress"><div style="width:100%"></div></div>
          <span>82%</span>
      </div>
  `;

  // Inicializa com o conteúdo original
  dynamicContent.innerHTML = originalContent;

  menuItems.forEach(item => {
      item.addEventListener('click', () => {
          const text = item.textContent.trim();

          if(text.includes('Parâmetros')){
    // Substitui conteúdo por gráfico de linha
    dynamicContent.innerHTML = `
        <h2>Gráfico de Parâmetros</h2>
        <canvas id="paramChart" width="400" height="300"></canvas>
    `;

    const ctx = document.getElementById('paramChart').getContext('2d');
    new Chart(ctx, {
        type: 'line',
        data: {
            labels: ['00:00', '01:00', '02:00', '03:00', '04:00', '05:00', '06:00', '07:00', '08:00',
                    '08:00', '09:00', '10:00', '11:00', '12:00', '13:00', '14:00', '15:00', '16:00', 
                    '17:00', '18:00', '19:00', '20:00', '21:00', '22:00', '23:00'
            ], // meses
            datasets: [
                {
                    label: 'Pressão',
                    data: [72, 74, 70, 73, 75, 72, 0 , 0, 0, 0, 80, 70, 100, 90, 90, 90, 0, 0, 0, 0, 0, 0, 0], 
                    borderColor: 'rgba(255, 0, 0, 0.8)',
                    backgroundColor: 'rgba(255, 0, 0, 0.2)',
                    tension: 0.3
                },
                {
                    label: 'Temperatura',
                    data: [86, 88, 85, 87, 86, 89],
                    borderColor: 'rgba(0, 0, 255, 0.8)',
                    backgroundColor: 'rgba(0, 0, 255, 0.2)',
                    tension: 0.3
                },
                {
                    label: 'Vibração',
                    data: [45, 50, 42, 48, 46, 45],
                    borderColor: 'rgba(0, 128, 0, 0.8)',
                    backgroundColor: 'rgba(0, 128, 0, 0.2)',
                    tension: 0.3
                }
            ]
        },
        options: { 
            responsive: true, 
            scales: { y: { beginAtZero: true } },
            plugins: {
                legend: { position: 'top' },
                title: { display: true, text: 'Parâmetros do Compressor' }
            }
        }
    });
}

          if(text.includes('Controle')){
              // Restaura o conteúdo original
              dynamicContent.innerHTML = originalContent;
          }
      });
  });