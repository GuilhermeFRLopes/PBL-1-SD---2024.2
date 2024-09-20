const char* asciiText[] = {
    "▗▄▄▖  ▗▄▖ ▗▖ ▗▖ ▗▄▄▖▗▄▄▄▖",
    "▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌   ",
    "▐▛▀▘ ▐▛▀▜▌▐▌ ▐▌ ▝▀▚▖▐▛▀▀▘",
    "▐▌   ▐▌ ▐▌▝▚▄▞▘▗▄▄▞▘▐▙▄▄▖"
};

void draw_ascii_text(int start_x, int start_y, video_WHITE, value) {
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
    // Loop para aumentar o valor
    for (int i = 0; i < 10; i++) { // Aumenta o valor 10 vezes
        draw_ascii_text(10, 5, video_WHITE, value); // Exibe a arte ASCII e o valor
        video_show(); // Mostra o que foi desenhado
        value++; // Aumenta o valor
        sleep(1); // Espera 1 segundo antes de atualizar
        video_clear(); // Limpa a tela para a próxima iteração
    }
    return 0;
}
