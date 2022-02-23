#ifndef Menu_h
#define Menu_h


/* Conexiones Display
LCD       ESP32
Vcc       3.3 v
GND       GND
SDA        GPIO21
SCL       GPIO22
*/
/*Conector encoder rotativo. Funciones disponibles
rotaryEncoder.currentButtonState() != BUT_RELEASED
rotaryEncoder.readEncoder();
rotaryEncoder.setBoundaries(limInf,limSup, false);
rotaryEncoder.encoderChanged()*/
#define ROTARY_ENCODER_A_PIN 35
#define ROTARY_ENCODER_B_PIN 34
#define ROTARY_ENCODER_BUTTON_PIN 32  
#define ROTARY_ENCODER_VCC_PIN -1 /*put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder antes 27 */

#define DIRLCD 0x3F  // Dirección i2c del lcd


int16_t leeEncoder();
int8_t deltaEncoder();  // calcula el sentido giro encoder
void incializaRotaryEncoder();
// Incialización del lcd

void inicializaLcd();

void escribeLcd(String mensaje1, String mensaje2);

int leeEstadoRotativo();

bool botonEncoderPulsado();

// Función principal de menu, devuelve el numero de opccion elegida
int miMenu(String menu[],int maxMenuItems,   String OpDefecto[], int nMenuOpDef);
void muestraMenu(String menu[], int maxMenuItems,  String opDefecto[], int opcionMenu);
double dameValor(String cadena, double valor, double inc, double min, double max);

void FuncionMenuPrincipal();
void FuncionMenu1();



#endif
