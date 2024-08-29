/*Claro! Vou completar o código para que você possa criar e desenhar a peça em forma de "L" no monitor VGA. Vou adicionar a lógica para criar a peça, desenhá-la na tela e também incluir um loop para manter a tela atualizada. Aqui está o código completo:

```c*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intelfpgaup/video.h>
#include <time.h>
#include <unistd.h>


#define BLOCO_TAM 10 // Tamanho de cada quadrado
#define LARGURA_TELA 319 // Tamanho da tela VGA
#define ALTURA_TELA 239 // Tamanho da tela VGA



typedef struct {
    int pos_x, pos_y; // Posição relativa do quadrado dentro da peça
    short cor;        // Cor do quadrado
    bool ativo;       // Indica se o quadrado faz parte da peça (ativo ou não)
} Quadrado;

typedef struct {
    int tam_x, tam_y;     // Dimensões da peça (número de quadrados)
    int pos_x, pos_y;     // Posição da peça no tabuleiro
    Quadrado quadrados[4][4]; // Matriz de quadrados, 4x4 é suficiente para qualquer peça de Tetris
} Peca;

typedef struct {
    int largura, altura; // Dimensões do tabuleiro
    short cor_fundo;     // Cor de fundo do tabuleiro
    short matriz[319][239]; // Matriz para armazenar as peças no tabuleiro (20x10)
} Tabuleiro;

Peca criarPecaL(short cor){
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
    //pecaL.quadrados[2][1] = (Quadrado){2, 1, cor, true}; // Quadrado à direita do terceiro

    return pecaL;
}

Peca criarPecaQ(short cor){
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
    pecaL.quadrados[1][1] = (Quadrado){1, 1, cor, true};
    pecaL.quadrados[0][1] = (Quadrado){0, 1, cor, true};



    return pecaL;
}

Tabuleiro criarTabuleiro() {
    Tabuleiro tab;
    tab.largura = 319;
    tab.altura = 239;
    tab.cor_fundo =  video_WHITE;    

    // Inicializa a matriz do tabuleiro com zeros (sem peças)
    for (int i = 0; i < tab.altura; i++) {
        for (int j = 0; j < tab.largura; j++) {
            tab.matriz[i][j] = tab.cor_fundo;
        }
    }

    return tab;
}

void desenhaTabuleiro(Tabuleiro tab) {
    for (int i = 0; i < tab.altura; i++) {
        for (int j = 0; j < tab.largura; j++) {
            //if (tab.matriz[i][j] != tab.cor_fundo) {
                video_box(j * BLOCO_TAM, i * BLOCO_TAM, (j + 1) * BLOCO_TAM - 1, (i + 1) * BLOCO_TAM - 1, tab.matriz[i][j]);
            //}
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

short corAleatoria(){
    short cores[]= {video_WHITE, video_YELLOW, video_RED, video_GREEN, video_BLUE, video_CYAN, video_MAGENTA, video_GREY, video_PINK, video_ORANGE};

    // Semente para o gerador de números aleatórios, usando o tempo atual
    srand(time(NULL));

    // Gerar um número aleatório entre 0 e 9
    int numAleatorio = rand() % 10;

    return cores[numAleatorio];
}

void moverPeca(Peca *peca, Peca *pecaq,int dx, int dy) {
    if(peca->pos_y < 200) {
        peca->pos_x += dx;
        peca->pos_y += dy;
        

    } 
    if (pecaq->pos_y  < 200){
        pecaq -> pos_x += dx;
        pecaq -> pos_y += dy + 1;
    }
        else if (/*pecaq -> pos_y +2 == peca ->pos_y && */pecaq -> pos_y == 185){
        peca->pos_x = peca->pos_x;
        peca->pos_y = peca -> pos_y;
    }

}

void fixarPeca(Tabuleiro *tab, Peca peca) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (peca.quadrados[i][j].ativo) {
                // Calcula as coordenadas absolutas no tabuleiro
                int x = peca.pos_x + j;
                int y = peca.pos_y + i;

                // Verifica se a posição está dentro dos limites do tabuleiro
                //if (x >= 0 && x < tab->largura && y >= 0 && y < tab->altura) {
                    // Fixa a cor do quadrado na matriz do tabuleiro
                   // tab->matriz[y][x] = peca.quadrados[i][j].cor;
               // }
            }
        }
    }
}


int main(){
    video_open();    // Inicializa o vídeo
    video_clear();   // Limpa o vídeo

    Tabuleiro tabu;
    desenhaTabuleiro(tabu);

    // Cria a peça "L" com uma cor aleatória
    short cor = corAleatoria();
    Peca pecaL = criarPecaL(cor);
    Peca pecaq = criarPecaQ(corAleatoria());

    // Desenha a peça "L" na tela
    desenhaPeca(pecaL);
    desenhaPeca(pecaq);

    fixarPeca(&tabu, pecaL);

    video_show();    // Mostra o conteúdo na tela

    // Mantém a tela atualizada até que o usuário feche
    while (1) {

        

        moverPeca(&pecaL, &pecaq,0 , 1);
        //moverPeca(&pecaq, 0 , 2);
        video_clear();



        //Tabuleiro tabu = criarTabuleiro();
        //desenhaTabuleiro(tabu);
        
        desenhaPeca(pecaL);
        desenhaPeca(pecaq);

        video_show();    
        //sleep(1);
        // Aqui você pode adicionar lógica para movimentar a peça ou outras interações
    }

    video_close();   // Fecha o vídeo

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
