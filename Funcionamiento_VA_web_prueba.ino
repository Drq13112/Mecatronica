
#include "Librerias.h"
#include "Mesa.h"
#include "Eyector.h"
#include "ConexionSerial.h"
#include "Server.h"

extern int TomarFoto();




/* Conexiones LCD
  LCD       ESP32
  Vcc       3.3 v
  GND       GND
  SDA        GPIO21
  SCL       GPIO22
*/
/*
  // set the LCD number of columns and rows
  int lcdColumns = 16;
  int lcdRows = 2;

  // set LCD address, number of columns and rows
  // if you don't know your display address, run an I2C scanner sketch
  LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);
*/

/*


  loop()
  myCinta.controlVelocidad(800); //unidades random 0-1050
  myCinta.Parar();

  myCinta.controlPosicion(pasos_objetivo) ;

 
 */

int automatico = 0;   //Variable donde almacenar el estado del automático. 0 -> Manual, 1 -> Activado NO validado, 2 -> Activado validado.
int pin_boton_aux = 2;
int pin_uv = 2;//12;

bool realizado = false;  // variable con la cual saber si ya se ha realizado la acción requerida manualmente
bool bandera_aux = false;

int objeto_control_indice;
String objeto_control_nombre[3] = {"CT", "MG", "EY"};                  // Variable para saber el nombre del objeto que estamos moviendo

         //Variables para manejar el valor máximo, minimo y el paso de cada consigna de cada objeto
double objeto_control_vel_max[3] = {1050, 50, 1};
double objeto_control_vel_min[3] = {-1050, 0, 1};
double objeto_control_vel_paso[3] = {15, 1, 1};

double objeto_control_pos_max[3] = {10000, 270, 1};
double objeto_control_pos_min[3] = {-10000, 0, 0};
double objeto_control_pos_paso[3] = {5, 5, 1};

        // Lo mismo de ante spero cambiando algunos datos para que se puedan escribir todos por serial
double objeto_control_vel_max_s[3] = {1050, 50, 1};
double objeto_control_vel_min_s[3] = {-1050, 1, 1};

double objeto_control_pos_max_s[3] = {10000, 270, 2};
double objeto_control_pos_min_s[3] = {-10000, 1, 1};


int cinta_giro = 0; 
int mesa_giro = 0;
int mvmto_eyector = 0;


int consigna_indice = 0;
double consigna_valor[2];
String consigna_nombre[2] = {"POS", "VEL"};

int pin_eyector = 18;
int grados_eyect_plano = 80; //la inclinación hay que sumarle 70 grados  -  80 / 150
int grados_eyect_arriba = 150;

//declaramos el objeto eyector
Eyector eyector;


Cinta cinta; //declaramos el objeto cinta
system_t sys;

//declaramos el objeto mesa
Mesa mesa;


void setup() {
  Serial.begin(115200);

  iniciar_server();
  
  pinMode(pin_boton_aux, INPUT);
  pinMode(pin_uv, INPUT);

  inicializaLcd();                  // Incialización del display
  incializaRotaryEncoder();         //Incialización encoder rotativo HW-040


  Serial.println("antes d la cinta");

  // inicializamos la cinta
  settingsLoadFromEEprom();
  setupMovement();
  
  eyector.inicio(pin_eyector, grados_eyect_arriba, grados_eyect_plano); 
  eyector.posicion(1);
  eyector.posicion(0); 


  cinta.SetupCinta(175); 

  Serial.println("despues d la cinta");

  //Iniciamos el eyector
    
  //Inicializamos la mesa
  mesa.inicio(15, 5, 30, 14);

  
}




int contador = 0;
int tiempoMenu = 500;
int max_contador = tiempoMenu / 50;



void loop() {
  
  ejecutar_server();       

  if (automatico == 1 && digitalRead(pin_boton_aux)) automatico = 2;




  if (botonEncoderPulsado()) {
    FuncionMenuPrincipal();
  }


  switch (automatico) {
    case 0:                         // Estado manual
      if (contador > max_contador || contador == 0) {
        escribeLcd("M.MANUAL " + objeto_control_nombre[objeto_control_indice] , "CNSG " + consigna_nombre[consigna_indice] + ": " + String(consigna_valor[consigna_indice]));
        contador = 1;
      }
      delay(50);
      contador ++;

      //Se hace lo que indique el movimento manual si se indica consigna
      if(!realizado)
      {
        realizado = true;

        
        switch(objeto_control_indice){
          case 0:   //Cinta
              
            if(!consigna_indice) //posicion
              {
                Serial.print("Cinta moviendose a posicion: ");
                Serial.println(consigna_valor[consigna_indice]);

                cinta.controlPosicion(consigna_valor[consigna_indice]);

                Serial.println("Alcanzado");
              }
            else                  //velocidad
              {
                Serial.print("Cinta moviendose a velocidad: ");
                Serial.println(consigna_valor[consigna_indice]);

                

                
                while(1)
                { 
                  cinta.controlVelocidad(consigna_valor[consigna_indice]); //unidades random 0-1050 
                  if (digitalRead(pin_boton_aux))
                  {
                    cinta.Parar();
                    break;
                  }
                }
                

                Serial.println("Parado");
                
              }
            break;
  
          case 1:   //Mesa
            if(!consigna_indice) //posicion
              {
                Serial.print("Mesa moviendose a posicion: ");
                Serial.println(consigna_valor[consigna_indice]);

                mesa.posicion(consigna_valor[consigna_indice]);
                
                Serial.println("Alcanzado ");
              }
            else                  //velocidad
              {
                Serial.print("Mesa moviendose a velocidad: ");
                Serial.println(consigna_valor[consigna_indice]);

                while(!digitalRead(pin_boton_aux))
                {
                 mesa.velocidad(consigna_valor[consigna_indice]); 
                }

                Serial.println("Parado");
                
              }
            break;
  
          case 2:   //Eyector
          
            Serial.print("Eyector moviendose a poisicion: ");
            Serial.println(consigna_valor[consigna_indice]);

            eyector.posicion(consigna_valor[consigna_indice]);
            
            Serial.println("alcanzado");
                
            break;
        }

        muestra_menu();
      }else menu_serial(Serial.read());
      
      
      break;
      
    case 1:                         // Estado auto no validado
      escribeLcd("M. AUTOMATICO   ", "NO VALIDADO    ");
      break;
      
   case 2:                          // Estado auto validado
      escribeLcd("M. AUTO VALIDADO", "ESPERANDO PIEZA "); 

      if(digitalRead(pin_uv))
      { 
        cinta.controlPosicion(100);
        escribeLcd("M. AUTO VALIDADO", "TOMANDO FOTO   ");
        int posicion = TomarFoto();
        delay(5000);

        if(posicion == 0){
          escribeLcd("M. AUTO VALIDADO", "PIEZA MAL   ");
          cinta.controlPosicion(-100);
          delay(2000);
        }
        else{
          escribeLcd("M. AUTO VALIDADO", "PIEZA TIPO " + String(posicion) + "    ");
          cinta.controlPosicion(400);
          delay(2000);
          mesa.posicion(90*posicion);
          delay(2000);
          eyector.posicion(1);
          escribeLcd("M. AUTO VALIDADO", "PIEZA DEPOSITADA");
          eyector.posicion(0);
          mesa.posicion(0);          
        }
        
      }
      
      break;
  }
}



/*Creamos una función que saca por pantalla
 * las opciones del menu, ya que la usamos 
 * en un par de sitios
 */
void muestra_menu()
{
  Serial.println();
  Serial.println("Bienvenido al menu por Serial, ");
  Serial.println(" a -> pasar a automatico");
  Serial.println(" o -> cambiar objeto de control");
  Serial.println(" p -> establecer consigna de posicion");
  if(objeto_control_indice != 2) Serial.println(" v -> establecer consigna de velocidad");
  Serial.println("Puedes escribir varios comandos seguidos");

}



/* Craemos una fucnión que nos genere el menú
 *  del serial para limpiar el codigo usado en el loop
 */
void menu_serial(char lectura)
{
  //creamos una variable auxiliar para leer numeros de serial
  int lectura_n;
  int lectura_int;
  
   switch(lectura)
  {
    //pasamos al automatico
    case 'a':
      Serial.println("\nModo automático NO validado");
      Serial.println("Antes de validar el automático, cierre este serial \ne inicie el programa de Vision Artificial.");
      Serial.println("Para volver a este modo, cierre el programa de VA \ny salga del modo automatico");
      automatico = 1;
    break;
    
    //cambiamos fuerza de consigna
    case 'o':
      
      Serial.println("\nControlando el objeto " + objeto_control_nombre[objeto_control_indice]);
      Serial.println("   1 -> Volver");
      Serial.println("   2 -> Cambiar a Cinta");
      Serial.println("   3 -> Cambiar a Mesa Giratoria");
      Serial.println("   4 -> Cambiar a Eyector");
      Serial.print("Eliga una opción: ");
      do
      {
        lectura_int= Serial.parseInt();
      } 
      while(lectura_int <=0 || lectura_int >4 );
      Serial.println(lectura_int);
      if(lectura_int != 1){
        objeto_control_indice = lectura_int - 2;
        consigna_valor[0] = 0;
        consigna_valor[1] = 0;
      }

      muestra_menu();
      
    break;



    //cambiamos posicion de consigna
    case 'p':
      realizado = false;
      
      consigna_indice = 0;
      
      Serial.println("\nActual consigna de posicion para " + objeto_control_nombre[objeto_control_indice] + ": " + String(consigna_valor[0]));
      Serial.print("Eliga un valor entre " + String(objeto_control_pos_min_s[objeto_control_indice]) + " y " + String(objeto_control_pos_max_s[objeto_control_indice]) + ": ");
      
      do
      {
        lectura_n = Serial.parseInt();
      }
      while(lectura_n == 0 );
      Serial.println(lectura_n);

      if(objeto_control_indice == 2) consigna_valor[0] = lectura_n-1;
      else consigna_valor[0] = lectura_n;
      
    break;

    //cambiamos velocidad de consigna
    case 'v':
      realizado = false;
      
      consigna_indice = 1;
      
      Serial.println("\nActual consigna de velocidad para " + objeto_control_nombre[objeto_control_indice] + ": " + String(consigna_valor[1]));
      Serial.print("Eliga un valor entre " + String(objeto_control_vel_min_s[objeto_control_indice]) + " y " + String(objeto_control_vel_max_s[objeto_control_indice]) + ": ");
      
      do
      {
        lectura_n= Serial.parseFloat();
      }
      while(lectura_n == 0);
      Serial.println(lectura_n);
  
      consigna_valor[1] = lectura_n;
      
    break;

    //ignoramos
    default:
    break;
  }
}
