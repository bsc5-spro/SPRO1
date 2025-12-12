#include "nextion.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <util/delay.h>

#define NUMBER_STRING 1001
#define SET 2001
#define CONTROL 3001

uint32_t readValue;

int read_value(void);

void init_display(void){
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

void set_property(char component[], char property[], int val){
    printf("%s.%s=%d%c%c%c", component, property, val, 255, 255, 255);
}

int read_value(void){
    char readBuffer[100];
    int typeExpected = 0;
    readValue = 0;
    for(int i = 0; i<8;i++){
        scanf("%c", &readBuffer[i]);
        switch (readBuffer[i])
        {
        case 0x71:
            typeExpected = NUMBER_STRING;
            readBuffer[0] = 0x71;//Move indicator to front, just to keep the nice format
            break;
        case 0x11:
            typeExpected = CONTROL;
            readBuffer[0] = 0x11;
            break;
        case 0x33:
            typeExpected = SET;
            readBuffer[0] = 0x33;
            break;
        case 0x1A:
            scanf("%c", &readBuffer[i]);
            scanf("%c", &readBuffer[i]);
            scanf("%c", &readBuffer[i]);
            continue;
            break;
        default:
            break;
        }
    }

    switch (typeExpected)
    {
    case NUMBER_STRING:
        for(int i = 1; i<8; i++)
        {
            scanf("%c", &readBuffer[i]);
        }
        if(readBuffer[0] == 0x71 && readBuffer[5] == -1 && readBuffer[6] == -1 && readBuffer[7] == -1)//This is a complete number return
        {
            readValue = readBuffer[1] | (readBuffer[2] << 8) | (readBuffer[3] << 16)| (readBuffer[4] << 24);
        }
        break;
    case SET:
        for(int i = 1; i<5; i++)
        {
            scanf("%d", &readBuffer[i]);
        }
        if(readBuffer[0] == 0x33 && readBuffer[2] == -1 && readBuffer[3] == -1 && readBuffer[4] == -1)//This is a complete number return
        {
            readValue = readBuffer[1] ;
        }
        break;
    case CONTROL:
    for(int i = 1; i<5; i++)
        {
            scanf("%d", &readBuffer[i]);
        }
        if(readBuffer[0] == 0x11 && readBuffer[2] == -1 && readBuffer[3] == -1 && readBuffer[4] == -1)//This is a complete number return
        {
            readValue = readBuffer[1] ;
            if (readValue == 1){ // start button
                return 0;
            } else if (readValue == 2){ // back button
                return -1;
            }
        }
        break;
    
    default:
        break;
    }
    

    return readValue;

}	

fixed read_numpad(void){
    fixed number;
    int numberBuffer[20];
    int index = 0;
    number.decimalPlace = 0; // place of decimal point from the right
    char decimalSet = 0;
    char receivingNumbers = 1;

    while (receivingNumbers){

        int action = read_value();
        switch (action)
        {
        case 11:
            numberBuffer[index]=-1;
            index++;
            break;
        case 12:
            index--;
            break;
        case -1:
            receivingNumbers=0;
            break;
        default:
            numberBuffer[index]=action;
            index++;
            break;
        }

        int multiplier=1.0;
        int count = 0;
        number.i_number = 0;
        for (int i = index -1; i >=0; i--){
            if (numberBuffer[i]==-1){
                number.decimalPlace = count;
            } else{
                number.i_number += (float)numberBuffer[i]*multiplier;
                multiplier*=10;
                count++;
            }
        }
        set_value("inpnum", number.i_number);
        set_property("inpnum", "vvs1", number.decimalPlace);
    }

    if (number.decimalPlace == 0)
        number.f_number = (float)number.i_number;
    else
        number.f_number = (float)number.i_number/pow(10, number.decimalPlace);

    return number;
}