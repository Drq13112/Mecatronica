<#ifndef Eyector_h
#define Eyector_h

class Eyector{
  private:
    int pinEyector;
    int grados_inclinado;
    int grados_plano;

  public:
    Eyector();
  
    void inicio(int,int,int);

    int grados_actual;
    
    void posicion(int);
    
};
#endif>
