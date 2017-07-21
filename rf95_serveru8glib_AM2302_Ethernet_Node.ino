// rf95_server.pde
// -*- mode: C++ -*-

#include <Ethernet.h>
#include <SPI.h>
#include <RH_RF95.h>
#include "U8glib.h"
//definimos red ethernet
// Definimos la MAC Address de nuestro Ethernet Shield
byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x98, 0x91};

// Definimos la Dirección IP del Servidor Node.js
byte server[] = {169,254 , 191, 217};

// Definimos la Dirección IP del Arduino
byte ip[] = {169, 254, 191, 88};

// Instanciamos el cliente Ethernet con el que nos conectaremos al servidor
EthernetClient client;
// Definimos la variable donde almacenaremos la respuesta del servidor.
String response = String(10);
// Definimos el separador del mensaje
char separator = '|';
// Utilizaremos esta variable para verificar si la lectura de la respuesta ha comenzado
boolean started = false;
int i=0;
// Singleton instance of the radio driver
//RH_RF95 rf95;
// Change this constructor to match your display!!!
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);  // Fast I2C / TWI 

RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95 

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
//#define Serial SerialUSB

int led = 8;
int sdcard=4;
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
void setup() 
{
    Ethernet.begin(mac, ip); // Iniciamos el Ethernet Shield
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
u8g.setFont(u8g_font_helvB14);
  u8g.setColorIndex(1); // Instructs the display to draw with a pixel on. 

  pinMode(led, OUTPUT);     
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");  
  Serial.println("init success"); 
 
}

void loop()
{
  u8g.firstPage();
 /* if (rf95.available())
  {
    // Should be a message for us now   
 //   uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      digitalWrite(led, HIGH);
//      RH_RF95::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rf95.lastRssi(), DEC);
      
      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      i=1;
       digitalWrite(led, LOW);
    }
    else
    {
      //El paquete no es para el server
      Serial.println("recv failed");
      i=2;
    }
  }*/
//////Mensajes display
  do {  
       if(i==0){
         u8g.drawStr( 0, 30, "Falla...");
       }
       if(i==1){
          u8g.drawStr( 0, 30, "Rx ok, Rply");
       }
       if(i==2){
          u8g.drawStr( 0, 30, "No Rx. cliente?");
       }

  } while( u8g.nextPage() );
///Fin mensajes display
//var mensaje=(char*)buf;
String mensaje="100";
//Red ethernet
  // Realizamos la conexión al servidor Node.js en el puerto 8080
  if (client.connect(server, 8080)) {
    // Enviamos la peticion GET utilizando el protocolo HTTP
  //  client.println("GET /temp?value=" + String((char*)buf) + " HTTP/1.0");
  client.println("GET /temp?value=" + mensaje + " HTTP/1.0");
    client.println();

    // A continuación realizaremos la lectura de la respuesta desde el servidor
    // En este caso solo vamos a hacer lectura mientras estemos conectados
    // Luego de leer la respuesta completa desconectamos el cliente
    while (client.connected()) {
      // Solo realizaremos la lectura de la respuesta si el cliente tiene
      // algo para nosotros
      if (client.available()) {
        // Debemos leer caracter por caracter
        char c = client.read();

        // En el siguiente bloque de código utilizaremos el separador `|` para
        // verificar cuando empieza y termina el mensaje que necesitamos
        if (c != separator && started) {
          response += c;
        } else if (c == separator && !started) {
          started = true;
        } else if (c == separator && started) {
          started = false;
          client.stop();
        }
      }
    }

    // Luego de recibir la respuesta del servidor podemos decidir cual LED encender
    if (response == "Normal") {
      digitalWrite(led, LOW);
    } else if (response == "Advertencia") {
      digitalWrite(led, HIGH);
    }

    response = ""; // Limpiamos la variable para la siguiente petición
  } else {
    Serial.print("connection failed");
  }
}


