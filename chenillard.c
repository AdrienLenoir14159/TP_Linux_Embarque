#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <fcntl.h> 
#include <unistd.h>


#define NUMBER_OF_LED 10
#define DELAY 1000

static char fileName[50];

static void generate_file_name_led_x(int x){
    char* base_url = "/sys/class/leds/fpga_led%d/brightness";
    int j = snprintf(fileName, strlen(base_url), base_url , x);
}

static void turn_on_led_x(){
    FILE * output_file = fopen(fileName, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fwrite("1", 1, strlen("1"), output_file);
    fclose(output_file);    
}

static void turn_off_led_x(){
    FILE * output_file = fopen(fileName, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fwrite("0", 1, strlen("0"), output_file);
    fclose(output_file);    
}

static void delay(int x){
    int i;
    for(i=0; i<x*10000; i++);
}

int main(int arc, char* argv[]){
    int i;
    printf("Starting Chenillard \r\n");
    while(1){
        for(i = 0; i < NUMBER_OF_LED; i++){
            generate_file_name_led_x(i);
            turn_on_led_x();
            delay(DELAY); // DELAY*10 000 = 10 000 000 empty for loop cycles
            turn_off_led_x();
        }
    }
    return 1;
}