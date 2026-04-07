#include <ch32v20x.h>

int main(){
        RCC->APB1PCENR |= (1<<22);
}
