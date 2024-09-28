#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <intelfpgaup/video.h>
#include <intelfpgaup/KEY.h>
#include <time.h>

#define I2C0_BASE_ADDR 0xFFC04000 // Endereço base do I2C0
#define IC_CON_OFFSET 0x0         // Deslocamento do registrador ic_con
#define MAP_SIZE 0x1000           // Tamanho do mapeamento de memória

#define IC_TAR_REG 0x04                             // Offset do registrador IC_TAR
#define IC_CON_REG (I2C0_BASE_ADDR + IC_CON_OFFSET) // Correto
#define IC_DATA_CMD_REG 0x10                        // Offset do registrador IC_DATA_CMD
#define IC_ENABLE_REG 0x6C                          // Offset do registrador IC_ENABLE
#define IC_RXFLR_REG 0x78                           // Offset do registrador IC_RXFLR

// Endereço I2C do ADXL345
#define ADXL345_ADDR 0x53

// Registradores internos do ADXL345
#define ADXL345_REG_DATA_X0 0x32 // Registrador inicial dos dados X, Y, Z (6 bytes)

// Logica do game
#define BLOCO_TAM 10
#define LARGURA_TELA 300
#define ALTURA_TELA 200

// Tamanho para a matriz
#define COLUNA_TABULEIRO 10
#define LINHA_TABULEIRO 20

int score = 0; // Pontuação do jogador

int velocidade = 0; //velocidade da queda das peças
char str[15]; // String para exibição da pontuação
char str1[15]; // String para exibição da pontuação
char str2[15]; // String para exibição da pontuação
char str3[15]; // String para exibição da pontuação



//char jogadores[3] = {"Jogador 1", "Jogador 2", "Jogador 3"};
int pontos[3] = {0,0,0};

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
    peca.pos_y = 0;                      // Começa no topo da tela
    peca.tam_x = 4;                      // Dimensão máxima 4x4 para qualquer peça
    peca.tam_y = 4;                      // TESTAR SEM

    // Inicializa a matriz de quadrados como inativa
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            peca.quadrados[i][j].ativo = false;
        }
    }

    //tipoPeca = 9;

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
        peca.quadrados[0][1].ativo = true;
        peca.quadrados[1][1].ativo = true;
        peca.quadrados[2][1].ativo = true;
        peca.quadrados[3][1].ativo = true;
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
                video_box(x, y, x + BLOCO_TAM - 2, y + BLOCO_TAM - 2, peca.quadrados[i][j].cor); // x = pixel de inicio da peça; x + bloco tam = pixel de fim da peça
            }
        }
    }
}

void moverPeca(Peca *peca, int dy)
{
    // printf("\npos: %d\n", peca->pos_y);
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
                if (x < 0 || x + 1 >= LINHA_TABULEIRO || y < 0 || y >= COLUNA_TABULEIRO)
                {
                    return true;
                }

                // verifica se tem uma peça abaixo
                if (x + 1 < LINHA_TABULEIRO && tabuleiro->ocupado[x + 1][y] == true)
                {
                    return true;
                }
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
            }
        }
    }
}

void mostrarAllPecas(Tabuleiro *tab)
{
    for (int i = 0; i < LINHA_TABULEIRO; i++)
    {
        for (int j = 0; j < COLUNA_TABULEIRO; j++)
        {
            // faz com que mostre na tela as posições que já estão ativas no tabuleiro
            if (tab->ocupado[i][j] == true)
            {
                int x = (j * BLOCO_TAM);
                int y = (i * BLOCO_TAM);
                video_box(x, y, x + BLOCO_TAM - 2, y + BLOCO_TAM - 2, tab->cor[i][j]);
            }
        }

        video_box(0, 200, 101, 210, video_GREEN); // desenha o limite do linha - baixo
        video_box(100, 0, 101, 210, video_GREEN); // desenha o limite da coluna - direita
        video_box(0, 0, 101, 10, video_GREEN); // desenha o limite da coluna - cima
        video_box(0, 0, 1, 210, video_GREEN); // desenha o limite da coluna - esquerda

    }
}

void verificaLinhaCompleta(Tabuleiro *tab) {
    // começa de baixo para cima
    for (int i = LINHA_TABULEIRO - 1; i >= 0; i--) {
        bool linhaCompleta = true;

        for (int j = 0; j < COLUNA_TABULEIRO; j++) {
            if (!tab->ocupado[i][j]) {
                linhaCompleta = false;
                break;
            }
        }

        if (linhaCompleta) {
            score += 10;
            velocidade -= 3500;

            // desocupa a linha completa
            for (int j = 0; j < COLUNA_TABULEIRO; j++) {
                tab->ocupado[i][j] = false;
            }

            // mover todas as linhas acima para baixo
            for (int p = i; p > 0; p--) {
                for (int k = 0; k < COLUNA_TABULEIRO; k++) {
                    tab->ocupado[p][k] = tab->ocupado[p-1][k];
                    tab->cor[p][k] = tab->cor[p-1][k];
                }
            }

            for (int k = 0; k < COLUNA_TABULEIRO; k++) {
                tab->ocupado[0][k] = false;
            }

            i++;
        }
    }
}

// fonte é: big;
void desenhaText()
{
    char nomePause[7][100] = {
        "     _____    ",
        "    |  __ \\      ",
        "    | |__| |_ _ _   _ ___  ___ ",
        "    |  ___/ _` | | | / __|/ _ \\ ",
        "    | |  | |_| | |_| \\__ \\  __/ ",
        "    |_|   \\__,_|\\__,_|___/\\___| ",
        "                                ",
    };

    int i = 0;
    for (i; i < 7; i++)
    {
        video_text(30, 20 + i, nomePause[i]); // Desenha o caractere
    }
}

void desenhaFimDoJogo()
{
    char nomePause[13][100] = {
        "        _____                      ",
        "       / ____|                     ",
        "      | |  __  __ _ _ __ ___   ___ ",
        "      | | |_ |/ _` | '_ ` _ \\ / _ \\",
        "      | |__| | |_| | | | | | |  __/",
        "      |_____|\\__,_|_| |_| |_|\\___|",
        "                                     ",
        "          ____                 ",
        "         / __ |                ",
        "        | |  | |_   _____ _ __ ",
        "        | |  | | \\ / / _ \\ '__|",
        "        | |__| \\ V /  __/ |   ",
        "        \\____/  \\_/ \\___|_|   ",
    };

    int i = 0;
    for (i; i < 13; i++)
    {
        video_text(30, 20 + i, nomePause[i]); // Desenha o caractere
    }

   // sprintf(str, "Score: %d", score); // Atualiza a pontuação e exibe em terminal
   // video_text(50, 20, str);           // Exibe a pontuação
}

bool reiniciarGame(Tabuleiro *tabuleiro, Peca peca)
{
    for (int i = 0; i < 4; i++) // não precisa percorrer o tabuleiro todo, pois está passando a posição da peça na matriz
    {
        for (int j = 0; j < 4; j++)
        {
            if (peca.quadrados[i][j].ativo)
            {
                int x = (peca.pos_y / BLOCO_TAM) + i;
                int y = (peca.pos_x / BLOCO_TAM) + j;

                // verifica se tem uma peça abaixo logo ao exibir a nova peça
                if (x + 1 < LINHA_TABULEIRO && tabuleiro->ocupado[x + 1][y] == true)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

int main()
{
    int jogadas = 0;
    int fd;
    void *i2c_base;

    // Abrir /dev/mem para acessar a memória do sistema
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1)
    {
        perror("Erro ao abrir /dev/mem");
        return -1;
    }

    // Mapear a memória do controlador I2C0
    i2c_base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, I2C0_BASE_ADDR);
    if (i2c_base == MAP_FAILED)
    {
        perror("Erro ao mapear a memória do I2C");
        close(fd);
        return -1;
    }

    // Configurar o registrador IC_CON para:
    // - Mestre
    // - Modo rápido (400 kbit/s)
    // - Endereçamento de 7 bits
    // - Reinício habilitado
    uint32_t ic_con_value = 0x65;
    *((volatile uint32_t *)(i2c_base + IC_CON_OFFSET)) = ic_con_value; // Corrigido
    printf("Registrador IC_CON configurado com valor: 0x%X\n", ic_con_value);

    // Configurar o registrador IC_TAR para:
    // - Endereço de escravo ADXL345 (0x53)
    // - Endereçamento de 7 bits
    uint32_t ic_tar_value = 0x53; // Endereço de escravo (7 bits)
    *((uint32_t *)(i2c_base + IC_TAR_REG)) = ic_tar_value;
    printf("Registrador IC_TAR configurado com valor: 0x%X\n", ic_tar_value); // 3. Habilitar o I2C0
    *((uint32_t *)(i2c_base + IC_ENABLE_REG)) = 0x1;
    printf("I2C habilitado\n");

    video_open();
    video_clear();

    Tabuleiro tab;
    iniTabuleiro(&tab);

    Peca peca = criarPecasAleatorias();

    video_show();

    int pause;
    int valor = 1;
    KEY_open();

    while (1)
    {
        video_erase();
        while (!verificarColisao(&tab, peca))
        {
            usleep(500000 + velocidade);
            KEY_read(&pause);
            //printf("botao: %d", pause);
            if (pause != 0)
            {
                valor *= -1;
            }

            if (valor == 1)
            {
                video_erase();
                sprintf(str, "Score: %d", score); // Atualiza a pontuação e exibe em terminal
                video_text(50, 5, str);           // Exibe a pontuação

                pontos[jogadas] = score;

                sprintf(str1, "Jogador 1: %d", pontos[0]); // Atualiza a pontuação e exibe em terminal
                video_text(50, 7, str1);           // Exibe a pontuação

                sprintf(str2, "Jogador 2: %d", pontos[1]); // Atualiza a pontuação e exibe em terminal
                video_text(50, 9, str2);           // Exibe a pontuação

                sprintf(str3, "Jogador 3: %d", pontos[2]); // Atualiza a pontuação e exibe em terminal
                video_text(50, 11, str3);           // Exibe a pontuação


                // Escrever no IC_DATA_CMD para solicitar a leitura dos dados de X, Y, Z
                // Enviar o registrador de início de leitura (0x32 - registrador de dados do ADXL345)
                *((uint32_t *)(i2c_base + IC_DATA_CMD_REG)) = ADXL345_REG_DATA_X0;

                // Solicitar leitura de 6 bytes (dados de X, Y, Z)
                for (int i = 0; i < 6; i++)
                {
                    *((uint32_t *)(i2c_base + IC_DATA_CMD_REG)) = 0x100; // Cmd para leitura
                }

                // Verificar o IC_RXFLR para garantir que os dados estejam prontos para leitura
                while (*((uint32_t *)(i2c_base + IC_RXFLR_REG)) < 6);

                // Ler os dados do IC_DATA_CMD (6 bytes: 2 para X, 2 para Y, 2 para Z)
                int16_t accel_data[3] = {0}; // Array para armazenar os valores de X, Y, Z

                for (int i = 0; i < 3; i++)
                {
                    // Lê dois bytes (low byte primeiro, depois o high byte)
                    uint8_t low_byte = *((uint32_t *)(i2c_base + IC_DATA_CMD_REG)) & 0xFF;
                    uint8_t high_byte = *((uint32_t *)(i2c_base + IC_DATA_CMD_REG)) & 0xFF;
                    accel_data[i] = (int16_t)((high_byte << 8) | low_byte); // Combinar os dois bytes
                }

                // accel_read(&acel_rdy, &acel_tap, &acel_dtap, &acel_x, &acel_y, &acel_z, &acel_mg);
                printf("\n------------------------------------\n");
                printf("Aceleração em X: %d\n", accel_data[0]);
                printf("Aceleração em Y: %d\n", accel_data[1]);
                printf("Aceleração em Z: %d\n", accel_data[2]);
                printf("\n------------------------------------\n");

                desenhaPeca(peca);

                moverPeca(&peca, 10); // move para baixo
                if (accel_data[0] < -5)
                {
                    moverDirOuEsq(&tab, &peca, -10);
                } // move para a esquerda
                else if (accel_data[0] > 5)
                {
                    moverDirOuEsq(&tab, &peca, 10);
                } // move para a direita

                video_clear();

                desenhaPeca(peca);
                mostrarAllPecas(&tab);

                video_show();
            }
            else
            {
                desenhaText();
                KEY_read(&pause);
                if (pause != 0)
                {
                    valor *= -1;
                }
            }
        }

        addPecaNoTabuleiro(&tab, peca);
        verificaLinhaCompleta(&tab);

        peca = criarPecasAleatorias();

        // lógica para exibir mensagem de fim do jogo e botão para reiniciar
        if (reiniciarGame(&tab, peca))
        {

            while (1)
            {
                desenhaFimDoJogo();
                //video_erase();
                KEY_read(&pause);
                if (pause != 0)
                {   

                    pontos[jogadas] = score;                    
                    score = 0;
                    jogadas += 1;
                    if (jogadas == 3){
                        jogadas = 0;
                        pontos[0] = 0;
                        pontos[1] = 0;
                        pontos[2] = 0;
                    }
                    break;
                }
            }

            iniTabuleiro(&tab); // reinicia o tabuleiro ao apertar o botão
        }
    }

    video_close();

    // Desabilitar o I2C0 após a operação
    *((uint32_t *)(i2c_base + IC_ENABLE_REG)) = 0x0;
    printf("I2C desabilitado\n");

    // Desmapear a memória e fechar o arquivo de memória
    munmap(i2c_base, MAP_SIZE);
    close(fd);

    return 0;
}
