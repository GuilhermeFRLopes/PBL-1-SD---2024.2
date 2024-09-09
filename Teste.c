#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intelfpgaup/video.h>
#include <intelfpgaup/accel.h>
#include <time.h>
#include <unistd.h>

#define BLOCO_TAM 10
#define LARGURA_TELA 300
#define ALTURA_TELA 200

// Tamanho para a matriz
#define COLUNA_TABULEIRO 30
#define LINHA_TABULEIRO 20
int acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg;

// y até o 200 e x até o 300
// y = 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 180, 190, 200
// x = 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300

typedef struct
{
    bool ocupado[LINHA_TABULEIRO][COLUNA_TABULEIRO]; // Matriz para armazenar o estado das posições
} Tabuleiro;

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

Peca criarPeca(short cor)
{
    Peca pecaL;
    pecaL.pos_x = (LARGURA_TELA / 2) - (BLOCO_TAM * 1); // Centraliza a peça
    pecaL.pos_y = 0;                                    // Começa no topo da tela
    pecaL.tam_x = 3;
    pecaL.tam_y = 2;

    // Inicializa a matriz de quadrados para a peça
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            pecaL.quadrados[i][j].ativo = false; // Inicializa todos os quadrados como inativos
        }
    }

    // Definindo os quadrados ativos para a peça
    pecaL.quadrados[0][0] = (Quadrado){0, 0, cor, true}; // Quadrado superior esquerdo
    pecaL.quadrados[1][0] = (Quadrado){1, 0, cor, true}; // Quadrado abaixo do primeiro
    pecaL.quadrados[2][0] = (Quadrado){2, 0, cor, true}; // Quadrado abaixo do segundo
    // pecaL.quadrados[2][1] = (Quadrado){2, 1, cor, true}; // Quadrado à direita do terceiro

    return pecaL;
}

void desenhaPeca(Peca peca)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (peca.quadrados[i][j].ativo)
            {
                int x = peca.pos_x + j * BLOCO_TAM;
                int y = peca.pos_y + i * BLOCO_TAM;
                video_box(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, peca.quadrados[i][j].cor); // x = pixel de inicio da peça; x + bloco tam = pixel de fim da peça
            }
        }
    }
}

short corAleatoria()
{
    short cores[] = {video_WHITE, video_YELLOW, video_RED, video_GREEN, video_BLUE, video_CYAN, video_MAGENTA, video_GREY, video_PINK, video_ORANGE};

    // Semente para o gerador de números aleatórios, usando o tempo atual
    srand(time(NULL));

    // Gerar um número aleatório entre 0 e 9
    int numAleatorio = rand() % 10;

    return cores[numAleatorio];
}

void moverPeca(Peca *peca, int dy)
{
    //printf("\npos: %d\n", peca->pos_y);
    peca->pos_y += dy;
}

void iniTabuleiro(Tabuleiro *tabuleiro)
{
    for (int i = 0; i < LINHA_TABULEIRO; i++)
    {
        for (int j = 0; j < COLUNA_TABULEIRO; j++)
        {
            tabuleiro->ocupado[i][j] = false;
        }
    }
}

void moverDirOuEsq(Tabuleiro *tab, Peca *peca, int dx)
{
    bool podeMover = true;

    for (int i = 0; i < 4; i++) // não precisa percorrer o tabuleiro todo, pois está passando a posição da peça na matriz
    {
        for (int j = 0; j < 4; j++)
        {
            if (peca->quadrados[i][j].ativo)
            {
                int x = (peca->pos_y / BLOCO_TAM) + i;
                int y = (peca->pos_x / BLOCO_TAM) + j;

                // para saber se a posição da peça será para a esquerda ou direita
                if (dx > 0)
                {
                    // verifica se tem uma peça a direita
                    if (y + 1 >= COLUNA_TABULEIRO || tab->ocupado[x][y + 1] == true)
                    {
                        podeMover = false;
                    }
                }

                else
                {
                    // verifica se tem uma peça a esquerda
                    if (y - 1 < 0 || tab->ocupado[x][y - 1] == true)
                    {
                        podeMover = false;
                    }
                }
            }
        }
    }

    if (podeMover)
    {
        peca->pos_x += dx;
    }
}

bool verificarColisao(Tabuleiro *tabuleiro, Peca peca)
{
    for (int i = 0; i < 4; i++) // não precisa percorrer o tabuleiro todo, pois está passando a posição da peça na matriz
    {
        for (int j = 0; j < 4; j++)
        {
            if (peca.quadrados[i][j].ativo)
            {
                int x = (peca.pos_y / BLOCO_TAM) + i;
                int y = (peca.pos_x / BLOCO_TAM) + j;

                // verifica se a peça está dentro do tabuleiro
                if (x < 0 || x >= LINHA_TABULEIRO || y < 0 || y >= COLUNA_TABULEIRO)
                {
                    return true;
                }

                // verifica se tem uma peça abaixo
                if (x + 1 < LINHA_TABULEIRO && tabuleiro->ocupado[x + 1][y] == true)
                {
                    return true;
                }

                // // verifica se tem uma peça a esquerda
                // if (y - 1 >= 0 && tabuleiro->ocupado[x][y - 1] == true)
                // {
                //     return true;
                // }

                // // verifica se tem uma peça a direita
                // if (y + 1 < COLUNA_TABULEIRO && tabuleiro->ocupado[x][y + 1] == true)
                // {
                //     return true;
                // }
            }
        }
    }
    return false;
}

void addPecaNoTabuleiro(Tabuleiro *tabuleiro, Peca peca)
{
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 31; j++)
        {
            // faz com que marque como true no tabuleiro
            if (i < 4 && j < 4 && peca.quadrados[i][j].ativo)
            {
                int x = (peca.pos_y / BLOCO_TAM) + i;
                int y = (peca.pos_x / BLOCO_TAM) + j;

                tabuleiro->ocupado[x][y] = true;

                // mostra a peça na tela
                x = peca.pos_x + (j * BLOCO_TAM);
                y = peca.pos_y + (i * BLOCO_TAM);
                video_box(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, peca.quadrados[i][j].cor);
            }

            // faz com que mostre na tela as posições que já estão ativas no tabuleiro
            else if (tabuleiro->ocupado[i][j] == true)
            {
                int x = (j * BLOCO_TAM);
                int y = (i * BLOCO_TAM);
                video_box(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, video_WHITE);
            }
        }
    }
}

void verificaLinhaCompleta(Tabuleiro *tab)
{
    int linhaCompleta, i, j, p, k;

    for (i = 0; i < LINHA_TABULEIRO; i++)
    {
        linhaCompleta = 0;

        for (j = 0; j < COLUNA_TABULEIRO; j++)
        {
            if (tab->ocupado[i][j] == true)
            {
                linhaCompleta += 1;
            }
        }

        if (linhaCompleta == COLUNA_TABULEIRO)
        {
            for (j = 0; j < COLUNA_TABULEIRO; j++)
            {
                tab->ocupado[i][j] = false; // tira os blocos da linha completa
            }

            for (p = i - 1; p >= 0; p--) // move de cima para baixo
            {
                for (k = 0; k < COLUNA_TABULEIRO; k++)
                {
                    if (tab->ocupado[p][k] == true)
                    {
                        tab->ocupado[p + 1][k] = tab->ocupado[p][k]; // faz com que desça o bloco para a linha que foi desocupada
                        tab->ocupado[p][k] = false; 
                    }
                }
            }
        }
    }
}

int main()
{
    video_open();
    video_clear();

    Tabuleiro tab;
    iniTabuleiro(&tab);

    short cor = corAleatoria();
    Peca peca = criarPeca(cor);

    video_show();

    while (1)
    {
        while (!verificarColisao(&tab, peca))
        {
            accel_read(&acel_rdy, &acel_tap, &acel_dtap, &acel_x, &acel_y, &acel_z, &acel_mg);

            desenhaPeca(peca);

            moverPeca(&peca, 10); // move para baixo
            if (acel_x < -10)
            {
                moverDirOuEsq(&tab, &peca, -10);
            } // move para a esquerda
            else if (acel_x > 10)
            {
                moverDirOuEsq(&tab, &peca, 10);
            } // move para a direita

            video_clear();

            desenhaPeca(peca);

            video_show();
            sleep(1);
        }

        addPecaNoTabuleiro(&tab, peca);
        verificaLinhaCompleta(&tab);

        peca = criarPeca(corAleatoria());
    }

    video_close();
    return 0;
}