#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#define I2C_BASE_ADDR 0xFF708000 // Endereço base do controlador I2C na DE1-SoC
#define G_SENSOR_ADDR 0x53
#define I2C_DATA_REG_OFFSET 0x00 // Offset do registrador de dados do I2C
#define I2C_CMD_REG_OFFSET 0x04  // Offset do registrador de comando do I2C

// Registradores do ADXL345
#define DATAX0 0x32
#define DATAX1 0x33
#define DATAY0 0x34
#define DATAY1 0x35
#define DATAZ0 0x36
#define DATAZ1 0x37

volatile uint32_t *i2c_base;

void i2c_write(uint8_t reg, uint8_t value) {
    // Escreve o endereço do registrador
    *(i2c_base + I2C_CMD_REG_OFFSET) = (G_SENSOR_ADDR << 1) | 0; // Endereço do dispositivo e operação de escrita
    *(i2c_base + I2C_DATA_REG_OFFSET) = reg; // Endereço do registrador
    *(i2c_base + I2C_DATA_REG_OFFSET) = value; // Valor a ser escrito
    // Inicia a operação de escrita (assumindo que há um bit de controle para isso)
    // *(i2c_base + I2C_CMD_REG_OFFSET) |= (1 << START_BIT); // Exemplo de bit de controle
}

void i2c_read(uint8_t reg, uint8_t *data, size_t length) {
    // Escreve o endereço do registrador
    *(i2c_base + I2C_CMD_REG_OFFSET) = (G_SENSOR_ADDR << 1) | 0; // Endereço do dispositivo e operação de escrita
    *(i2c_base + I2C_DATA_REG_OFFSET) = reg; // Endereço do registrador
    // Inicia a operação de leitura
    *(i2c_base + I2C_CMD_REG_OFFSET) = (G_SENSOR_ADDR << 1) | 1; // Endereço do dispositivo e operação de leitura

    // Lê os dados
    for (size_t i = 0; i < length; i++) {
        data[i] = *(i2c_base + I2C_DATA_REG_OFFSET); // Lê o dado
    }
}

int main() {
    int fd;
    uint8_t buf[6];

    // Abre o dispositivo de memória
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("Failed to open /dev/mem");
        return 1;
    }

    // Mapeia a memória do controlador I2C
    i2c_base = (uint32_t *)mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, I2C_BASE_ADDR);
    if (i2c_base == MAP_FAILED) {
        perror("Failed to mmap");
        close(fd);
        return 1;
    }

    // Lê os dados de aceleração
    i2c_read(DATAX0, buf, 6);

    // Converte os dados para valores de aceleração
    int16_t x = (buf[1] << 8) | buf[0]; // X
    int16_t y = (buf[3] << 8) | buf[2]; // Y
    int16_t z = (buf[5] << 8) | buf[4]; // Z

    // Exibe os valores
    printf("X: %d, Y: %d, Z: %d\n", x, y, z);

    // Desmapeia a memória
    munmap((void *)i2c_base, getpagesize());
    close(fd);
    return 0;
}
