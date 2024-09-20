#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intelfpgaup/video.h>
#include <time.h>
#include <unistd.h>
#include <Accel.h>

#define BLOCO_TAM 10
#define LARGURA_TELA 300
#define ALTURA_TELA 200

// Tamanho para a matriz
#define COLUNA_TABULEIRO 10
#define LINHA_TABULEIRO 20
int acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg;

// y até o 200 e x até o 300
// y = 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 180, 190, 200
// x = 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100,

typedef struct
{
    bool ocupado[LINHA_TABULEIRO][COLUNA_TABULEIRO]; // Matriz para armazenar o estado das posições
    short cor[LINHA_TABULEIRO][COLUNA_TABULEIRO];
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
    pecaL.pos_x = (60) - (BLOCO_TAM * 1); // Centraliza a peça
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

short corAleatoria()
{
    short cores[] = {video_YELLOW, video_RED, video_GREEN, video_BLUE, video_CYAN, video_MAGENTA, video_GREY, video_PINK, video_ORANGE};

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
    peca.pos_x = (60) - (BLOCO_TAM * 1); // Centraliza a peça
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
                if (x < 0 || x+1 >= LINHA_TABULEIRO || y < 0 || y >= COLUNA_TABULEIRO)
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
    for (int i = 0; i < LINHA_TABULEIRO; i++)   {
        for (int j = 0; j < COLUNA_TABULEIRO; j++)
        {
            // faz com que marque como true no tabuleiro
            if (i < 4 && j < 4 && peca.quadrados[i][j].ativo)
            {
                int x = (peca.pos_y / BLOCO_TAM) + i;
                int y = (peca.pos_x / BLOCO_TAM) + j;

                tabuleiro->ocupado[x][y] = true;
                tabuleiro->cor[x][y] = peca.quadrados[i][j].cor;

                // mostra a peça na tela
                x = peca.pos_x + (j * BLOCO_TAM);
                y = peca.pos_y + (i * BLOCO_TAM);
                video_box(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, peca.quadrados[i][j].cor);
            }

            // faz com que mostre na tela as posições que já estão ativas no tabuleiro
            // else if (tabuleiro->ocupado[i][j] == true)
            // {
            //     int x = (j * BLOCO_TAM);
            //     int y = (i * BLOCO_TAM);
            //     video_box(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, video_YELLOW);
            // }
        }
    }
}

void mostrarAllPecas(Tabuleiro *tab){
    for (int i = 0; i < LINHA_TABULEIRO; i++)
    {
        for (int j = 0; j < COLUNA_TABULEIRO; j++)
        {
            // faz com que mostre na tela as posições que já estão ativas no tabuleiro
            if (tab->ocupado[i][j] == true)
            {
                int x = (j * BLOCO_TAM);
                int y = (i * BLOCO_TAM);
                video_box(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, tab->cor[i][j]);
            }
        }

        video_box(0, 200, 110, 210, video_WHITE);//desenha o limite do linha
        video_box(100, 0, 110, 210, video_WHITE);//desenha o limite da coluna

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
    int fd;
    void *i2c_base;
    int16_t accel_data[3] = {0};


    // Abrir /dev/mem para acessar a memória do sistema
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Erro ao abrir /dev/mem");
        return -1;
    }

     // Inicializar o acelerômetro
    if (!inicializar_acelerometro(fd, &i2c_base)) {
        close(fd);
        return -1;
    }

    
    video_open();
    video_clear();

    Tabuleiro tab;
    iniTabuleiro(&tab);

    Peca peca = criarPecasAleatorias();

    video_show();

    while (1)
    {
        while (!verificarColisao(&tab, peca))
        {
            ler_dados_acelerometro(&i2c_base, *accel_data);

            printf("\n------------------------------------\n");
            printf("Aceleração em X: %d\n", accel_data[0]);
            printf("Aceleração em Y: %d\n", accel_data[1]);
            printf("Aceleração em Z: %d\n", accel_data[2]);
            printf("\n------------------------------------\n");
            
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
            mostrarAllPecas(&tab);

            video_show();
            usleep(500000);
        }

        addPecaNoTabuleiro(&tab, peca);
        verificaLinhaCompleta(&tab);

        peca = criarPecasAleatorias();
    }

    video_close();
    munmap(i2c_base, MAP_SIZE);
    close(fd);

    return 0;
}