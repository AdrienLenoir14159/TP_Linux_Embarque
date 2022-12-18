#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <fcntl.h> 
#include <unistd.h>


#define NBR_LEDS 10
#define DELAY 10000000
#define EXIT_WHEN_POWER_ON 2
#define EXIT_WHEN_POWER_OFF 3

static char file[100];
char* directory = "/sys/class/leds/fpga_led%d/brightness";

static void change_led_file(int n){ // Changes current led file to led<x> file
    snprintf(file, strlen(directory), directory , n);
}

static void power_ON_led(){ // Powers ON the led linked to the currently used file
    FILE * output_file = fopen(file, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_WHEN_POWER_ON); // Exit code : 2
    }
    fwrite("1", 1, strlen("1"), output_file);
    fclose(output_file);    
}

static void power_OFF_led(){ // Powers OFF the led linked to the currently used file
    FILE * output_file = fopen(file, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_WHEN_POWER_OFF); // Exit code : 3
    }
    fwrite("0", 1, strlen("0"), output_file);
    fclose(output_file);    
}

int main(int arc, char* argv[]){
    printf("\r\n==== Starting Chenillard ====\r\n");
    while(1){
        for(int indice_LED = 0; indice_LED < NBR_LEDS; indice_LED++){
            change_led_file(indice_LED);
            power_ON_led();
            for(int d = 0; d < DELAY; d++); // Artificial delay
            power_OFF_led();
        }
    }
    return 0;
}