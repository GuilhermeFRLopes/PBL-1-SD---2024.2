/*Claro! Vou completar o código para que você possa criar e desenhar a peça em forma de "L" no monitor VGA. Vou adicionar a lógica para criar a peça, desenhá-la na tela e também incluir um loop para manter a tela atualizada. Aqui está o código completo:

```c*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intelfpgaup/video.h>
#include <time.h>
#include <unistd.h>

#define BLOCO_TAM 10        // Tamanho de cada quadrado
#define LARGURA_TELA 319    // Largura da tela VGA
#define ALTURA_TELA 239     // Altura da tela VGA
#define MAX_PECAS 10        // Número máximo de peças que podem ser criadas

typedef struct {
    int pos_x, pos_y;    // Posição relativa do quadrado dentro da peça
    short cor;           // Cor do quadrado
    bool ativo;          // Indica se o quadrado faz parte da peça (ativo ou não)
} Quadrado;

typedef struct {
    int tam_x, tam_y;       // Dimensões da peça (número de quadrados)
    int pos_x, pos_y;       // Posição da peça no tabuleiro
    Quadrado quadrados[4][4]; // Matriz de quadrados, 4x4 é suficiente para qualquer peça de Tetris
} Peca;

typedef struct {
    int largura, altura;   // Dimensões do tabuleiro
    short cor_fundo;       // Cor de fundo do tabuleiro
    Quadrado matriz[ALTURA_TELA / BLOCO_TAM][LARGURA_TELA / BLOCO_TAM]; // Matriz para armazenar as peças no tabuleiro
} Tabuleiro;

Peca criarPecaL(short cor) {
    Peca pecaL;
    pecaL.pos_x = (LARGURA_TELA / 2) - (BLOCO_TAM * 1); // Centraliza a peça
    pecaL.pos_y = 0; // Começa no topo da tela
    pecaL.tam_x = 3;
    pecaL.tam_y = 2;

    // Inicializa a matriz de quadrados para a peça "L"
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            pecaL.quadrados[i][j].ativo = false; // Inicializa todos os quadrados como inativos
        }
    }

    // Definindo os quadrados ativos para a peça "L"
    pecaL.quadrados[0][0] = (Quadrado){0, 0, cor, true}; // Quadrado superior esquerdo
    pecaL.quadrados[1][0] = (Quadrado){1, 0, cor, true}; // Quadrado abaixo do primeiro
    pecaL.quadrados[2][0] = (Quadrado){2, 0, cor, true}; // Quadrado abaixo do segundo
    pecaL.quadrados[2][1] = (Quadrado){2, 1, cor, true}; // Quadrado à direita do terceiro

    return pecaL;
}

Tabuleiro criarTabuleiro() {
    Tabuleiro tab;
    tab.largura = LARGURA_TELA / BLOCO_TAM;
    tab.altura = ALTURA_TELA / BLOCO_TAM;
    tab.cor_fundo = video_WHITE;    

    // Inicializa a matriz do tabuleiro com a cor de fundo
    for (int i = 0; i < tab.altura; i++) {
        for (int j = 0; j < tab.largura; j++) {
            tab.matriz[i][j].ativo = false;
            tab.matriz[i][j].cor = tab.cor_fundo;
        }
    }

    return tab;
}

void desenhaTabuleiro(Tabuleiro tab) {
    for (int i = 0; i < tab.altura; i++) {
        for (int j = 0; j < tab.largura; j++) {
            video_box(j * BLOCO_TAM, i * BLOCO_TAM, (j + 1) * BLOCO_TAM - 1, (i + 1) * BLOCO_TAM - 1, tab.matriz[i][j].cor);
        }
    }
}

void desenhaPeca(Peca peca) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (peca.quadrados[i][j].ativo) {
                int x = peca.pos_x + j * BLOCO_TAM;
                int y = peca.pos_y + i * BLOCO_TAM;
                video_box(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, peca.quadrados[i][j].cor);
            }
        }
    }
}

short corAleatoria() {
    short cores[] = {video_WHITE, video_YELLOW, video_RED, video_GREEN, video_BLUE, video_CYAN, video_MAGENTA, video_GREY, video_PINK, video_ORANGE};

    srand(time(NULL));  // Semente para o gerador de números aleatórios

    int numAleatorio = rand() % 10; // Gerar um número aleatório entre 0 e 9

    return cores[numAleatorio];
}

bool verificaColisao(Tabuleiro tab, Peca peca, int dx, int dy) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (peca.quadrados[i][j].ativo) {
                int nova_x = (peca.pos_x / BLOCO_TAM) + j + dx;
                int nova_y = (peca.pos_y / BLOCO_TAM) + i + dy;

                // Verifica colisão com as bordas do tabuleiro
                if (nova_x < 0 || nova_x >= tab.largura || nova_y >= tab.altura) {
                    return true;
                }

                // Verifica colisão com outras peças no tabuleiro
                if (tab.matriz[nova_y][nova_x].ativo) {
                    return true;
                }
            }
        }
    }
    return false;
}

void fixarPecaNoTabuleiro(Tabuleiro *tab, Peca peca) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (peca.quadrados[i][j].ativo) {
                int x = (peca.pos_x / BLOCO_TAM) + j;
                int y = (peca.pos_y / BLOCO_TAM) + i;
                tab->matriz[y][x] = peca.quadrados[i][j]; // Fixa a peça no tabuleiro
            }
        }
    }
}

void moverPeca(Peca *peca, int dx, int dy) {
    peca->pos_x += dx;
    peca->pos_y += dy;
}

int main() {
    video_open();    // Inicializa o vídeo
    video_clear();   // Limpa o vídeo

    Tabuleiro tab = criarTabuleiro();
    desenhaTabuleiro(tab);

    Peca pecaAtual = criarPecaL(corAleatoria());
    desenhaPeca(pecaAtual);

    video_show();    // Mostra o conteúdo na tela

    // Loop para movimentação da peça
    while (1) {
        if (!verificaColisao(tab, pecaAtual, 0, 1)) {
            moverPeca(&pecaAtual, 0, BLOCO_TAM); // Move a peça para baixo
        } else {
            fixarPecaNoTabuleiro(&tab, pecaAtual); // Fixa a peça no tabuleiro
            pecaAtual = criarPecaL(corAleatoria()); // Cria uma nova peça
        }

        video_clear();  // Limpa a tela

        desenhaTabuleiro(tab); // Redesenha o tabuleiro
        desenhaPeca(pecaAtual); // Desenha a peça na nova posição

        video_show();  // Atualiza a tela
        usleep(500000); // Delay para controlar a velocidade de movimento

        // Verifica se a peça atingiu o fundo ou colidiu com outra peça
        if (pecaAtual.pos_y + pecaAtual.tam_y * BLOCO_TAM >= ALTURA_TELA) {
            break; // Sai do loop se a peça alcançar o fundo
        }
    }

    video_close();  // Fecha o vídeo

    return 0;
}


/*
```

### Explicações das Adições:
1. **Definições de Cores**: Adicionei definições para as cores que você pode usar.
2. **Tamanho do Bloco**: Defini `BLOCO_TAM` para o tamanho de cada quadrado da peça.
3. **Posição Inicial da Peça**: A posição inicial da peça "L" foi ajustada para centralizá-la na tela.
4. **Loop Infinito**: Um loop infinito foi adicionado para manter a tela atualizada. Você pode adicionar lógica para movimentar a peça ou outras interações dentro desse loop.

Agora, ao executar o código, a peça "L" deve ser desenhada na tela VGA com uma cor aleatória. Você pode expandir a lógica para incluir movimentação e outras funcionalidades do jogo Tetris.*/
