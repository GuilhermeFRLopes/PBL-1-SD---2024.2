void verificaLinhaCompleta(Tabuliero *tab) {
    // Começa de baixo para cima
    for (int i = LINHA_TABULEIRO - 1; i >= 0; i--) {
        bool linhaCompleta = true;

        // Verifica se a linha está completa
        for (int j = 0; j < COLUNA_TABULEIRO; j++) {
            if (!tab->ocupado[i][j]) {  // Se encontrar um espaço vazio
                linhaCompleta = false;
                break;
            }
        }

        // Se a linha estiver completa
        if (linhaCompleta) {
            // Aumenta a pontuação e diminui a velocidade
            score += 10;
            velocidade -= 3500;

            // Desocupa a linha completa
            for (int j = 0; j < COLUNA_TABULEIRO; j++) {
                tab->ocupado[i][j] = false;
            }

            // Mover todas as linhas acima para baixo
            for (int p = i; p > 0; p--) {
                for (int k = 0; k < COLUNA_TABULEIRO; k++) {
                    // Move a linha acima para a linha atual
                    tab->ocupado[p][k] = tab->ocupado[p-1][k];
                }
            }

            // Após mover as linhas, limpar a primeira linha (que não tem nada acima)
            for (int k = 0; k < COLUNA_TABULEIRO; k++) {
                tab->ocupado[0][k] = false;
            }

            // Como a linha atual foi preenchida e removida, devemos verificar a mesma linha novamente
            i++;
        }
    }
}