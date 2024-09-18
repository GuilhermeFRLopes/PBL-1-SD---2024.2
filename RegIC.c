#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define I2C0_BASE_ADDR   0xFFC04000    // Endereço base do controlador I2C0
#define IC_CON_REG       0x00          // Offset do registrador IC_CON
#define IC_TAR_REG       0x04          // Offset do registrador IC_TAR
#define IC_DATA_CMD_REG  0x10          // Offset do registrador IC_DATA_CMD
#define IC_ENABLE_REG    0x6C          // Offset do registrador IC_ENABLE
#define IC_RXFLR_REG     0x78          // Offset do registrador IC_RXFLR
#define MAP_SIZE         0x1000        // Tamanho do mapeamento de memória

// Endereço I2C do ADXL345
#define ADXL345_ADDR     0x53

// Registradores internos do ADXL345
#define ADXL345_REG_DATA_X0 0x32       // Registrador inicial dos dados X, Y, Z (6 bytes)

int main() {
    int fd;
    void *i2c_base;

    // Abrir /dev/mem para acessar a memória do sistema
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Erro ao abrir /dev/mem");
        return -1;
    }

    // Mapear a memória do controlador I2C0
    i2c_base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, I2C0_BASE_ADDR);
    if (i2c_base == MAP_FAILED) {
        perror("Erro ao mapear a memória do I2C");
        close(fd);
        return -1;
    }

    // 1. Configurar o IC_CON para modo mestre, endereçamento de 7 bits, e modo rápido (400 kbit/s)
    uint32_t ic_con_value = 0x65;
    *((uint32_t *)(i2c_base + IC_CON_REG)) = ic_con_value;
    printf("Registrador IC_CON configurado com valor: 0x%X\n", ic_con_value);

    // 2. Configurar o IC_TAR com o endereço do ADXL345
    uint32_t ic_tar_value = ADXL345_ADDR;  // Endereço de escravo (7 bits)
    *((uint32_t *)(i2c_base + IC_TAR_REG)) = ic_tar_value;
    printf("Registrador IC_TAR configurado com valor: 0x%X\n", ic_tar_value);

    // 3. Habilitar o I2C0
    *((uint32_t *)(i2c_base + IC_ENABLE_REG)) = 0x1;
    printf("I2C habilitado\n");

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

    // Imprimir os valores dos eixos X, Y, Z
    printf("Aceleração em X: %d\n", accel_data[0]);
    printf("Aceleração em Y: %d\n", accel_data[1]);
    printf("Aceleração em Z: %d\n", accel_data[2]);

    // 8. Desabilitar o I2C0 após a operação
    *((uint32_t *)(i2c_base + IC_ENABLE_REG)) = 0x0;
    printf("I2C desabilitado\n");

    // Desmapear a memória e fechar o arquivo de memória
    munmap(i2c_base, MAP_SIZE);
    close(fd);

    return 0;
}
