//************************************************************************** 
// Author: Brandon Garrido y Margareth Vela
// Carnet: 19421 / 19458
// Fecha de creación: 24/10/2021
// Ultima modificación: 24/10/2021
//**************************************************************************


// PIC16F887 Configuration Bit Settings

// 'C' source line config statements



//------------------------------------------------------------------------------
//                          Importación de librerías
//------------------------------------------------------------------------------  

#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
//------------------------------------------------------------------------------
//                          Directivas del compilador
//------------------------------------------------------------------------------  

#define _XTAL_FREQ 4000000 //Para delay



//------------------------------------------------------------------------------
//                       Palabras de configuración
//------------------------------------------------------------------------------  

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits 
#pragma config WDTE = OFF       // Watchdog Timer Enable bit 
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit 
#pragma config CP = OFF         // Code Protection bit 
#pragma config CPD = OFF        // Data Code Protection bit 
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit 
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit 
#pragma config LVP = OFF        // Low Voltage Programming Enable bit 

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit 
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits 

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.




//------------------------------------------------------------------------------
//                          Variables
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
//                          Prototipos
//------------------------------------------------------------------------------
void setup(void);
void IOCB_interrupt(void);



//------------------------------------------------------------------------------
//                          Loop principal
//------------------------------------------------------------------------------


void main(void) {

    setup(); //configuraciones de modulos y timers
    
    while(1){ //loop forever
        
    }
}




void __interrupt() isr(void){


    if(INTCONbits.RBIF){ // verifica si se presionó algún push 
        IOCB_interrupt();
    }

}

void IOCB_interrupt(){ //rutinas de interrupción para cada push 

    
    if (PORTBbits.RB0 == 0){ // Pulso instrucción de subir 
        PORTDbits.RD0 = 0;
        __delay_ms(50);
        PORTDbits.RD0 = 1;
    }else if (PORTBbits.RB1 == 0){ // Pulso instrucción de bajar 
        PORTDbits.RD1 = 0;
        __delay_ms(50);
        PORTDbits.RD1 = 1; 
    }else if (PORTBbits.RB2 == 0){ // Pulso instrucción para girar derecha 
        PORTDbits.RD2 = 0;
        __delay_ms(50);
        PORTDbits.RD2 = 1;
    }else if (PORTBbits.RB3 == 0){ // Pulso instrucción para girar izquierda 
        PORTDbits.RD3 = 0;
        __delay_ms(50);
        PORTDbits.RD3 = 1;  
    }
    
    INTCONbits.RBIF = 0;
    
    return;
} 



void setup(){
    
     
    
    //configuración de reloj interno
    OSCCONbits.IRCF = 0x06; // 110 - 4MHz
    OSCCONbits.SCS = 1; // Habilitar reloj interno
    //configuración interrupciones
    
   
    //configuración interrupciones
    INTCONbits.GIE = 1; // Se habilitan las interrupciones globales
    INTCONbits.RBIE = 1; // habilitar banderas de interrupción puertos B
       
    
    //configuración I/O
    ANSELH = 0;
    ANSEL = 0;

    TRISB = 0xFF; // habilitar pines RB0, RB1, RB2, RB3 como inputs
    TRISD = 0x00; // habilitar pines RD0, RD1, RD2, RD3 como outputs

    OPTION_REGbits.nRBPU =  0 ; // se habilita el pull up interno en PORTB
    WPUB = 0xFF;  // se habilita los pull ups para los pines del puerto B
    
    
    PORTB = 0xFF; // Estados iniciales de las inputs en pullup
  
    PORTD= 0xFF; // Estados iniciales de las salidas en pullup

    INTCONbits.RBIF = 0;  
    //configuración iocb
    IOCB = 0x0F; // setear interrupciones en los pines RB0 y RB1 
    


}



