#include <msp430.h>
#define TIMER_COUNT 62500

unsigned int estado = 0;
unsigned int cont = 0;
unsigned int cont_ativo = 0; // false

void apaga_led(){
    P1OUT = 0x00;
}

void acende_led(){
    P1OUT = BIT0;
}

void pisca_led(){
    P1OUT ^= BIT0;
    __delay_cycles(100000);
}

void iniciar_cont(){
    cont_ativo = 1; // true
}

void zerar_cont(){
    cont_ativo = 0;
    cont = 0;
}

void maquina_estados(){
    if(estado == 0){
        apaga_led();
        zerar_cont();
    }else if(estado == 1){
        acende_led();
        iniciar_cont();
        if(cont == 10){    //monitor the flag status
          estado = 0;
        }
    }else if(estado == 2){
        pisca_led();
        iniciar_cont();
        if(cont == 10){    //monitor the flag status
          estado = 0;
        }
    }
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P1DIR |= BIT0; // Define se bit entrada ou saida)
    P1OUT = 0x00;

    BCSCTL1 = CALBC1_1MHZ; //set clock to 1MHz
    DCOCTL = CALDCO_1MHZ;

    TACCR0 = TIMER_COUNT;   //Timer Count for value 1sec
    TACTL = TASSEL_2 + ID_3 + MC_3;

    _enable_interrupts();
    TACCTL0 = CCIE;

    P1IE |= BIT3;
    P1IES = 0x00;

    P1IFG = 0x00; //registrador referente a flag

    while(1){
        maquina_estados();
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void){
    zerar_cont();
    estado++;
    if(estado > 2){
        estado = 0;
    }
    P1IFG = 0x00;
}


#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A(void) {
    if(cont_ativo == 1 && cont < 10){
        cont ++;
    }

}
