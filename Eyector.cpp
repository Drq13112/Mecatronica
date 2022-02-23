#include "Librerias.h"

#define COUNT_LOW 0
#define COUNT_HIGH 8888
#define TIMER_WIDTH 16

extern void ejecutar_server();
extern int mvmto_eyector;

Servo myservo;

Eyector::Eyector(){};

void Eyector::inicio(int pin_nuevo, int grados_inclinado_nuevo, int grados_plano_nuevo)
{
      pinEyector = pin_nuevo;
      grados_inclinado = grados_inclinado_nuevo;
      grados_plano = grados_plano_nuevo;
      myservo.attach(pinEyector);
      /*
      ledcSetup(1, 50, TIMER_WIDTH); // channel 1, 50 Hz, 16-bit width
      ledcAttachPIN(SERVO_PIN,1);   // GPIO 22 assigned to channel 
      */
}

    
    
void Eyector::posicion(int posEyector)
{
  mvmto_eyector = 1;
  ejecutar_server();


  for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (int pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  
  /*
  if(posEyector == 1) {
    
    for (int i=COUNT_LOW ; i < COUNT_HIGH ; i=i+100)
   {
      ledcWrite(1, i);       // sweep servo 1
      delay(50);
   }

    
  }else{

    for (int i=COUNT_HIGH ; i > COUNT_LOW ; i=i-100)
   {
      ledcWrite(1, i);       // sweep servo 1
      delay(50);
   }
  }
*/
  
  
  delay(1500);
  Motor(0);

  mvmto_eyector = 0;
  ejecutar_server();

}
