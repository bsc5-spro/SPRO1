#include "nextion.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#define NUMBER_STRING 1001

uint32_t readValue;

int read_value(void);

void init_display(void){
    char readBuffer[100];
	//uart_init();//initialize communication with PC - debugging
	//io_redirect();//redirect printf function to uart, so text will be shown on PC
	
	printf("page 0%c%c%c",255,255,255);//init at 9600 baud.
	_delay_ms(20);
	readValue = 0;
}

int get_value(char component[]){
    printf("get %s.val%c%c%c", component, 255, 255, 255);
    return read_value();
}

void set_value(char component[], int val){
    printf("%s.val=%d%c%c%c", component, val, 255, 255, 255);
}

int read_value(void){
    char readBuffer[100];
    int typeExpected = 0;
    readValue = 0;
    for(int i = 0; i<8;i++){
        scanf("%c", &readBuffer[i]);
            if(readBuffer[i] == 0x71)//Expect number string
            {
                typeExpected = NUMBER_STRING;
                readBuffer[0] = 0x71;//Move indicator to front, just to keep the nice format
                break;
            }
            if(readBuffer[i] == 0x1A)//some error occurred - retrieve the 0xFF commands and start over
            {
                scanf("%c", &readBuffer[i]);
                scanf("%c", &readBuffer[i]);
                scanf("%c", &readBuffer[i]);
                continue;
            }
    }

    if(typeExpected == NUMBER_STRING){
        for(int i = 1; i<8; i++)
        {
            scanf("%c", &readBuffer[i]);
        }
        if(readBuffer[0] == 0x71 && readBuffer[5] == -1 && readBuffer[6] == -1 && readBuffer[7] == -1)//This is a complete number return
        {
            readValue = readBuffer[1] | (readBuffer[2] << 8) | (readBuffer[3] << 16)| (readBuffer[4] << 24);
        }

    }
    
    

    return readValue;

}	