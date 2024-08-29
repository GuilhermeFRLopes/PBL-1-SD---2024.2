#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <intelfpgaup/video.h>
#include <time.h>

#define video_BLACK 0x00
#define LARGURA_TELA 319 // Tamanho da tela VGA
#define ALTURA_TELA 239 // Tamanho da tela VGA

typedef struct {
  int tam_x, tam_y; // Tamanho do bloco (o quanto se deve incrementar o espaço
                    // na video_box pra criar o bloco)
  int pos_x, pos_y; // Posição em eixo X e Y em que será criado o bloco
  short cor;        // Cor do bloco
  bool quebrado;    // Se o bloco está já quebrado ou não
} Bloco;

typedef struct eli_1{
    int tam_x;
    int tam_y;
    int pos_x;
    int pos_y;
    short cor;
    int forma[3][2];
} eli_1;

typedef struct {
    int tam_x, tam_y; // Tamanho da barra (largura e altura)
    int pos_x, pos_y; // Posição da barra na tela
    short cor;        // Cor da barra
} Barra;

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