#include "Accel.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define I2C0_BASE_ADDR 0xFFC04000 // Endereço base do I2C0
#define IC_CON_OFFSET 0x0         // Deslocamento do registrador ic_con
#define MAP_SIZE 0x1000           // Tamanho do mapeamento de memória

#define IC_TAR_REG 0x04                             // Offset do registrador IC_TAR
#define IC_CON_REG (I2C0_BASE_ADDR + IC_CON_OFFSET) // Correto
#define IC_DATA_CMD_REG 0x10                        // Offset do registrador IC_DATA_CMD
#define IC_ENABLE_REG 0x6C                          // Offset do registrador IC_ENABLE
#define IC_RXFLR_REG 0x78                           // Offset do registrador IC_RXFLR


bool inicializa_acelerometro(inf fd, void **i2c_base){
    //Mapear a memória do controlador I2C0
    *i2c_base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, I2C0_BASE_ADDR);
    if (*i2c_base == MAP_FAILED) {
        perror("Erro ao mapear a memória do I2C");
        return false;
    }

    /*Sequência de manipulação dos registradores para o Acelerometro*/

    // 1. Configurar o IC_CON para modo mestre(master) e para o modo de enderaçamento de 7 Bits o qual o Aceleromtero suporta 
    uint32_t ic_con_value = 0x65; // Configuração do I2C

    // Registra o passo anterior no registrador IC_CON
    *((volatile uint32_t *)(*i2c_base + IC_CON_REG)) = ic_con_value;

    // 2. Configurar o IC_TAR com o endereço do ADXL345
    uint32_t ic_tar_value = ADXL345_ADDR;
    *((uint32_t *)(*i2c_base + IC_TAR_REG)) = ic_tar_value;

    // 3. Habilitar o protocolo de comunicação (I2C0) do ADXL345 
    *((uint32_t *)(*i2c_base + IC_ENABLE_REG)) = 0x1;
    
    return true;
}

void ler_dados_acelerometro(void *i2c_base, int16_t *dados){
    // 4. Escrever no IC_DATA_CMD para solicitar a leitura dos dados de X, Y, Z
        // Enviar o registrador de início de leitura (0x32 - registrador de dados do ADXL345)
        *((uint32_t *)(i2c_base + IC_DATA_CMD_REG)) = ADXL345_REG_DATA_X0;

        // 5. Solicitar leitura de 6 bytes (dados de X, Y, Z)
        for (int i = 0; i < 6; i++) {
            *((uint32_t *)(i2c_base + IC_DATA_CMD_REG)) = 0x100;  // Cmd para leitura
        }

        // 6. Verificar o IC_RXFLR para garantir que os dados estejam prontos para leitura
        while (*((uint32_t *)(i2c_base + IC_RXFLR_REG)) < 6);

        // 7. Ler os dados do IC_DATA_CMD (6 bytes: 2 para X, 2 para Y, 2 para Z)
        int16_t accel_data[3] = {0};  // Array para armazenar os valores de X, Y, Z

        for (int i = 0; i < 3; i++) {
            // Lê dois bytes (low byte primeiro, depois o high byte)
            uint8_t low_byte = *((uint32_t *)(i2c_base + IC_DATA_CMD_REG)) & 0xFF;
            uint8_t high_byte = *((uint32_t *)(i2c_base + IC_DATA_CMD_REG)) & 0xFF;
            accel_data[i] = (int16_t)((high_byte << 8) | low_byte);  // Combinar os dois bytes
        }
}
    