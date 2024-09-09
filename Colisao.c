#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intelfpgaup/video.h>
#include <time.h>
#include <unistd.h>

#define BLOCO_TAM 10     // Tamanho de cada quadrado
#define LARGURA_TELA 319 // Largura da tela VGA
#define ALTURA_TELA 239  // Altura da tela VGA

typedef struct
{
    int pos_x, pos_y; // Posição relativa do quadrado dentro da peça
    short cor;        // Cor do quadrado
    bool ativo;       // Indica se o quadrado faz parte da peça (ativo ou não)
} Quadrado;

typedef struct
{
    int tam_x, tam_y;         // Dimensões da peça (número de quadrados)
    int pos_x, pos_y;         // Posição da peça no tabuleiro
    Quadrado quadrados[4][4]; // Matriz de quadrados, 4x4 é suficiente para qualquer peça de Tetris
} Peca;

typedef struct
{
    int largura, altura;                                                // Dimensões do tabuleiro
    short cor_fundo;                                                    // Cor de fundo do tabuleiro
    Quadrado matriz[LARGURA_TELA / BLOCO_TAM][ALTURA_TELA / BLOCO_TAM]; // Matriz para armazenar as peças no tabuleiro
} Tabuleiro;

short corAleatoria();
Peca criarPecasAleatorias();
Tabuleiro criarTabuleiro();
void desenhaTabuleiro(Tabuleiro tabuleiro);
void desenhaPeca(Peca peca);
bool verificaColisao(Tabuleiro tabuleiro, Peca peca, int dx, int dy, int *xNoTabuleiro, int *yNoTabuleiro);
void fixarPecaNoTabuleiro(Tabuleiro *tabuleiro, Peca peca);
void moverPeca(Peca *peca, int dx, int dy);

int main()
{
    video_open();
    video_clear();

    Tabuleiro tabuleiro = criarTabuleiro();
    desenhaTabuleiro(tabuleiro);

    Peca peca = criarPecasAleatorias();
    desenhaPeca(peca);

    video_show();    // Mostra o conteúdo na tela
    int colisaoEmX = 0; // inteiro que guarda a posicao em que o bloco colidiu no eixo x
    int colisaoEmY = 0; // inteiro que guarda a posicao em que o bloco colidiu no eixo y
    // Loop para movimentação da peça
    while (1) {
        if (!verificaColisao(tabuleiro, peca, 0, 1, &colisaoEmX, &colisaoEmY)) {
            moverPeca(&peca, 0, BLOCO_TAM); // Move a peça para baixo
        } else {
            fixarPecaNoTabuleiro(&tabuleiro, peca, colisaoEmX, colisaoEmY); // Fixa a peça no tabuleiro
            peca = criarPecasAleatorias(); // Cria uma nova peça
        }

        video_clear();  // Limpa a tela

        desenhaTabuleiro(tabuleiro); // Redesenha o tabuleiro
        desenhaPeca(peca); // Desenha a peça na nova posição

        video_show();  // Atualiza a tela
        usleep(500000); // Delay para controlar a velocidade de movimento
    }

    video_close();  // Fecha o vídeo

    return 0;
}

short corAleatoria()
{
    short cores[] = {video_WHITE, video_YELLOW, video_RED, video_GREEN, video_BLUE, video_CYAN, video_MAGENTA, video_GREY, video_PINK, video_ORANGE};

    srand(time(NULL)); // Semente para o gerador de números aleatórios

    int numAleatorio = rand() % 9; // Gerar um número aleatório entre 0 e 9

    return cores[numAleatorio];
}

Peca criarPecasAleatorias()
{
    // Define as cores disponíveis para as peças
    short cor = corAleatoria();

    // Gera um número aleatório para escolher a peça
    int tipoPeca = rand() % 7;

    Peca peca;
    peca.pos_x = (LARGURA_TELA / 2) - (BLOCO_TAM * 1); // Centraliza a peça
    peca.pos_y = 0;                                    // Começa no topo da tela
    peca.tam_x = 4;                                    // Dimensão máxima 4x4 para qualquer peça
    peca.tam_y = 4;                                    // TESTAR SEM

    // Inicializa a matriz de quadrados como inativa
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            peca.quadrados[i][j].ativo = false;
        }
    }

    // Define a forma da peça com base no número aleatório
    switch (tipoPeca)
    {
    case 0: // Peça "I"
        peca.quadrados[0][1].ativo = true;
        peca.quadrados[1][1].ativo = true;
        peca.quadrados[2][1].ativo = true;
        peca.quadrados[3][1].ativo = true;
        break;
    case 1: // Peça "O"
        peca.quadrados[1][1].ativo = true;
        peca.quadrados[1][2].ativo = true;
        peca.quadrados[2][1].ativo = true;
        peca.quadrados[2][2].ativo = true;
        break;
    case 2: // Peça "T"
        peca.quadrados[1][1].ativo = true;
        peca.quadrados[0][1].ativo = true;
        peca.quadrados[2][1].ativo = true;
        peca.quadrados[1][0].ativo = true;
        break;
    case 3: // Peça "L"
        peca.quadrados[0][1].ativo = true;
        peca.quadrados[1][1].ativo = true;
        peca.quadrados[2][1].ativo = true;
        peca.quadrados[2][2].ativo = true;
        break;
    case 4: // Peça "J"
        peca.quadrados[0][1].ativo = true;
        peca.quadrados[1][1].ativo = true;
        peca.quadrados[2][1].ativo = true;
        peca.quadrados[2][0].ativo = true;
        break;
    case 5: // Peça "Z"
        peca.quadrados[0][1].ativo = true;
        peca.quadrados[1][1].ativo = true;
        peca.quadrados[1][2].ativo = true;
        peca.quadrados[2][2].ativo = true;
        break;
    case 6: // Peça "S"
        peca.quadrados[0][2].ativo = true;
        peca.quadrados[1][1].ativo = true;
        peca.quadrados[1][2].ativo = true;
        peca.quadrados[2][1].ativo = true;
        break;
    }

    // Define a cor para os quadrados ativos
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (peca.quadrados[i][j].ativo)
            {
                peca.quadrados[i][j].cor = cor;
            }
        }
    }

    return peca;
}

Tabuleiro criarTabuleiro(){
    Tabuleiro tab
    tab.largura = LARGURA_TELA / BLOCO_TAM;
    tab.altura = ALTURA_TELA / BLOCO_TAM;
    tab.cor_fundo = video_GREY;    

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

bool verificaColisao(Tabuleiro tabuleiro, Peca peca, int dx, int dy, int *xNoTabuleiro, int *yNoTabuleiro)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (peca.quadrados[i][j].ativo)
            {
                int novo_x = (peca.pos_x + j * BLOCO_TAM + dx) / BLOCO_TAM;
                int novo_y = (peca.pos_y + i * BLOCO_TAM + dy) / BLOCO_TAM;

                // Verifica se a peça está fora dos limites do tabuleiro ou colidiu com outra peça
                if (novo_x < 0 || novo_x >= tabuleiro.largura / BLOCO_TAM || novo_y >= tabuleiro.altura / BLOCO_TAM || (novo_y >= 0 && tabuleiro.matriz[novo_x][novo_y].ativo))
                {
                    *xNoTabuleiro = novo_x;
                    *yNoTabuleiro = novo_y;
                    return true;
                }
            }
        }
    }
    return false;
}

void fixarPecaNoTabuleiro(Tabuleiro *tab, Peca peca, int posX, int posY) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (peca.quadrados[i][j].ativo) {
                /* int x = (peca.pos_x + j * BLOCO_TAM) / BLOCO_TAM;
                int y = (peca.pos_y + i * BLOCO_TAM) / BLOCO_TAM; */
                int x = posX + j;
                int y = posY + i;
                tab->matriz[y][x] = peca.quadrados[i][j]; // Fixa a peça no tabuleiro
            }
        }
    }
}

void moverPeca(Peca *peca, int dx, int dy) {
    peca->pos_x += dx;
    peca->pos_y += dy;
}