#include "nextion.h"
#include <stdio.h>
#include <usart.h>
#define NUMBER_STRING 1001

uint32_t get_value(char component[]);
uint32_t read_value(void);

void init_display(void){
    char readBuffer[100];
	//uart_init();//initialize communication with PC - debugging
	//io_redirect();//redirect printf function to uart, so text will be shown on PC
	
	printf("page 0%c%c%c",255,255,255);//init at 9600 baud.
	_delay_ms(20);
	uint32_t readValue = 0;
}

uint32_t get_value(char component[]){
    printf("get.%s.val%c%c%c", component, 255, 255, 255);
    return read_value();
}

uint32_t read_value(void){
    char readBuffer[100];
    int typeExpected = 0;
    uint32_t readValue = 0;
for(int i = 0; i<8;i++){
    scanf("%c", &readBuffer[i]);
        if(readBuffer[i] == 0x71)//Expect number string
        {
            typeExpected = NUMBER_STRING;
            readBuffer[0] = 0x71;//Move indicator to front, just to keep the nice format
            break;
        }
    }
    if(typeExpected == NUMBER_STRING){
        for(int i = 1; i<8; i++)
        {
            canf("%c", &readBuffer[i]);
        }
        if(readBuffer[0] == 0x71 && readBuffer[5] == 0xFF && readBuffer[6] == 0xFF && readBuffer[7] == 0xFF)//This is a complete number return
        {
            readValue = readBuffer[1] | (readBuffer[2] << 8) | (readBuffer[3] << 16)| (readBuffer[4] << 24);
        }
    }

for(int i = 0; i<7; i++){
    scanf("%c", &readBuffer[i]);
    if(readBuffer[i] == 0x1A)//some error occurred - retrieve the 0xFF commands and start over
    {
        scanf("%c", &readBuffer[i]);
        scanf("%c", &readBuffer[i]);
        scanf("%c", &readBuffer[i]);
        continue;
    }
}

return readValue;

}	