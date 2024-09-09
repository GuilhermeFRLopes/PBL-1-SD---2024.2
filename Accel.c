#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <../auxFiles/address_map_arm.h>


/* Possivies registradores:
    POWER_CTL(0X2D)  
    DATA_REGISTERS(0X32-0X37)
 */


#define ADC_BASE 0x00004000  // Defina o endereço base correto para o acelerômetro
#define LW_BRIDGE_BASE 0xFF200000
#define LW_BRIDGE_SPAN 0x00005000

int main(void){

    volatile * accel_ptr;
    int fd = -1;
    void *LW_virtual;

    if (fd = open("dev/men", (O_RDWR | O_SYNC)) == -1){
        printf("ERROR : could not open \"/ dev / mem \"...\n")
        return (-1)
    }


    LW_virtual = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, LW_BRIDGE_BASE);
    if(LW_virtual == MAP_FAILED){
        printf (" ERROR : mmap () failed ...\n") ;
        close(fd)
        return(-1)
    }
    // Definir o ponteiro para o endereço virtual do registrador de dados do acelerômetro
    accel_ptr = (int *) (LW_virtual + ADC_BASE + 0x32);  // Registrador de dados X0

    // Ler os valores de aceleração (por exemplo, no eixo X)
    int accel_data_x = *accel_ptr;  // Ler o valor da aceleração no eixo X
    printf("Leitura da aceleração no eixo X: %d\n", accel_data_x);

    close(fd)
}