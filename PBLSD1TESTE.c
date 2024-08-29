#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intelfpgaup/video.h>
#include <time.h>

#define video_BLACK 0x00
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

Peca criarPecaL(short cor){
    Peca pecaL;
    pecaL.pos_x = (319 / 2);
    pecaL.pos_y = (319 / 2);
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

int main(){

    video_open();    // Inicializa o vídeo
    video_clear();   // Limpa o vídeo

    //.............

    video_show();    // Mostra o conteúdo na tela
    video_close();   // Fecha o vídeo


return 0;
}