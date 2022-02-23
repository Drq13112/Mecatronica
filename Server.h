// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Wifi";
const char* password = "12345678";


// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
String valueString = String(0);

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


WiFiClient client;   // Listen for incoming clients

void iniciar_server()
{
  int contador = 0;
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    contador ++;
    delay(500);
    Serial.print(".");
    if (contador % 6 == 0)WiFi.begin(ssid, password);
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}


extern bool realizado;

extern int cinta_giro; 
extern int mesa_giro;
extern int mvmto_eyector;

extern int automatico;
extern int objeto_control_indice;
extern int consigna_indice;
extern double consigna_valor[];
extern String consigna_nombre[];

extern double objeto_control_vel_max[];
extern double objeto_control_vel_min[];

extern double objeto_control_pos_max[];
extern double objeto_control_pos_min[];

double maximo[3];
double minimo[3];

void ejecutar_server()
{
  //Serial.println("Servidor ejecutandose");
  client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    //Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();


            // Hace los cambios oportunos según en la página en la que se esté
            if (header.indexOf("GET /man") >= 0) {
              automatico = 0;
              realizado = false;
            } else if (header.indexOf("GET /auto_val") >= 0) {
              automatico = 2;
              realizado = false;
            } else if (header.indexOf("GET /auto") >= 0) {
              automatico = 1;
              realizado = false;
            } else if (header.indexOf("GET /cinta") >= 0) {
              objeto_control_indice = 0;
            } else if (header.indexOf("GET /mesa") >= 0) {
              objeto_control_indice = 1;
            } else if (header.indexOf("GET /eyec") >= 0) {
              objeto_control_indice = 2;
            } else if (header.indexOf("GET /pos") >= 0) {
              consigna_indice = 0;
              for (int i = 0; i < 3; i++)
              {
                maximo[i] = objeto_control_pos_max[i];
                minimo[i] = objeto_control_pos_min[i];
              }
            } else if (header.indexOf("GET /vel") >= 0) {
              consigna_indice = 1;
              for (int i = 0; i < 3; i++)
              {
                maximo[i] = objeto_control_vel_max[i];
                minimo[i] = objeto_control_vel_min[i];
              }
            } else if (header.indexOf("GET /?value=") >= 0) {
              int pos1 = header.indexOf('=');
              int pos2 = header.indexOf('&');
              valueString = header.substring(pos1 + 1, pos2);
              consigna_valor[consigna_indice] = valueString.toInt();
              realizado = false;
            }


            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>");
            client.println("html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".activo {background-color: #4CAF50; border: none; color: white; padding: 25px 20px;}");
            client.println(".activo_peque {background-color: #4CAF50; border: none; color: white; padding: 15px 10px;}");
            client.println(".apagado {background-color: #555555; padding: 25px 20px;}");
            client.println(".apagado_peque {background-color: #555555; padding: 15px 10px;}");
            client.println(".movimiento {background-color: #DB4035; padding: 25px 20px;}");

            client.println("text-decoration: none; font-size: 15px; margin: 2px; cursor: pointer;}");

            client.println(".slider { width: 300px; color: red}</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");

            client.println("</style></head>");

            // Empieza el cuerpo
            client.println("<body>");

            client.println("<h1>");
            client.println("<img src=\"https://eii.uva.es/escuela/logos/peq/eiibn.jpg\" style=\"vertical-align:middle\" width=\"100\" height=\"100\">");
            client.println("<a href = \"/\" >Control WEB proyecto</a>");
            client.println("<img src=\"data:image/png;base64,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\" style=\"vertical-align:middle\" width=\"75\" height=\"75\">");
            client.println("</h1>");

            client.println("<p> Angel Padilla y David Redondo </p>");

            if (!automatico)
            { //manual
              client.println("<p><a href=\"/auto\"><button class=\"button activo\">Automatico</button></a></p>");
              client.println("<h2>Modo Manual</h2>");
            }
            else if (automatico == 1) { //auto no validado
              client.println("<p><a href=\"/auto_val\"><button class=\"button activo\">Validar</button></a>");
              client.println("<a href=\"/man\"><button class=\"button apagado\">Manual</button></a></p>");
              client.println("<h2>Automatico NO validado</h2>");
            }
            else { //auto validado
              client.println("<p><a href=\"/man\"><button class=\"button apagado\">Manual</button></a></p>");
              client.println("<h2>Automatico VALIDADO</h2>");
            }


            if (!consigna_indice && !automatico) { //elegida consigna de posicion
              client.println("<p><a href=\"/pos\"><button class=\"button activo_peque\">Posicion</button></a>");
              client.println("<a href=\"/vel\"><button class=\"button apagado_peque\">Velocidad</button></a></p>");
            }
            else if(!automatico)   //elegida consigna de velocidad
            {
              client.println("<p><a href=\"/pos\"><button class=\"button apagado_peque\">Posicion</button></a>");
              client.println("<a href=\"/vel\"><button class=\"button activo_peque\">Velocidad</button></a></p>");
            }


            if (!automatico)
            {
              switch (objeto_control_indice)
              {
                case 0:


                  client.println("<p><a  href=\"/cinta\"><button class=\"button activo\">Cinta</button></a>");

                  if (!consigna_indice)client.println("Posicion: <span id=\"servoPos\"></span>");
                  else client.println("Velocidad: <span id=\"servoPos\"></span>");
                  client.println("<input type=\"range\" min=\"" + String(minimo[objeto_control_indice]) + "\" max=\"" + String(maximo[objeto_control_indice]) + "\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\"" + valueString + "\"/>");

                  client.println("<script>var slider = document.getElementById(\"servoSlider\");");
                  client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
                  client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
                  client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
                  client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script></p>");



                  client.println("<p><a href=\"/mesa\"><button class=\"button apagado\">Mesa</button></a></p>");
                  if (!consigna_indice)client.println("<p><a href=\"/eyec\"><button  class=\"button apagado\">Eyector</button></a></p>");
                  break;

                case 1:
                  client.println("<p><a  href=\"/cinta\"><button class=\"button apagado\">Cinta</button></a></p>");



                  client.println("<p><a href=\"/mesa\"><button class=\"button activo\">Mesa</button></a>");

                  if (!consigna_indice)client.println("Posicion: <span id=\"servoPos\"></span>");
                  else client.println("Velocidad: <span id=\"servoPos\"></span>");
                  client.println("<input type=\"range\" min=\"" + String(minimo[objeto_control_indice]) + "\" max=\"" + String(maximo[objeto_control_indice]) + "\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\"" + valueString + "\"/>");

                  client.println("<script>var slider = document.getElementById(\"servoSlider\");");
                  client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
                  client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
                  client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
                  client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script></p>");



                  if (!consigna_indice)client.println("<p><a href=\"/eyec\"><button  class=\"button apagado\">Eyector</button></a></p>");
                  break;

                case 2:
                  client.println("<p><a  href=\"/cinta\"><button class=\"button apagado\">Cinta</button></a></p>");
                  client.println("<p><a href=\"/mesa\"><button class=\"button apagado\">Mesa</button></a></p>");



                  

                  if (!consigna_indice){
                    client.println("<p><a href=\"/eyec\"><button  class=\"button activo\">Eyector</button></a>");
                    
                    client.println("Posicion: <span id=\"servoPos\"></span>");
                    client.println("<input type=\"range\" min=\"" + String(minimo[objeto_control_indice]) + "\" max=\"" + String(maximo[objeto_control_indice]) + "\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\"" + valueString + "\"/>");
  
                    client.println("<script>var slider = document.getElementById(\"servoSlider\");");
                    client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
                    client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
                    client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
                    client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script></p>");
                  }
                  break;

              }
            }
            else {  //En automático
              if(!cinta_giro)client.println("<p><a  href=\"/cinta\"><button class=\"button apagado\">Cinta</button></a>");
              else {
                client.println("<p><a  href=\"/cinta\"><button class=\"button movimiento\">Cinta</button></a>");
                client.println("Consigna de " + consigna_nombre[consigna_indice] + " a " + String(consigna_valor[consigna_indice])); 
              }

              if(!mesa_giro)client.println("<p><a  href=\"/cinta\"><button class=\"button apagado\">Mesa</button></a>");
              else {
                client.println("<p><a  href=\"/cinta\"><button class=\"button movimiento\">Mesa</button></a>");
                client.println("Consigna de " + consigna_nombre[consigna_indice] + " a " + String(consigna_valor[consigna_indice])); 
              }
              
              if(!mvmto_eyector)client.println("<p><a  href=\"/cinta\"><button class=\"button apagado\">Eyector</button></a>");
              else {
                client.println("<p><a  href=\"/cinta\"><button class=\"button movimiento\">Eyector</button></a>");
                client.println("Consigna de " + consigna_nombre[consigna_indice] + " a " + String(consigna_valor[consigna_indice])); 
              }
            }




            //Se termina la web
            client.println("</body></html>");


//            // Hace los cambios oportunos según en la página en la que se esté
//            if (header.indexOf("GET /man") >= 0) {
//              automatico = 0;
//              realizado = false;
//            } else if (header.indexOf("GET /auto_val") >= 0) {
//              automatico = 2;
//              realizado = false;
//            } else if (header.indexOf("GET /auto") >= 0) {
//              automatico = 1;
//              realizado = false;
//            } else if (header.indexOf("GET /cinta") >= 0) {
//              objeto_control_indice = 0;
//            } else if (header.indexOf("GET /mesa") >= 0) {
//              objeto_control_indice = 1;
//            } else if (header.indexOf("GET /eyec") >= 0) {
//              objeto_control_indice = 2;
//            } else if (header.indexOf("GET /pos") >= 0) {
//              consigna_indice = 0;
//              for (int i = 0; i < 3; i++)
//              {
//                maximo[i] = objeto_control_pos_max[i];
//                minimo[i] = objeto_control_pos_min[i];
//              }
//            } else if (header.indexOf("GET /vel") >= 0) {
//              consigna_indice = 1;
//              for (int i = 0; i < 3; i++)
//              {
//                maximo[i] = objeto_control_vel_max[i];
//                minimo[i] = objeto_control_vel_min[i];
//              }
//            } else if (header.indexOf("GET /?value=") >= 0) {
//              int pos1 = header.indexOf('=');
//              int pos2 = header.indexOf('&');
//              valueString = header.substring(pos1 + 1, pos2);
//              consigna_valor[consigna_indice] = valueString.toInt();
//              realizado = false;
//            }



            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

    }
    }

        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        return;
        //Serial.println("Client disconnected.");
        //Serial.println("");
      
  }
}
