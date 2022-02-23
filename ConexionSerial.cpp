
#include "ConexionSerial.h"

int TomarFoto()
{

int data;
String cad;
Serial.println(-1);

// -1  ---> Solicitar foto
// -2  ---> Ponemos un valor en el serial que no correspode a nada para que no haya mal entendidos

delay(20000);

cad=Serial.readString();
data=cad.toInt();

Serial.println(-2);
return data;
}
