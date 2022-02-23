#include <Stepper.h>

extern void ejecutar_server();
extern int mesa_giro;

const double pasos_por_revolucion = 200*16*4;
Stepper motor_mesa(pasos_por_revolucion, 15, 5);
    
// Clase con la cual controlar la mesa giratoria
class Mesa{
  private:
    // variables globales de la clase
    int velocidad_nominal;
    int pin_cero;
    double posicion_actual;

  public:
    //constructor
    Mesa(){};
    
    void inicio(int pin_A, int pin_B, int velocidad_nuevo, int pin_cero_nuevo){
      //creamos el objeto del motor pap y lo guardamos en la variable general
      Stepper motor_nuevo(pasos_por_revolucion, pin_A, pin_B);
      motor_mesa = motor_nuevo;
      
      //guardamos la posicion del pin del final de carrera que nos indica que hemos llegado al 0
      pin_cero = pin_cero_nuevo;
      //pinMode(pin_cero, INPUT);

      // cargamos el valor nominal de la velocidad en la variable global
      velocidad_nominal = velocidad_nuevo;

      //inicializamos su posicion en el 0;
      iniciar_cero();
      
    }

    void iniciar_cero(){
      
      motor_mesa.setSpeed(10);
      
      while(!digitalRead(pin_cero))
      {
        motor_mesa.step(2);
      }
      delay(100);
      
      motor_mesa.step(-pasos_por_revolucion/8-400);
      motor_mesa.step(pasos_por_revolucion/8);
      
      posicion_actual = 0;
    }
    
    void velocidad(int rpm){
      mesa_giro = 1;
      ejecutar_server();
     
      posicion(0);

      motor_mesa.setSpeed(rpm);
      motor_mesa.step(-1*270.0/360.0*pasos_por_revolucion);
      delay(500);
      
      motor_mesa.setSpeed(rpm);
      motor_mesa.step(270.0/360.0*pasos_por_revolucion);
      delay(500);
    }
    
    void posicion(double posicion_objetivo_grados){
      mesa_giro = 1;
      ejecutar_server();
      
      motor_mesa.setSpeed(velocidad_nominal);
      
      int pasos = (posicion_objetivo_grados - posicion_actual)/360.0*pasos_por_revolucion;
      
      motor_mesa.step(pasos*-1);
      
      posicion_actual = posicion_objetivo_grados;

      parar();
    }

    void parar(){
      //motor_mesa.setSpeed(0);
      mesa_giro = 0;
      ejecutar_server();
    }

};
