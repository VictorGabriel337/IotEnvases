# IotEnvases
Sistema IoT para Monitoramento de Cádencia de Produção
Este repositório contém o código e a documentação do sistema de monitoramento de cádencia de produção, desenvolvido com a plataforma ESP8266. O sistema foi projetado para capturar e monitorar em tempo real os tempos de operação de uma máquina, distinguindo entre o tempo de produção e o tempo de máquina parada, com o objetivo de otimizar os processos industriais e melhorar a eficiência operacional.

Objetivo do Sistema
O sistema foi desenvolvido para coletar dados de sensores instalados em máquinas industriais, registrando o tempo que a máquina permanece produzindo e o tempo que ela fica parada. Com base nesses dados, é possível calcular a cádencia de produção, permitindo que gestores e operadores monitorem o desempenho das máquinas e identifiquem possíveis áreas de melhoria na linha de produção.

Funcionalidades Principais
Monitoramento de Tempo de Produção: Através do sensor conectado ao ESP8266, o sistema contabiliza o tempo em que a máquina está em operação e produza de forma eficiente.
Monitoramento de Tempo de Máquina Parada: O sistema também detecta quando a máquina está inativa, registrando esse tempo para análise posterior.
Envio de Dados para a Nuvem: Os dados coletados pelo sensor são enviados periodicamente para a nuvem para visualização em tempo real e análise histórica.
Cálculo da Cádencia: Com base nos tempos registrados de operação e inatividade, o sistema calcula a cádencia de produção, permitindo a avaliação da eficiência da máquina.
Interface de Visualização (Opcional): Painel de controle simples para visualização dos dados de cádencia, tempos de operação e inatividade em tempo real.
Benefícios
Otimização de Produção: Ao monitorar a cádencia em tempo real, o sistema permite que os gestores identifiquem rapidamente quando uma máquina está parada mais do que o esperado, permitindo ações corretivas.
Aumento da Eficiência Operacional: A coleta precisa de dados sobre os tempos de operação e inatividade contribui para otimizar os processos e melhorar o tempo de ciclo da produção.
Redução de Custos: Com o acompanhamento constante das máquinas, é possível reduzir o tempo ocioso e melhorar o aproveitamento dos recursos de produção.
Análise de Dados para Melhoria Contínua: O sistema permite o envio de dados históricos para análise posterior, auxiliando na implementação de melhorias no processo produtivo.
Tecnologias Utilizadas
ESP8266: Microcontrolador utilizado para captura dos dados do sensor e envio para a nuvem.
Sensor de Tempo (Especificar o modelo): Sensor utilizado para identificar quando a máquina está em operação ou parada.
Nuvem (Exemplo: MQTT, Firebase, etc.): Serviço de nuvem utilizado para o armazenamento e visualização dos dados coletados.
Linguagem de Programação: [Exemplo: Arduino IDE, C++]
Estrutura do Repositório
