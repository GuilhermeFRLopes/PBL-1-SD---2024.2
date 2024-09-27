#ifndef ACCEL_H
#define ACCEL_H

#include <stdint.h>
#include <stdbool.h>


// Endereço I2C do ADXL345
#define ADXL345_ADDR 0x53

// Registradores internos do ADXL345
#define ADXL345_REG_DATA_X0 0x32 // Registrador inicial dos dados X, Y, Z (6 bytes)

// Endereço I2C do ADXL345
#define ADXL345_ADDR 0x53

// Registradores internos do ADXL345
#define ADXL345_REG_DATA_X0 0x32 // Registrador inicial dos dados X, Y, Z (6 bytes)

// Funções para gerenciamento do acelerômetro
bool inicializar_acelerometro(int fd, void **i2c_base);

void ler_dados_acelerometro(void *i2c_base, int16_t *dados);

#endif // ACELEROMETRO_H
