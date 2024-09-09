/*Claro! Vou completar o código para que você possa criar e desenhar a peça em forma de "L" no monitor VGA. Vou adicionar a lógica para criar a peça, desenhá-la na tela e também incluir um loop para manter a tela atualizada. Aqui está o código completo:

```c*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intelfpgaup/video.h>
#include <intelfpgaup/accel.h>
#include <time.h>
#include <unistd.h>


#define BLOCO_TAM 10 // Tamanho de cada quadrado
#define LARGURA_TELA 319 // Tamanho da tela VGA
#define ALTURA_TELA 239 // Tamanho da tela VGA

int acel_rdy, acel_tap, acel_dtap, acel_x, acel_y, acel_z, acel_mg;

typedef struct {
    int pos_x, pos_y; // Posição relativa do quadrado dentro da peça
    short cor;        // Cor do quadrado
    bool ativo;       // Indica se o quadrado faz parte da peça (ativo ou não)
} Quadrado;

typedef struct {
    int tam_x, tam_y;     // Dimensões da peça (número de quadrados)
    int pos_x, pos_y;     // Posição da peça no tabuleiro
    bool fixado;
    Quadrado quadrados[4][4]; // Matriz de quadrados, 4x4 é suficiente para qualquer peça de Tetris
} Peca;

/*
typedef struct {
    int largura, altura; // Dimensões do tabuleiro
    short cor_fundo;     // Cor de fundo do tabuleiro
    Quadrado matriz[LARGURA_TELA][ALTURA_TELA]; // Matriz para armazenar as peças no tabuleiro
} Tabuleiro;*/

typedef struct {
    int largura, altura; // Dimensões do tabuleiro
    short cor_fundo;     // Cor de fundo do tabuleiro
    Quadrado matriz[20][20]; // Matriz para armazenar as peças no tabuleiro
} Tabuleiro;

Peca criarPecaL(short cor){
    Peca pecaL;
    pecaL.pos_x = (115) - (BLOCO_TAM * 1); // Centraliza a peça
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

Peca criarPecaQ(short cor){
    Peca pecaL;
    // LARGURA_TELA / 2 = 159.5
    // BLOCO_TAM * 1 = 10;
    pecaL.pos_x = (LARGURA_TELA / 2) - (BLOCO_TAM * 1); // Centraliza a peça. Essa conta = 149.5
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
    tab.largura = 20;
    tab.altura = 20;
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
                //posiçao inicial x = 149.5 e y = 0
                //entao int x = peca.pos_x + j * BLOCO_TAM; é igual a 149.5 + 0 * 10
                //int y = peca.pos_y + i * BLOCO_TAM; é igual a 0 + 0 * 10
                int x = peca.pos_x + j * BLOCO_TAM;
                int y = peca.pos_y + i * BLOCO_TAM;
                //void video_box (int /*x1*/, int /*y1*/, int /*x2*/, int /*y2*/, short /*color*/);
                /*
                x1, y1: coordenadas de um canto do retângulo.
                x2, y2: coordenadas do canto oposto.
                color: cor do retângulo.
                */
                video_box(x, y, x + BLOCO_TAM - 1, y + BLOCO_TAM - 1, peca.quadrados[i][j].cor);
            }
        }
    }
}


//k = 1 149 - 150 - 151 - 152 - 153 - 154 - 155 - 156 - 157 - 158
                                                                           // 0  - 1    - 2  -  3  -  4  -  5  -  6  -  7  -  8  -  9

short corAleatoria(){
    short cores[]= {video_WHITE, video_YELLOW, video_RED, video_GREEN, video_BLUE, video_CYAN, video_MAGENTA, video_GREY, video_PINK, video_ORANGE};

    // Semente para o gerador de números aleatórios, usando o tempo atual
    srand(time(NULL));

    // Gerar um número aleatório entre 0 e 9
    int numAleatorio = rand() % 10;

    return cores[numAleatorio];
}


void fixarPeca(Tabuleiro *tab, Peca peca) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (peca.quadrados[i][j].ativo) {
                // Calcula as coordenadas absolutas no tabuleiro
                int x = peca.pos_x + j;
                int y = peca.pos_y + i;

                // Verifica se as coordenadas estão dentro dos limites do tabuleiro
                if (x >= 0 && x < tab->largura && y >= 0 && y < tab->altura) {
                    tab->matriz[y][x] = peca.quadrados[i][j];
                }
            }
        }
    }
}

// Função para verificar e limpar linhas completas
void verificarLinhasCompletas(Tabuleiro *tab) {
    for (int y = 0; y < ALTURA_TABULEIRO; y++) {
        bool linhaCompleta = true;

        // Verifica se a linha está completa
        for (int x = 0; x < LARGURA_TABULEIRO; x++) {
            if (!tab->matriz[y][x].ativo) {
                linhaCompleta = false;
                break;
            }
        }

        // Se a linha estiver completa, limpá-la e descer as linhas acima
        if (linhaCompleta) {
            // Limpa a linha completa
            for (int x = 0; x < LARGURA_TABULEIRO; x++) {
                tab->matriz[y][x].ativo = false;
            }

            // Desce as linhas acima
            for (int linhaAcima = y; linhaAcima > 0; linhaAcima--) {
                for (int x = 0; x < LARGURA_TABULEIRO; x++) {
                    tab->matriz[linhaAcima][x] = tab->matriz[linhaAcima - 1][x];
                }
            }

            // Limpa a linha superior (agora vazia)
            for (int x = 0; x < LARGURA_TABULEIRO; x++) {
                tab->matriz[0][x].ativo = false;
            }

            // Reavalia a linha atual, pois pode haver mais linhas completas após a descida
            y--; // Decrementa y para verificar a linha atual novamente
        }
    }
}

/*void moverPeca(Peca *peca, Tabuleiro *tab,int dx, int dy) {
    
    if (tab -> matriz[dx + 1][dy + 1].ativo == true){
        fixarPeca(tab, *peca);
        peca.fixado = true;
    } else {
        //peca->pos_x += dx;
        //peca->pos_y += dy;

        if(peca->pos_y < 170) {
            peca->pos_x += dx;
            peca->pos_y += dy;
        } else {
            fixarPeca(tab, *peca);
            peca.fixado = true;
        }
    }
}*/

/*
void moverPeca(Peca *peca, Tabuleiro *tab, int dx, int dy) {
    // Calcula a nova posição
    int nova_pos_x = peca->pos_x + dx;
    int nova_pos_y = peca->pos_y + dy;

    // Verifica e ajusta a nova posição X
    if (nova_pos_x < 0) {
        nova_pos_x = 0; // Limite inferior
    } else if (nova_pos_x > 199) {
        nova_pos_x = 199; // Limite superior
    }

    // Verifica e ajusta a nova posição Y
    if (nova_pos_y < 0) {
        nova_pos_y = 0; // Limite inferior
    } else if (nova_pos_y > 170) {
        nova_pos_y = 170; // Limite superior
    }

    // Verifica se a posição onde a peça vai se mover já está ocupada
    if (tab->matriz[nova_pos_x + 1][nova_pos_y + 1].ativo == true) {
        fixarPeca(tab, *peca);
        peca->fixado = true;
    } else {
        // Move a peça para a nova posição ajustada
        peca->pos_x = nova_pos_x;
        peca->pos_y = nova_pos_y;

        // Verifica se a nova posição Y está dentro do limite
        if (peca->pos_y >= 170) {
            fixarPeca(tab, *peca);
            peca->fixado = true;
        }
    }
}*/

void moverPeca(Peca *peca, Tabuleiro *tab, int dx, int dy) {
    // Calcula a nova posição
    int nova_pos_x = peca->pos_x + dx;
    int nova_pos_y = peca->pos_y + dy;

    // Verifica e ajusta a nova posição X
    if (nova_pos_x < 0) {
        nova_pos_x = 0; // Limite inferior
    } else if (nova_pos_x > 199) {
        nova_pos_x = 199; // Limite superior
    }

    // Verifica se a nova posição Y está dentro do limite
    if (nova_pos_y < 0) {
        nova_pos_y = 0; // Limite inferior
    } else if (nova_pos_y > 170) {
        nova_pos_y = 170; // Limite superior
    }

    // Verifica se a nova posição está ocupada
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (peca->quadrados[i][j].ativo) {
                int x = nova_pos_x + j;
                int y = nova_pos_y + i;

                // Verifica se a nova posição está dentro dos limites do tabuleiro
                if (x < 0 || x >= tab->largura || y < 0 || y >= tab->altura || tab->matriz[y][x].ativo) {
                    // Colisão detectada
                    fixarPeca(tab, *peca);
                    peca->fixado = true;
                    return; // Sai da função se houver colisão
                }
            }
        }
    }

    // Move a peça para a nova posição ajustada
    peca->pos_x = nova_pos_x;
    peca->pos_y = nova_pos_y;
}

 /*if(peca->pos_y < 170) {
        peca->pos_x += dx;
        peca->pos_y += dy;
    } */

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


int main(){
    video_open();    // Inicializa o vídeo
    video_clear();   // Limpa o vídeo

    Tabuleiro tab = criarTabuleiro();
    desenhaTabuleiro(tab);

    Peca pecaAtual = criarPecaL(corAleatoria());
    desenhaPeca(pecaAtual);

    video_show();     // Mostra o conteúdo na tela
    
    // Mantém a tela atualizada até que o usuário feche
    while (1) {
        accel_read(&acel_rdy, &acel_tap, &acel_dtap, &acel_x, &acel_y, &acel_z, &acel_mg);
        if (acel_x < -10)
            {
                moverPeca(&pecaAtual, &tab,-1 , 1);
            } // move para a esquerda
            else if (acel_x > 10)
            {
                moverPeca(&pecaAtual, &tab,1 , 1);
            } else {
                moverPeca(&pecaAtual, &tab,0 , 1); // Move a peça para baixo
            }
        //moverPeca(&pecaAtual, &tab,0 , 1); // Move a peça para baixo

        video_clear();  // Limpa a tela

        desenhaTabuleiro(tab); // Redesenha o tabuleiro
        desenhaPeca(pecaAtual); // Desenha a peça na nova posição

        video_show();  // Atualiza a tela
        usleep(500000); // Delay para controlar a velocidade de movimento
        if (pecaAtual.fixado == true){            
            pecaAtual = criarPecaL(corAleatoria());
        }
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
