#include <SimpleSerialBT.h>
#include <RGB.h>

char buffer[8];
int color;

SerialBT bt;
RGB foco;

uint8_t rgb_pins[]={27,14,12};
uint8_t rgb_ch[]={0,1,2};

