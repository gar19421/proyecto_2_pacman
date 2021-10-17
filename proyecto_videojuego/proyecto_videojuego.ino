//************************************************************************** 
// Author: Brandon Garrido y Margareth Vela
// Carnet: 19421 / 19458
// Fecha de creación: 12/10/2021
// Ultima modificación: .../10/2021
//**************************************************************************


//-------Proyecto 2 Electrónica Digital 2---------//
//--------------Videojuego - PACMAN--------------//


#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

//#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

//Incluimos librerias necesarias
#include <SPI.h>
#include <SD.h>

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};  


//#define SW1 PF_4
//#define SW2 PF_0


//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);


//extern uint8_t fondo[];
//extern uint8_t uvg[];
extern uint8_t  sprite_pacman[];
extern uint8_t  sprite_pacman_toup[];
extern uint8_t  sprite_pacman_todown[];
extern uint8_t  sprite_ghost[];


uint8_t contador = 0;

uint8_t state_pacman = 0x00;
uint8_t flag_pacman = 0x00; // arriba 1, abajo 2, derecha 4, izquierda 3

uint8_t state_ghost = 0x00;
uint8_t flag_ghost= 0x00;

//mapeo de coordenadas
uint16_t pacmanx = 12;
uint8_t pacmany = 107;
uint16_t ghostx= 282;
uint8_t ghosty= 122;

uint8_t index1;

volatile byte state = LOW;
File myFile;

//***************************************************************************************************************************************
// Initialization
//***************************************************************************************************************************************
void setup() {


  // se inicializa la comunicación con la sd
  Serial.begin(9600);  //iniciamos comunicacion serial
  SPI.setModule(0);  //iniciamos comunicacion SPI en el modulo 0
  Serial.print("Initializing SD card...");
  
  pinMode(12, OUTPUT);  //Colocamos el CS del PA3
  
//Se verifica que se haya iniciado correctamente la SD
  if (!SD.begin(12)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  //Se inicializa la pantalla lcd
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  //Serial.begin(9600); ya se inicializó con la lcd
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Start");
  LCD_Init();
  LCD_Clear(0x00);
  
  //LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
  
  //LCD_Bitmap(0, 0, 320, 240, fondo);
  uploadBackgroundSD(0, 0, 320, 240,"prueba.txt");
  delay(3000);// push
  
  LCD_Clear(0x00);

  //dibujar bordes
  H_line(10, 10,300, 0x09b9b9b);//de izquierda a derecha
  V_line(10, 10, 95, 0x09b9b9b);
  V_line(310, 10, 95,0x09b9b9b);//top

  V_line(10, 135, 95, 0x09b9b9b);//bottom
  V_line(310, 135, 95,0x09b9b9b);
  H_line(10, 230,300, 0x09b9b9b);

  //dibujar laberinto
  Rect(40,40,10, 160, 0x09b9b9b);//de izquierda a derecha
  
  Rect(80,40,10, 70,  0x09b9b9b);
  Rect(80,140,10, 60, 0x09b9b9b);//vertical
  Rect(90,40,90, 10,  0x09b9b9b);
  Rect(90,190,190, 10, 0x09b9b9b); //horizontal

  Rect(120,80,60, 30, 0x09b9b9b); //bloques centrales externos
  Rect(120,140,60, 20, 0x09b9b9b);
  //Rect(130,90,50, 10, 0x09b9b9b); //bloques centrales internos
  //Rect(130,150,40, 10, 0x09b9b9b);
  String text1 = "Pts: 1"; //texto interno
  LCD_Print(text1, 130,90, 1, 0xffff, 0x00);

  Rect(210,40,10, 120, 0x09b9b9b);//Gusanito / vertical
  Rect(220,150,60, 10, 0x09b9b9b);

  Rect(250,40,20, 80, 0x09b9b9b); //bloque grande final

  
  //FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c)
 // FillRect(80, 60, 160, 120, 0x0400);

  //LCD_Print(String text, int x, int y, int fontSize, int color, int background)
 // String text1 = "LAB 08";
  //LCD_Print(text1, 110, 110, 2, 0xffff, 0x0000);
  initialState();
 }



//***************************************************************************************************************************************
// Loop
//***************************************************************************************************************************************
void loop() {
  delay(11);
  //Movimiento para pacman
  if ( state_pacman == 0 && flag_pacman == 1){ //posibilidad pacman hacia arriba

    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany + 26, 26, 0x00);

    if(pacmany == 12){
      state_pacman = 1;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 0  && flag_pacman == 2){//posibilidad pacman hacia abajo
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany - 1, 26, 0x00);
    
    if(pacmany == 202){
      state_pacman = 2;
      flag_pacman = 0;
    }
    
  }


  if ( state_pacman == 1 && flag_pacman == 4){ // codigo estado 1 (arriba)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 52){
      state_pacman = 3;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 1  && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany - 1, 26, 0x00);
    
    if(pacmany == 202){
      state_pacman = 2;
      flag_pacman = 0;
    }
    
  }
  

  if ( state_pacman == 2 && flag_pacman == 4){ // codigo estado 2 (abajo)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 52){
      state_pacman = 4;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 2  && flag_pacman == 1){
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 12){
      state_pacman = 1;
      flag_pacman = 0;
    }
    
  }


  if ( state_pacman == 3 && flag_pacman == 4){ // codigo estado 3 (arriba->derecha)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 182){
      state_pacman = 5;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 3  && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 112){
      state_pacman = 6;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 3  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx + 26, pacmany, 26, 0x00);
    
    if(pacmanx == 12){
      state_pacman = 1;
      flag_pacman = 0;
    }
    
  }


  if ( state_pacman == 4 && flag_pacman == 4){ // codigo estado 4 (abajo->derecha)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 282){
      state_pacman = 7;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 4  && flag_pacman == 1){
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 112){
      state_pacman = 6;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 4  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx + 26, pacmany, 26, 0x00);
    
    if(pacmanx == 12){
      state_pacman = 2;
      flag_pacman = 0;
    }
    
  }


                                                //estado 5 está despues
  if ( state_pacman == 6 && flag_pacman == 4){ // codigo estado 6 (centro izquierda)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 92){
      state_pacman = 8;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 6  && flag_pacman == 1){
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 12){
      state_pacman = 3;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 6  && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 202){
      state_pacman = 4;
      flag_pacman = 0;
    }
    
  }


                                                  //estado 7 está despues
  if ( state_pacman == 8 && flag_pacman == 4){ // codigo estado 8 (centro)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 182){
      state_pacman = 9;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 8  && flag_pacman == 1){
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 52){
      state_pacman = 10;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 8  && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 162){
      state_pacman = 11;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 8  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx +26, pacmany, 26, 0x00);
    
    if(pacmanx == 52){
      state_pacman = 6;
      flag_pacman = 0;
    }
    
  }




  if( state_pacman == 9  && flag_pacman == 1){// codigo estado 9 (centro)
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 52){
      state_pacman = 13;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 9  && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 162){
      state_pacman = 12;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 9  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx +26, pacmany, 26, 0x00);
    
    if(pacmanx == 92){
      state_pacman = 8;
      flag_pacman = 0;
    }
    
  }
  



 if ( state_pacman == 10 && flag_pacman == 4){ // codigo estado 10 (centro)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 182){
      state_pacman = 13;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 10 && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 112){
      state_pacman = 8;
      flag_pacman = 0;
    }
    
  }



  if ( state_pacman == 11 && flag_pacman == 4){ // codigo estado 11 (centro)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 182){
      state_pacman = 12;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 11 && flag_pacman == 1){
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 112){
      state_pacman = 8;
      flag_pacman = 0;
    }
    
  }




  if ( state_pacman == 5 && flag_pacman == 4){ // codigo estado 5 (centro->derecha)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 222){
      state_pacman = 14;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 5  && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 52){
      state_pacman = 13;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 5  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx +26, pacmany, 26, 0x00);
    
    if(pacmanx == 52){
      state_pacman = 3;
      flag_pacman = 0;
    }
    
  }




  if( state_pacman == 13  && flag_pacman == 1){// codigo estado 13 (centro->derecha)
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 12){
      state_pacman = 5;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 13  && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 112){
      state_pacman = 9;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 13  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx +26, pacmany, 26, 0x00);
    
    if(pacmanx == 92){
      state_pacman = 10;
      flag_pacman = 0;
    }
    
  }




  if ( state_pacman == 12 && flag_pacman == 4){ // codigo estado 12 (centro->derecha)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 282){
      state_pacman = 18;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 12  && flag_pacman == 1){
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 112){
      state_pacman = 9;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 12  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx +26, pacmany, 26, 0x00);
    
    if(pacmanx == 92){
      state_pacman = 11;
      flag_pacman = 0;
    }
    
  }




  if( state_pacman == 7  && flag_pacman == 1){// codigo estado 7 (centro->derecha)
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 162){
      state_pacman = 18;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 7  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx +26, pacmany, 26, 0x00);
    
    if(pacmanx == 52){
      state_pacman = 4;
      flag_pacman = 0;
    }
    
  }





  if ( state_pacman == 14 && flag_pacman == 4){ // codigo estado 14 (derecha)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 282){
      state_pacman = 15;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 14  && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 122){
      state_pacman = 16;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 14  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx +26, pacmany, 26, 0x00);
    
    if(pacmanx == 182){
      state_pacman = 5;
      flag_pacman = 0;
    }
   
  }




  if( state_pacman == 15  && flag_pacman == 2){// codigo estado 15 (derecha)
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 122){
      state_pacman = 17;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 15  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx +26, pacmany, 26, 0x00);
    
    if(pacmanx == 222){
      state_pacman = 14;
      flag_pacman = 0;
    }
    
  }



  if ( state_pacman == 16 && flag_pacman == 4){ // codigo estado 16 (derecha)

    index1 = (pacmanx/11)%3;
    pacmanx ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
    V_line( pacmanx-1, pacmany, 26, 0x00);

    if(pacmanx == 282){
      state_pacman = 17;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 16  && flag_pacman == 1){
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 12){
      state_pacman = 14;
      flag_pacman = 0;
    }
    
  }


  
  
  if( state_pacman == 17  && flag_pacman == 1){// codigo estado 17 (derecha)
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 12){
      state_pacman = 15;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 17  && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 162){
      state_pacman = 18;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 17  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx +26, pacmany, 26, 0x00);
    
    if(pacmanx == 222){
      state_pacman = 16;
      flag_pacman = 0;
    }
    
  }




 if( state_pacman == 18  && flag_pacman == 1){// codigo estado 18 (centro)
    index1 = (pacmany/11)%3;
    pacmany --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_toup,3,index1,0,0);
    H_line( pacmanx, pacmany +26, 26, 0x00);
    
    if(pacmany == 122){
      state_pacman = 17;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 18  && flag_pacman == 2){
    index1 = (pacmany/11)%3;
    pacmany ++;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman_todown,3,index1,0,0);
    H_line( pacmanx, pacmany -1, 26, 0x00);
    
    if(pacmany == 202){
      state_pacman = 7;
      flag_pacman = 0;
    }
    
  }else if( state_pacman == 18  && flag_pacman == 3){
    index1 = (pacmanx/11)%3;
    pacmanx --;
    LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,1,0);
    V_line( pacmanx +26, pacmany, 26, 0x00);
    
    if(pacmanx == 182){
      state_pacman = 12;
      flag_pacman = 0;
    }
    
  }

   //Movimiento para ghost
  //STATE_GHOST 0
  if (state_ghost == 0 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 12){
      state_ghost = 1;
      flag_ghost = 0;
    }
  }else if( state_ghost == 0 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 162){
      state_ghost = 2;
      flag_ghost = 0;
    }
       
  }else if( state_ghost == 0 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 222){
      state_ghost = 4;
      flag_ghost = 0;
    }
  }
  //STATE_GHOST 1
  if (state_ghost == 1 && flag_ghost == 3){

    index1 = (ghostx/11)%4;
    ghostx --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx + 26, ghosty, 26, 0x00);
    
    if(ghostx == 222){
      state_ghost = 5;
      flag_ghost = 0;
    }
  }else if( state_ghost == 1 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 122){
      state_ghost = 0;
      flag_ghost = 0;
      }
    }
  //STATE_GHOST 2
  if (state_ghost == 2 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 122){
      state_ghost = 0;
      flag_ghost = 0;
    }
  }else if( state_ghost == 2 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 202){
      state_ghost = 3;
      flag_ghost = 0;
      }
  }else if( state_ghost == 2 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 182){
      state_ghost = 6;
      flag_ghost = 0;
    }
  }
  //STATE_GHOST 3
  if (state_ghost == 3 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 162){
      state_ghost = 2;
      flag_ghost = 0;
    }
  }else if( state_ghost == 3 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 52){
      state_ghost = 7;
      flag_ghost = 0;
    }
  }
  //STATE_GHOST 4
  if (state_ghost == 4 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 12){
      state_ghost = 5;
      flag_ghost = 0;
    }
  }else if( state_ghost == 4 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 282){
      state_ghost = 0;
      flag_ghost = 0;
    }
  }
  //STATE_GHOST 5
  if( state_ghost == 5 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 122){
      state_ghost = 4;
      flag_ghost = 0;
      }
  }else if( state_ghost == 5 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 182){
      state_ghost = 8;
      flag_ghost = 0;
    }
  }else if( state_ghost == 5 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 282){
      state_ghost = 1;
      flag_ghost = 0;
    }
  }

  //STATE_GHOST 6
  if (state_ghost == 6 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 112){
      state_ghost = 10;
      flag_ghost = 0;
    }
  }else if( state_ghost == 6 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 92){
      state_ghost = 9;
      flag_ghost = 0;
    }
  }else if( state_ghost == 6 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 282){
      state_ghost = 2;
      flag_ghost = 0;
    }
  }

  //STATE_GHOST 7
    if (state_ghost == 7 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 112){
      state_ghost = 12;
      flag_ghost = 0;
    }
  }else if( state_ghost == 7 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 12){
      state_ghost = 11;
      flag_ghost = 0;
    }
  }else if( state_ghost == 7 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 282){
      state_ghost = 3;
      flag_ghost = 0;
    }
  }

  //STATE_GHOST 8
    if( state_ghost == 8 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 52){
      state_ghost = 13;
      flag_ghost = 0;
      }
  }else if( state_ghost == 8 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 52){
      state_ghost = 14;
      flag_ghost = 0;
    }
  }else if( state_ghost == 8 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 222){
      state_ghost = 5;
      flag_ghost = 0;
    }
  }

  //STATE_GHOST 9
  if (state_ghost == 9 && flag_ghost == 1){
    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 112){
      state_ghost = 15;
      flag_ghost = 0;
    }
  }else if( state_ghost == 9 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 182){
      state_ghost = 6;
      flag_ghost = 0;
    }
  }

  //STATE_GHOST 10
  if (state_ghost == 10 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 52){
      state_ghost = 13;
      flag_ghost = 0;
    }
  }else if( state_ghost == 10 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 162){
      state_ghost = 6;
      flag_ghost = 0;
    }
       
  }else if( state_ghost == 10 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 92){
      state_ghost = 15;
      flag_ghost = 0;
    }
  }

  //STATE_GHOST 11
  if (state_ghost == 11 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 12){
      state_ghost = 16;
      flag_ghost = 0;
    }
  }else if( state_ghost == 11 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 52){
      state_ghost = 7;
      flag_ghost = 0;
    }
  }
  //STATE_GHOST 12
  if (state_ghost == 12 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 12){
      state_ghost = 14;
      flag_ghost = 0;
    }
  }else if( state_ghost == 12 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 202){
      state_ghost = 7;
      flag_ghost = 0;
      }
  }else if( state_ghost == 12 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 92){
      state_ghost = 15;
      flag_ghost = 0;
    }
  }

  //STATE_GHOST 13
  if (state_ghost == 13 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 12){
      state_ghost = 8;
      flag_ghost = 0;
    }
  }else if( state_ghost == 13 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 112){
      state_ghost = 10;
      flag_ghost = 0;
    }
       
  }else if( state_ghost == 13 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 92){
      state_ghost = 17;
      flag_ghost = 0;
    }
  }
  
  //STATE_GHOST 14
  if( state_ghost == 14 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 112){
      state_ghost = 12;
      flag_ghost = 0;
      }
  }else if( state_ghost == 14 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 12){
      state_ghost = 16;
      flag_ghost = 0;
    }
  }else if( state_ghost == 14 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 182){
      state_ghost = 8;
      flag_ghost = 0;
    }
  }

  //STATE_GHOST 15
  if (state_ghost == 15 && flag_ghost == 1){

    index1 = (ghosty/11)%4;
    ghosty --;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty + 26, 26, 0x00);
    
    if(ghosty == 52){
      state_ghost = 17;
      flag_ghost = 0;
    }
  }else if( state_ghost == 15 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 162){
      state_ghost = 9;
      flag_ghost = 0;
      }
  }else if( state_ghost == 15 && flag_ghost == 3){
    index1 = (ghostx/11)%4;
    ghostx--;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx+26, ghosty, 26, 0x00);
    
    if(ghostx == 52){
      state_ghost = 12;
      flag_ghost = 0;
    }
  }else if( state_ghost == 15 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 182){
      state_ghost = 10;
      flag_ghost = 0;
    }
  }

  //STAGE_GHOST 16
  if( state_ghost == 16 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 202){
      state_ghost = 11;
      flag_ghost = 0;
  }
  }else if( state_ghost == 16 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 52){
      state_ghost = 14;
      flag_ghost = 0;
    }
  }

  //STAGE_GHOST 17
  if( state_ghost == 17 && flag_ghost == 2){
    index1 = (ghosty/11)%4;
    ghosty++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    H_line( ghostx, ghosty - 1, 26, 0x00);
    
    if(ghosty == 112){
      state_ghost = 15;
      flag_ghost = 0;
  }
  }else if( state_ghost == 17 && flag_ghost == 4){
    index1 = (ghostx/11)%4;
    ghostx++;
    LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);
    V_line( ghostx-1, ghosty, 26, 0x00);
    
    if(ghostx == 182){
      state_ghost = 13;
      flag_ghost = 0;
    }
  }
  delay(11);
 
}



//--------------------------------------------Funciones--------------------------------------------------------


void initialState(){

      index1 = 1%3;
     
      //LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
      LCD_Sprite(pacmanx,pacmany,26,26,sprite_pacman,3,index1,0,0);
      LCD_Sprite(ghostx,ghosty,26,26,sprite_ghost,4,index1,0,0);

      //flag_ghost = 3;
      //flag_pacman = 4;

      //state_pacman = 1;
      //state_pacman == 1 && flag_pacman == 3

      pinMode(PC_6, INPUT_PULLUP);
      pinMode(PC_7, INPUT_PULLUP);
      pinMode(PD_6, INPUT_PULLUP);
      pinMode(PD_7, INPUT_PULLUP);
  
     attachInterrupt(digitalPinToInterrupt(PC_6), toUp, RISING);
     attachInterrupt(digitalPinToInterrupt(PC_7), toDown, RISING);
     attachInterrupt(digitalPinToInterrupt(PD_6), toRight, RISING);
     attachInterrupt(digitalPinToInterrupt(PD_7), toLeft, RISING);

      pinMode(PF_0, INPUT_PULLUP);
      pinMode(PC_4, INPUT_PULLUP);
      pinMode(PF_3, INPUT_PULLUP);
      pinMode(PE_0, INPUT_PULLUP);
  
     attachInterrupt(digitalPinToInterrupt(PF_0), toUp2, RISING);
     attachInterrupt(digitalPinToInterrupt(PC_4), toDown2, RISING);
     attachInterrupt(digitalPinToInterrupt(PF_3), toRight2, RISING);
     attachInterrupt(digitalPinToInterrupt(PE_0), toLeft2, RISING);
     
  }


  void toUp(){
      flag_pacman = 1;
    }

  void toDown(){
      flag_pacman = 2;
    }
    
  void toRight(){
      flag_pacman =4;
    }

  void toLeft(){
      flag_pacman =3;
    }

  void toUp2(){
      flag_ghost = 1;
    }

  void toDown2(){
      flag_ghost = 2;
    }
    
  void toRight2(){
      flag_ghost =4;
    }

  void toLeft2(){
      flag_ghost =3;
    }

//para abrir la ruta y el archivo de la imagen para ponerla en la lcd
void uploadBackgroundSD(unsigned int x, unsigned int y, unsigned int width, unsigned int height, char * archivo){

  myFile = SD.open(archivo);
  uint16_t n = 0;
  uint16_t dimension = width*height*2;
  unsigned char fondo2[640] = {};//se le da el dimensionamiento al array según el archivo
  unsigned char caracter;
  unsigned char digito;
  
  if (myFile) {
    // read from the file until there's nothing else in it:
    
    while(myFile.available()) {
    //Serial.print("aqui toy");
      while(n<640){
        unsigned char value = 0;
        caracter = myFile.read();       
        if (caracter == 120){ //se lee y verifica el valor de inicio x
            for(uint8_t i = 0; i < 2; i++){
              caracter = myFile.read();
              digito = Char_to_hex(caracter);
              if (i == 0){ // se convierte a su valor decimal
                value = digito*16;
              }
              else if (i == 1){
                value = value + digito;
              }
            }
            fondo2[n] = value; //se le da el valor en su respectiva posición del arreglo 
            n ++;
        }else if(!myFile.available()){
           break;
          }    
      }

      n=0;
      y++;
      LCD_Bitmap(x,y,width,1,fondo2); // se dibuja la imagen en la lcd
      
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening ");
  }

  
}


// para convertir valor leido del txt en ascci al valor en decimal para formar el hexadecimal
unsigned char Char_to_hex(char value){
  unsigned char valor;
  if(value>=48 && value <=57){
    valor = value - 48;
  }
  else if (value >= 97 && value <=102){
    valor = value -87;
  }
  return valor;
}


//hasta aquí codigo escrito 

//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+w;
  y2 = y+h;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = w*h*2-1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c); 
      k = k - 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }
  
  char charInput ;
  int cLength = text.length();
  Serial.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
    for (int j = 0; j < height; j++){
        k = (j*(ancho) + index*width -1 - offset)*2;
        k = k+width*2;
       for (int i = 0; i < width; i++){
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k+1]);
        k = k - 2;
       } 
    }
  }
  else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
