# Projeto de Jogo Tetris em Linguagem C

## Problema | - TEC499 - MI Sistemas Digitais - 2024.2 

Professor: Wild Freitas da Silva Santos

Grupo: [Guilherme Ferreira Rocha Lopes](https://github.com/GuilhermeFRLopes), [Thiago Ramon Santos de Jesus](https://github.com/lithiago) e [Ícaro de Souza Gonçalves](https://github.com/icarosg)

## Seções

1. [Introdução](#introdução)
2. [Hardware Utilizado](#hardware-utilizado)
3. [Software Utilizado](#software-utilizado)
4. [Metodologia](#metodologia)
5. [Documentação Utilizada](#documentação-utilizada)
6. [Testes realizados](#testes-realizados)
7. [Problemas](#problemas)
8. [Execução do jogo](#execução-do-jogo)

## Introdução

Este documento descreve em detalhes o desenvolvimento de um jogo de quebra-cabeça  conhecido como Tetris. Utiliza a linguagem C e é implementado na placa de desenvolvimento modelo [DE1-SoC da terasIC](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=167&No=836#contents).

O projeto consiste em um jogo de Tetris, onde peças de diferentes formatos caem do topo do espaço de jogo. O jogador pode mover as peças horizontalmente para que se encaixem nas linhas na parte inferior da tela. Quando uma linha é completada, ela é eliminada, permitindo que novas peças continuem caindo. O jogo dispõe de opções de pausa e reinício que são acessadas através dos botões, e a exibição do jogo é feita em um monitor de tubo.

## Hardware Utilizado

O projeto em questão faz uso de hardware específico para seu desenvolvimento, sendo empregada uma placa de desenvolvimento DE1-SoC da terasIC . Ela é  ideal para diversos projetos de prototipagem e desenvolvimento de sistemas embarcados. Ela combina um FPGA Cyclone V SoC da Intel com um processador dual-core ARM Cortex-A9, oferecendo uma plataforma completa e flexível para implementação de hardware e software. Seu acesso para a execução do jogo é feito através da conexão via SSH (ethernet) no terminal de um computador.

### teasIC DE1-SoC - Especificações

![1708714739472](image/README/1708714739472.png)

| Categoria                               | Detalhes                                       |
| --------------------------------------- | ---------------------------------------------- |
| **FPGA**                          | Cyclone V SoC 5CSEMA5F31C6                     |
| Logic Elements                          | 85K                                            |
| Memória Embarcada                      | 4,450 Kbits                                    |
| PLLs Fracionais                         | 6                                              |
| Controladores de Memória               | 2                                              |
| **Configuração e Depuração**  | Dispositivo de Configuração Serial (EPCS128) |
| On-Board                                | USB Blaster II                                 |
| **Memória**                      | 64MB SDRAM                                     |
| DDR3 SDRAM                              | 1GB                                            |
| Micro SD                                | Sim                                            |
| **Comunicação**                 | 2 Portas USB 2.0                               |
| UART para USB                           | Sim                                            |
| Ethernet                                | 10/100/1000                                    |
| PS/2                                    | Mouse/Teclado                                  |
| IR                                      | Emissor/Receptor                               |
| **Conectores**                    | 2 Headers de Expansão (3.3V)                  |
| Entrada ADC                             | 10 pinos                                       |
| Conector LTC                            | SPI Master, I2C, GPIO                          |
| **Display**                       | VGA DAC 24-bit                                 |
| **Áudio**                        | CODEC 24-bit                                   |
| Entrada de Vídeo                       | Decodificador TV                               |
| **ADC**                           | Taxa de Amostragem: 500 KSPS                   |
| Canais: 8                               |                                                |
| Resolução: 12 bits                    |                                                |
| Range: 0 ~ 4.096 V                      |                                                |
| **Botões, Interruptores e LEDs** | 4 Teclas de Usuário (FPGA)                    |
| 10 Interruptores de Usuário (FPGA)     |                                                |
| 11 LEDs de Usuário (FPGA + HPS)        |                                                |
| 2 Botões de Reset (HPS)                |                                                |
| 6 Displays 7 segmentos                  |                                                |
| **Sensores**                      | G-Sensor                                       |
| **Energia**                       | Entrada DC 12V                                 |

| Categoria                               | Detalhes                                       |
| --------------------------------------- | ---------------------------------------------- |
| **FPGA**                          | Cyclone V SoC 5CSEMA5F31C6                     |
| Logic Elements                          | 85K                                            |
| Memória Embarcada                      | 4,450 Kbits                                    |
| PLLs Fracionais                         | 6                                              |
| Controladores de Memória               | 2                                              |
| **Configuração e Depuração**  | Dispositivo de Configuração Serial (EPCS128) |
| On-Board                                | USB Blaster II                                 |
| **Memória**                      | 64MB SDRAM                                     |
| DDR3 SDRAM                              | 1GB                                            |
| Micro SD                                | Sim                                            |
| **Comunicação**                 | 2 Portas USB 2.0                               |
| UART para USB                           | Sim                                            |
| Ethernet                                | 10/100/1000                                    |
| PS/2                                    | Mouse/Teclado                                  |
| IR                                      | Emissor/Receptor                               |
| **Conectores**                    | 2 Headers de Expansão (3.3V)                  |
| Entrada ADC                             | 10 pinos                                       |
| Conector LTC                            | SPI Master, I2C, GPIO                          |
| **Display**                       | VGA DAC 24-bit                                 |
| **Áudio**                        | CODEC 24-bit                                   |
| Entrada de Vídeo                       | Decodificador TV                               |
| **ADC**                           | Taxa de Amostragem: 500 KSPS                   |
| Canais: 8                               |                                                |
| Resolução: 12 bits                    |                                                |
| Range: 0 ~ 4.096 V                      |                                                |
| **Botões, Interruptores e LEDs** | 4 Teclas de Usuário (FPGA)                    |
| 10 Interruptores de Usuário (FPGA)     |                                                |
| 11 LEDs de Usuário (FPGA + HPS)        |                                                |
| 2 Botões de Reset (HPS)                |                                                |
| 6 Displays 7 segmentos                  |                                                |
| **Sensores**                      | G-Sensor (Acelerômetro)                       |
| **Energia**                       | Entrada DC 12V                                 |

### Monitor de Tubo - Especificações

![1708605463822](image/README/1708605463822.png)

|            Característica            |      Detalhes      |
| :------------------------------------: | :----------------: |
|       **Tipo de Display**       |      Vertical      |
| **Tamanho da Tela (Horizontal)** |      32,4 cm      |
|  **Tamanho da Tela (Vertical)**  |      24,6 cm      |
|  **Tamanho da Tela (Diagonal)**  |      40,6 cm      |
|      **Varredura Vertical**      |    50 - 160 Hz    |
|     **Varredura Horizontal**     |    30 - 70 kHz    |
|  **Tamanho da Tela (Diagonal)**  |   43,2 cm (17")   |
|          **Dot Pitch**          |  0,270 x 0,270 mm  |
|     **Resolução Máxima**     | 1280 x 1024 pixels |
|    **Resolução do Display**    | 1024 x 768 pixels |

## Software utilizado

Para o desenvolvimento e execução dos códigos, o Visual Studio Code foi utilizado como ferramenta de escrita.

Visual Studio Code (VS Code): é um editor de código-fonte gratuito e de código aberto desenvolvido pela Microsoft. É multiplataforma, altamente extensível, oferece integração com Git, suporte a várias linguagens de programação, ferramentas de depuração integradas e um terminal incorporado.

## Metodologia

### Fluxograma do funcionamento do sistema

O fluxograma abaixo apresenta a maneira como o sistema se comporta de forma geral durante a execução do projeto:

![1708688254235](image/README/1708688254235.png)

**1. Inicialização:**

* Placa de desenvolvimento (FPGA)
* Monitor CRT
* Computador

**2. Conexão:**

* O computador é conectado à FPGA para execução do jogo.

**3. Inicialização do jogo:**

* São inicializadas variáveis e elementos do jogo através do terminal.
* Inicio da execução do jogo na FPGA.

**4. Jogatina:**

* Segue-se o fluxo do jogo ([Ver imagem do fluxograma abaixo](#Fluxograma-do-funcionamento-do-jogo)).

**5. Encerramento:**

* Encerra-se a execução do jogo no terminal caso o usuário não queira jogar novamente.

**6. Desconexão e desligamento:**

* Desconecta-se e desliga-se devidamente os periféricos .

### Fluxograma do funcionamento do jogo

O fluxograma abaixo apresenta a maneira como o projeto do jogo foi elaborado:

![1708633969184](image/README/1708633969184.png)

O processo de funcionamento do jogo acontece da seguinte forma:

**Início:**

* A peça é iniciada na parte superior e no meio do tabuleiro do jogo.
* O tabuleiro não compõe o monitor inteiro, o qual fica posicionado no canto esquerdo, com seus necessários tamanhos, sendo a parte vertical maior que a horizontal.

**Movimento da peça:**

* A peça se move livremente pelo tabuleiro.
* Ao colidir com blocos ou bordas:
  * Se for na borda superior, simplesmente a peça é fixada.
  * Se colidir com uma das bordas laterais, a peça não se move mais além do limite.
* Ao colidir com as paredes laterais, o movimento da peça é restringido.
* Se a peça chegar à parte inferior do tabuleiro:
  * O jogador deve posicionar as peças de forma estratégica para completar linhas.
  * Se o jogador não conseguir completar linhas a tempo e as peças acumularem até o topo, o jogo termina.

**Objetivo:**

* Completar as linhas e fazer o máximo de score possível.

**Pausa, retomada e reinicio:**

* O jogador pode pausar e retomar o jogo a qualquer momento pressionando qualquer botão presente na placa ([Ver imagem da placa](#teasic-de1-soc---especificações)).
* O jogador pode resetar o jogo após mensagem de gamer over, pressionando qualquer botão presente na placa ([Ver imagem da placa](#teasic-de1-soc---especificações)).

### Solução do Problema

Para a solução do problema foi utilizada a linguagem de programação C, bem como a utilização da interface de desenvolvimento (IDE) Visual Studio Code, para a elaboração dos códigos fonte. O projeto foi sintetizado utilizando um computador de placa única, o DE1-Soc, ao qual foi conectado um monitor de tubo Dell para exibição do jogo.

#### Bibliotecas utilizadas

Para auxiliar no desenvolvimento do código em C do Jogo foram utilizadas as seguintes bibliotecas: **Stdio**, **stdlib**, **stdint**, **time**, **sys/mman**, **fcntl**, **Stdbool**, **Intelfpgaup/KEY** e **Intelfpgaup/video**.

```
├── stdio.h
├── stdlib.h
├── stdint.h
├── time.h
├── sys/mman.h
├── fcntl.h
├── stdbool.h
├── intelfpgaup/KEY.h
├── intelfpgaup/video.h
```

As bibliotecas **Stdio** **stdlib**, **stdint**, **time**, **Stdbool** fornecem funções para diversas tarefas em C, como manipulação de entrada/saída, operações de baixo nível em sistemas Unix-like e tipos booleanos.

**sys/mman:** Essa biblioteca fornece funções para gerenciamento de memória, especialmente para mapeamento de arquivos ou dispositivos na memória. As principais funções dessa biblioteca incluem:
  * **mmap():** Permite mapear arquivos ou dispositivos para a memória do processo, facilitando o acesso aos dados sem precisar usar funções tradicionais de leitura e escrita. Isso é útil para melhorar o desempenho ao trabalhar com grandes quantidades de dados.

* **munmap():** Desfaz o mapeamento de um bloco de memória previamente mapeado.

* **mprotect():** Modifica as permissões de acesso a um bloco de memória.

* Essa biblioteca é frequentemente usada em aplicações que exigem acesso  rápido e eficiente a grandes volumes de dados ou para manipulação direta  de memória.

## `#include <fcntl.h>`

Essa biblioteca contém definições relacionadas a arquivos e controladores de entrada/saída. As principais funções e constantes que ela oferece incluem:

- **`open`**: Abre um arquivo ou dispositivo, retornando um descritor de arquivo que pode ser usado para operações de leitura, escrita e controle.

- **`O_CREAT`, `O_RDONLY`, `O_WRONLY`, `O_RDWR`, etc.**: Constantes usadas como flags para especificar como o arquivo deve ser aberto (por exemplo, se ele deve ser criado, se deve ser aberto apenas para leitura, etc.).

- **`fcntl`**: Permite modificar propriedades de um descritor de arquivo, como bloqueios ou modos de operação.


**intelfpgaup/KEY:**  Oferece funções para a utilização dos botões (KEYS) da placa tais como as utilizadas:

* **KEY_open():** abre o dispositivo para os botões;
* **KEY_read():** lê as entradas recebidas pelo dispositivo para os botões;
* **KEY_close():** fecha o dispositivo para os botões.

**Intelfpgaup/video:**  Oferece funções para utilização do VGA da placa para criação de gráficos, com funções para desenho de linhas, retângulos e texto. As utilizadas foram:

* **video_open():** abre o dispositivo de vídeo VGA;
* **video_erase():** limpa todos e apenas os textos que estivessem escritos na tela;
* **video_clear():** limpa todas as imagens que estivessem exibidas na tela;
* **video_show():** exibe na tela todos os elementos dispostos no buffer de video, tais como textos ou desenhos (linhas ou retângulos/boxes);
* **video_text():** dispõe no buffer de vídeo um texto de tamanho fixo a ser exibido na tela, selecionando-se sua posição em eixos X e Y;
* **video_box():** dispõe no buffer de vídeo um retângulo/box a ser exibido na tela, selecionando-se os pontos iniciais e finais em coordenadas de X e Y e a cor da figura a ser gerada;
* **video_close():** fecha o dispositivo de vídeo VGA.

### Telas do jogo

Abaixo são apresentadas as telas de inicio, execução do jogo, pausa e fim do jogo com vitória (You Win!) e derrota (You Lose!) e pause.

![1708711366769](image/README/1708711366769.png)![1708711382866](image/README/1708711382866.png)

## Documentação utilizada

[**Manual de usuário da placa de desenvolvimento DE1-SoC:** ](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=167&No=836&PartNo=4#contents)Contém todas as informações relacionadas ao funcionamento da DE1-SoC, bem como são realizadas a integração da placa com demais recursos (video externo e acelerômetro).

**[Manual de funções do Linux no ARM A9](https://fpgacademy.org/tutorials.html):** Utilizado para consulta das funções que foram implementadas no código fonte do jogo.

## Testes Realizados

Abaixo são apresentados alguns testes realizados com o proposito de verificar o funcionamento do projeto:

![1708711980034](image/README/1708711980034.png)

Na imagem 1 é apresentada a tela de inicio com a mensagem: "---START---!", na imagem 2 é apresentada o pause durante o jogo e na terceira imagem é apresentado a tela enquanto o jogo está sendo jogado.

No link a seguir são apresentados videos do funcionamento do jogo: [link aqui](https://imgur.com/a/LQylspQ)

## Problemas

Durante o desenvolvimento do jogo, foi apresentado como problema que, no momento em que a bola passa por  baixo da raquete, a bola ricocheteia entre a parte inferior da raquete e a parte inferior da tela do jogo (chão) antes de encerrar o jogo como derrota (YOU LOSE!).

## Execução do jogo

Em posse do código desse repositório e de uma placa de desenvolvimento DE1-SoC conectada a um monitor, para testar o funcionamento do programa, conecte a placa com um computador e execute o seguinte comando no terminal:

```
make all
```

#### [Voltar ao topo](#Projeto-de-jogo-Brick-Break-em-Linguagem-C)
