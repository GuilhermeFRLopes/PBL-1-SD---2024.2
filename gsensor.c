#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define I2C_DEVICE "/dev/i2c-0"
#define ADXL345_ADDR 0x53
#define DATA_FORMAT 0x31
#define DATAX0 0x32
#define DATAX1 0x33
#define DATAY0 0x34
#define DATAY1 0x35
#define DATAZ0 0x36
#define DATAZ1 0x37

int main() {
    int file;
    char buf[6];
    
    // Abre o dispositivo I2C
    if ((file = open(I2C_DEVICE, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        return 1;
    }

    // Especifica o endereço do G-sensor
    if (ioctl(file, I2C_SLAVE, ADXL345_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        close(file);
        return 1;
    }

    // Configura o G-sensor para resolução total e ±16g
    char config[2] = {DATA_FORMAT, 0x0B}; // 0x0B para full resolution, ±16g
    if (write(file, config, 2) != 2) {
        perror("Failed to write to the i2c bus");
        close(file);
        return 1;
    }

    // Lê os valores de X, Y e Z
    while (1) {
        // Lê os dados de aceleração
        if (read(file, buf, 6) != 6) {
            perror("Failed to read from the i2c bus");
            close(file);
            return 1;
        }

        // Converte os dados para valores de aceleração
        int16_t x = (buf[1] << 8) | buf[0];
        int16_t y = (buf[3] << 8) | buf[2];
        int16_t z = (buf[5] << 8) | buf[4];

        // Exibe os valores
        printf("X: %d, Y: %d, Z: %d\n", x, y, z);

        // Aguarda um segundo antes da próxima leitura
        sleep(1);
    }

    // Fecha o dispositivo I2C
    close(file);
    return 0;
}