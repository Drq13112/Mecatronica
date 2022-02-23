
#include "Librerias.h"

extern int cinta_giro;
extern void ejecutar_server();

double Kp, Ki, Kd;
double Setpoint, Input, Output;
double velMotor;
int encoderA = 23;  
int encoderB = 19;

Encoder myEnc(encoderA, encoderB);
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
extern system_t sys;

Cinta::Cinta(){}

void Cinta::SetupCinta(double velocidad_nominal_nuevo){
  velocidad_nominal = velocidad_nominal_nuevo;
  setupMovement();
  
  myPID.SetSampleTime(100);

  myPID.SetOutputLimits(-255, 255);

  myPID.SetMode(AUTOMATIC);

  Setpoint=0;
  myEnc.write(0);

  controlPosicion(600);
  delay(500);
  controlPosicion(-600);
  delay(500);
}
void Cinta::controlVelocidad(double Velocidad)
{ 
  cinta_giro = 1;
  ejecutar_server();
  
  Setpoint=Velocidad;

  unsigned long int t, dt = 100;  // intervalo para el calculo de velocidad
  float tiempo;
  int8_t inc;
  double Pos;
  static double Pos_ant = myEnc.read();
  //float periodo;
  static unsigned long int tinicio = millis();
  static unsigned long int t_ant = millis();   // Temporizador para calculo velocidad
  static unsigned long int t_ant2 = millis();  // Temporizador para calculo Periodo

  // Calculo velocidad

t = millis();
  if (t - t_ant > dt) {
    Pos = myEnc.read();
    Input = (double)1000 * (Pos - Pos_ant) / (t - t_ant);  // Calculo la velocidad
    t_ant = t;
    Pos_ant = Pos;
    Serial.print("+SetPoint:" + String(Setpoint));
    Serial.print(", ");

    Serial.print("Input:" + String(Input));
    Serial.print(", ");
    Serial.println("Output:" + String(Output));
  }

  //Serial.println("Pos: "+String(myEnc.read()));

    inc = deltaEncoder();
    //Serial.println("inc:"+String(inc));

    //if (inc != 0) Setpoint = (Setpoint > 0 ? Setpoint + inc : Setpoint - inc);  // Puedo cambiara la referencia con el rotary encoder



  if (Output < 140 && Output > -140) {
    Kp = 3;  //3
    Ki = 0.05;  //0.05
    Kd = 0;  //0.0
  } else {
    Kp = 0.76;  //0.76
    Ki = 0.05;  //0.05
    Kd = 0.05;  //0.05
  }
  myPID.SetTunings(Kp, Ki,Kd);
  myPID.Compute();
  velMotor = Output;  //Cambiar signo si el motor gira solo en un sentido
  velMotor = map(Velocidad, 0, 1050, 0, 255);
  Motor(velMotor);

}

void Cinta::controlPosicion(double Posicion){
  double velocidadmin = velocidad_nominal*-1;
  double velocidadmax = velocidad_nominal;

  
  cinta_giro = 1;
  ejecutar_server();

  Setpoint=Posicion;
  myEnc.write(0);

  float tiempo;
  unsigned long int t, dt = 100;                // intervalo para la visualiación valores
  static unsigned long int tinicio = millis();  // Para calculo  funcion sinoidal
  static unsigned long int t_ant = millis();    // Temporizador para calculo velocidad
  static unsigned long int t_ant2 = millis();   // Temporizador para calculo Periodo
  //static int dir=1;

  myPID.SetOutputLimits(velocidadmin,velocidadmax);
  do{
  t = millis();
  
    //int8_t inc = deltaEncoder();
    //Serial.println("inc:"+String(inc));

    
    //Serial.println("Pos: "+String(inc));
    //Setpoint = (Setpoint > 0 ? Setpoint + inc : Setpoint - inc); 
    // Puedo cambiar la referencia con el rotary encoder


  //Setpoint = move(sys.setPoint, 1000, 800, 0.1);  //double xd, double vmax, double a, double dt);
  Input = myEnc.read();  // Lee encoder del motor
  
  if (Output < 140 && Output > -140) {
    Kp = sys.kPZMPos;  //3
    Ki = sys.kIZMPos;  //0.05
    Kd = sys.kDZMPos;  //0.0
  } else {
    Kp = sys.kPPos;  //0.76
    Ki = 0.3;  //0.05
    Kd = 0.1;  //0.05
  }
  myPID.SetTunings(Kp, Ki, Kd);
  myPID.Compute();


  if (t - t_ant > dt ) {  // Imprime valores cada periodo de muestreo
    Serial.print("+SetPoint:" + String(Setpoint));
    Serial.print(", ");
    
    Serial.print("Input:" + String(Input));
    Serial.print(", ");
    Serial.println("Output:" + String(Output));
    t_ant = t;
  }

  velMotor = Output;  //Cambiar signo si el motor gira solo en un sentido
  if (Posicion>0)
  {
      velMotor=200;
  }
  else
  {
    velMotor=-200;
  }  
  
  Motor(velMotor);
  }while(!(Input >= Posicion-30 && Input <= Posicion+30));
  Parar();

}

void Cinta::Parar()
{
  //Paramos motor
  Motor(0);
  
  cinta_giro = 0;
  ejecutar_server();
}
