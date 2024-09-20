const char* asciiText[] = {
    "▗▄▄▖  ▗▄▖ ▗▖ ▗▖ ▗▄▄▖▗▄▄▄▖",
    "▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌   ",
    "▐▛▀▘ ▐▛▀▜▌▐▌ ▐▌ ▝▀▚▖▐▛▀▀▘",
    "▐▌   ▐▌ ▐▌▝▚▄▞▘▗▄▄▞▘▐▙▄▄▖"
};

void draw_ascii_text(int start_x, int start_y) {
    char single_char[2]; // Array para armazenar um único caractere e o terminador nulo
    single_char[1] = '\0'; // Define o terminador nulo

    // Desenha o texto ASCII
    for (int i = 0; i < 4; i++) { // São 4 linhas no texto fornecido
        for (int j = 0; asciiText[i][j] != '\0'; j++) { // Para cada caractere na linha
            if (asciiText[i][j] != ' ') { // Se não for um espaço
                single_char[0] = asciiText[i][j]; // Armazena o caractere
                video_text(start_x + j, start_y + i, single_char); // Desenha o caractere
            }
        }
    }
}

int main() {
    // Definindo as coordenadas de início
    int start_x = 10;
    int start_y = 5;

    // Definir o valor que será exibido ao lado da arte
    int value = 1234;

    // Chama a função para desenhar a arte ASCII
    draw_ascii_art(start_x, start_y, 0, value);

    // Chama a função para desenhar o texto fornecido
    draw_ascii_text(start_x, start_y + 15); // Desenha o texto um pouco abaixo da arte
    
    return 0;
}
