# ChupinVet - VetTemp

## INFORMAÇÕES DO GRUPO

**Nome:** Agatha Yie Won Yun  
**RM:** 561507  
**Turma:** 2TDSA

**Nome:** Ana Claudia Fernandes Martins  
**RM:** 561190 
**Turma:** 2TDSR

**Nome:** Anabelle Rosseto Rodrigues  
**RM:** 564526 
**Turma:** 2TDSR

**Nome:** Samantha Faruolo Galdi  
**RM:** 554794 
**Turma:** 2TDSA

**Nome:** Vitor Fria Dalmagro  
**RM:** 566052 
**Turma:** 2TDSA

---
## Sobre o VetTemp

A proposta desse projeto é um **monitor de temperatura para a sala dos pets**. Ele lê o DHT22, mostra o LCD, usa LEDs para alerta de Frio (Led Azul) e Calor (Led Vermelho), pega temperatura de São Paulo por API e ainda cria uma API própria em /api/status.

## Hardware utilizado

O circuito do projeto é o seguinte:

## Instruções de Uso

1. Clonar o repositório
- git clone 
- cd VetTemp_IoT
- code .

2. Rodar o código no Arduino
- Abra o vettemp.ino no Arduino
- Instale as bibliotecas no Arduino
- Vá em Sketch e Export Compiled Binary
- Depois que compilado, volte ao VsCode e abra o Diagram.json
- Você vai precisar pegar a License Key do Wokwi
- Depois, apenas clique no botão verde para rodar o projeto

3. Como Funciona?
- Logo que iniciado, na tela LCD aparece a mensagem "VetTemp Iniciando..."
- Logo após, aparecerá as informações de Temperatura(C) e Umidade(U) no LCD
> Sobre os leds: 
- Quando a temperatura está acima de 29 C° o led vermelho pisca
- Quando a temperatura está abaixo de 19 C° o led azul pisca
- O botão muda entre a tela de temperatura da sala e entre a temperatura externa (SP)

## Tecnologias Usadas

## Resultados Parciais