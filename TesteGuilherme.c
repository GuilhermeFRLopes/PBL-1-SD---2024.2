#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <intelfpgaup/video.h>
#include <time.h>

#define I2C0_BASE_ADDR 0xFFC04000  // Endereço base do I2C0
#define IC_CON_OFFSET 0x0           // Deslocamento do registrador ic_con
#define MAP_SIZE        0x1000        // Tamanho do mapeamento de memória

#define IC_TAR_REG      0x04          // Offset do registrador IC_TAR
#define IC_CON_REG (I2C0_BASE_ADDR + IC_CON_OFFSET) // Correto
#define IC_DATA_CMD_REG  0x10          // Offset do registrador IC_DATA_CMD
#define IC_ENABLE_REG    0x6C          // Offset do registrador IC_ENABLE
#define IC_RXFLR_REG     0x78          // Offset do registrador IC_RXFLR

// Endereço I2C do ADXL345
#define ADXL345_ADDR     0x53

// Registradores internos do ADXL345
#define ADXL345_REG_DATA_X0 0x32       // Registrador inicial dos dados X, Y, Z (6 bytes) 

//Logica do game
#define BLOCO_TAM 10
#define LARGURA_TELA 300
#define ALTURA_TELA 200

// Tamanho para a matriz
#define COLUNA_TABULEIRO 10
#define LINHA_TABULEIRO 20

int score = 0; // Pontuação do jogador

int velocidade = 0;
char str[15]; // String para exibição da pontuação

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
    short cores[] = {video_YELLOW, video_RED, video_GREEN, video_BLUE, video_CYAN, video_GREY, video_ORANGE};

    srand(time(NULL)); // Semente para o gerador de números aleatórios

    int numAleatorio = rand() % 7; // Gerar um número aleatório entre 0 e 9

    return cores[numAleatorio];
}

Peca criarPecasAleatorias()
{
    // Define as cores disponíveis para as peças
    short cor = corAleatoria();

    // Gera um número aleatório para escolher a peça
    int tipoPeca = rand() % 9;

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
    case 7: // Peça "---"
        peca.quadrados[0][0].ativo = true;
        peca.quadrados[0][1].ativo = true;
        peca.quadrados[0][2].ativo = true;
        peca.quadrados[0][3].ativo = true;
        break;
    case 8: // Peça "-|-"
        peca.quadrados[1][0].ativo = true;
        peca.quadrados[1][1].ativo = true;
        peca.quadrados[1][2].ativo = true;
        peca.quadrados[0][1].ativo = true;
        break;
    case 9: // Peça "-|-"
        peca.quadrados[0][0].ativo = true;
        peca.quadrados[1][0].ativo = true;
        peca.quadrados[2][0].ativo = true;
        peca.quadrados[0][1].ativo = true;
        peca.quadrados[0][2].ativo = true;
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
                video_line(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, peca.quadrados[i][j].cor); 

            }
        }
    }
}

// short corAleatoria()
// {
//     short cores[] = {video_WHITE, video_YELLOW, video_RED, video_GREEN, video_BLUE, video_CYAN, video_MAGENTA, video_GREY, video_PINK, video_ORANGE};

//     // Semente para o gerador de números aleatórios, usando o tempo atual
//     srand(time(NULL));

//     // Gerar um número aleatório entre 0 e 9
//     int numAleatorio = rand() % 10;

//     return cores[numAleatorio];
// }

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
    for (int i = 0; i < LINHA_TABULEIRO; i++)
    {
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
                video_line(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, peca.quadrados[i][j].cor);

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
                video_line(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, tab->cor[i][j]);
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
            score += 10;
            velocidade -= 2000;
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
const char* asciiArt[] = {
    "    ssssssssss       cccccccccccccccc   ooooooooooo   rrrrr   rrrrrrrrr       eeeeeeeeeeee    ",
    "  ss::::::::::s    cc:::::::::::::::c oo:::::::::::oo r::::rrr:::::::::r    ee::::::::::::ee  ",
    "ss:::::::::::::s  c:::::::::::::::::co:::::::::::::::or:::::::::::::::::r  e::::::eeeee:::::ee",
    "s::::::ssss:::::sc:::::::cccccc:::::co:::::ooooo:::::orr::::::rrrrr::::::re::::::e     e:::::e",
    " s:::::s  ssssss c::::::c     ccccccco::::o     o::::o r:::::r     r:::::re:::::::eeeee::::::e",
    "   s::::::s      c:::::c             o::::o     o::::o r:::::r     rrrrrrre:::::::::::::::::e ",
    "      s::::::s   c:::::c             o::::o     o::::o r:::::r            e::::::eeeeeeeeeee  ",
    "ssssss   s:::::s c::::::c     ccccccco::::o     o::::o r:::::r            e:::::::e           ",
    "s:::::ssss::::::sc:::::::cccccc:::::co:::::ooooo:::::o r:::::r            e::::::::e          ",
    "s::::::::::::::s  c:::::::::::::::::co:::::::::::::::o r:::::r             e::::::::eeeeeeee  ",
    " s:::::::::::ss    cc:::::::::::::::c oo:::::::::::oo  r:::::r              ee:::::::::::::e  ",
    "  sssssssssss        cccccccccccccccc   ooooooooooo    rrrrrrr                eeeeeeeeeeeeee  "
};

void draw_ascii_art(int start_x, int start_y, short color, int value) {
    char single_char[2]; // Array para armazenar um único caractere e o terminador nulo
    single_char[1] = '\0'; // Define o terminador nulo

    // Desenha a arte ASCII
    for (int i = 0; i < 12; i++) { // Atualiza para 12 linhas
        for (int j = 0; asciiArt[i][j] != '\0'; j++) { // Para cada caractere na linha
            if (asciiArt[i][j] != ' ') { // Se não for um espaço
                single_char[0] = asciiArt[i][j]; // Armazena o caractere
                video_text(start_x + j, start_y + i, single_char); // Desenha o caractere
            }
        }
    }

    // Desenha o valor ao lado da arte ASCII
    char value_str[20];
    sprintf(value_str, " Valor: %d", value);
    video_text(start_x + 60, start_y + 5, value_str); // Ajuste a posição conforme necessário
}
int main() {
    // Código para abrir o dispositivo de vídeo
    if (!video_open()) {
        printf("Erro ao abrir o dispositivo de vídeo.\n");
        return -1;
    }

    // Limpa a tela
    video_clear();

    // Inicializa o valor
    int value = 0;

    // Loop para aumentar o valor
    for (int i = 0; i < 10; i++) { // Aumenta o valor 10 vezes
        draw_ascii_art(10, 5, video_WHITE, value); // Exibe a arte ASCII e o valor
        video_show(); // Mostra o que foi desenhado
        value++; // Aumenta o valor
        sleep(1); // Espera 1 segundo antes de atualizar
        video_clear(); // Limpa a tela para a próxima iteração
    }

    // Espera por uma tecla para sair
    printf("Pressione qualquer tecla para sair...\n");
    getchar();

    // Código para fechar o dispositivo de vídeo
    video_close();
    return 0;
}
