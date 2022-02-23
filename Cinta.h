#ifndef Cinta_h
#define Cinta_h

class Cinta {
  double velocidad_nominal;
  public:

    Cinta();
    void SetupCinta(double);

    //Se controla usando el Setpoint
    void controlVelocidad(double);
    void controlPosicion(double);
    void Parar();
};
#endif
