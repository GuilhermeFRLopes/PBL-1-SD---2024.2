#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define I2C0_BASE_ADDR  0xFFC04000    // Endereço base do controlador I2C0
#define IC_CON_REG      0x00          // Offset do registrador IC_CON
#define MAP_SIZE        0x1000        // Tamanho do mapeamento de memória

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

    // Configurar o registrador IC_CON para:
    // - Mestre
    // - Modo rápido (400 kbit/s)
    // - Endereçamento de 7 bits
    // - Reinício habilitado
    uint32_t ic_con_value = 0x65;
    *((uint32_t *)(i2c_base + IC_CON_REG)) = ic_con_value;

    printf("Registrador IC_CON configurado com valor: 0x%X\n", ic_con_value);

    // Desmapear a memória e fechar o arquivo de memória
    munmap(i2c_base, MAP_SIZE);
    close(fd);

    return 0;
}
