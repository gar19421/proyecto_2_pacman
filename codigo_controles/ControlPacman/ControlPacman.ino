//************************************************************************** 
// Author: Brandon Garrido y Margareth Vela
// Carnet: 19421 / 19458
// Fecha de creación: 24/10/2021
// Ultima modificación: 24/10/2021
//**************************************************************************


//-------Proyecto 2 Electrónica Digital 2---------//
//--------------Videojuego - PACMAN--------------//

void setup() {
  // put your setup code here, to run once:

  pinMode(PC_6, INPUT_PULLUP); // Push ↑ up
  pinMode(PC_7, INPUT_PULLUP); // Push ↓ down
  pinMode(PD_6, INPUT_PULLUP); // Push  → right
  pinMode(PD_7, INPUT_PULLUP); // Push  ← left

  pinMode(PE_2, OUTPUT); // Salidas de tiva control a tiva principal
  pinMode(PE_3, OUTPUT);
  pinMode(PA_6, OUTPUT);
  pinMode(PA_7, OUTPUT);

  digitalWrite(PE_2, HIGH); // Estados iniciales de las salidas en pullup
  digitalWrite(PE_3, HIGH);
  digitalWrite(PA_6, HIGH);
  digitalWrite(PA_7, HIGH);
  
  attachInterrupt(digitalPinToInterrupt(PC_6), toUp, RISING);  //Rutinas de interrupción para los push
  attachInterrupt(digitalPinToInterrupt(PC_7), toDown, RISING);
  attachInterrupt(digitalPinToInterrupt(PD_6), toRight, RISING);
  attachInterrupt(digitalPinToInterrupt(PD_7), toLeft, RISING);
    
}

 void toUp(){ // Pulso instrucción de subir 
      digitalWrite(PE_2, LOW);
      delay(50);
      digitalWrite(PE_2, HIGH);
      
    }

  void toDown(){ // Pulso instrucción de bajar 
      digitalWrite(PA_7, LOW);
      delay(50);
      digitalWrite(PA_7, HIGH);
    }
    
  void toRight(){ // Pulso instrucción para girar derecha 
      digitalWrite(PA_6, LOW);
      delay(50);
      digitalWrite(PA_6, HIGH);
    }

  void toLeft(){ // Pulso instrucción para girar izquierda 
      digitalWrite(PE_3, LOW);
      delay(50);
      digitalWrite(PE_3, HIGH);
    }

void loop() {
  // put your main code here, to run repeatedly: 
  
}
