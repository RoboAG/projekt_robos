//Diese Version ist nicht vollständig funktionsfähig. Für Kommentare siehe V2.4
//Ausschnitte bitte nur mit folgenden Kommentar verwenden

/*****************************************************************
* RobotikAG des Johannes-Kepler-Gymnasiums Chemnitz              *
*                                                                *
* Autoren:                                                       *
*     Janek Neugebauer                Leonhard Nagel             *
*     Felix Krüger                    Christoph Männel           *
*                                                                *
* Stand:                                                         *
*     Juni 2018                                                  *
*                                                                *
* Ausschnitte dieses Programms stammen aus dem Programm          *
* zum Endwettbewerb des RoboSAX 2018.                            *
*                                                                *
* Dokumentation und Software unter "2017-2018_RoboSAX" auf       *
*   https://github.com/RoboAG/projekt_roboter                    *
*****************************************************************/

#include "3pi_neu.h"
#include <Pololu3pi/Pololu3pi.h>

#define uart0_baudrate 57600
#define uart0_buffer_size 64
uint8_t uart0_buffer[uart0_buffer_size];
uint8_t uart0_buffer_position = 0;

unsigned int linesensors[5];

void uart0_init(void) {
  uart0_buffer_position = 0;
  serial_set_baud_rate(uart0_baudrate);
  serial_receive_ring((char*) uart0_buffer, uart0_buffer_size);
}

uint8_t uart0_receive(void) {
  while(serial_get_received_bytes() == uart0_buffer_position);

  uint8_t result = uart0_buffer[uart0_buffer_position];
  ++uart0_buffer_position;
  if(uart0_buffer_position >= uart0_buffer_size) {uart0_buffer_position = 0;}

  return result;
}

void uart0_send(uint8_t data) {
  serial_send_blocking((char *) &data, 1);
}

void send_int(int data) {
    if(data < 0) {
        uart0_send('-');
        send_int(-data);
        return;
    } else if(data >= 10) {
        send_int(data / 10);
    }
    uart0_send('0' + data % 10);
}

void send_string(char data[]) {
    string_from_ram(uart0_send, data);
}

int infrawerte[2];

void inframessen(void){
    set_analog_mode(MODE_8_BIT);
    infrawerte[1] = analog_read(6);
    infrawerte[0] = analog_read(7);
}

/* int infraauswerten(int marceldavis)
 * Diese Funktion wurde entfernt, weil sie total sinnlos war*/

void init_leds(void){
    PORTB &= ~_BV(2);
    DDRB  &= ~_BV(2);
}
int read_leds(void){
    return(PINB & _BV(2));
}

void motoransteuerung(void){
    inframessen();
    read_line_sensors(linesensors, IR_EMITTERS_ON);
    if(linesensors[4] > 1300 && infrawerte[0] > 80) {
        set_motors(20, 20); //langsamer
        delay(400); //warten bis LED und Sensor an gleicher Stelle
        if(!read_leds()) {
            send_string("\nLED aus, Knopf wird gedrückt.\n");
            green_led(0);
            delay(100);
            set_motors(50, -50); //rechts drehen
            delay(350);
            set_motors(60, 60); //rammen
            delay(800);
            set_motors(-30, -30); //zurück fahren
            delay(500);
            set_motors(-50, 50); //links drehen
            delay(350);
        } else {
            send_string("\nLED an, Knopf wird nicht gedrückt.\n");
            green_led(1);
        }
    }
        if(infrawerte[1] >= 70){
            set_motors(-50,50);
            delay(307);
            //send_string("drehen_l\n");
        }else{
            int steer = (infrawerte[0] - 100) * 0.40;
            if(steer > 60){
                steer = 60;
            }
            if(steer < -60){
                steer = -60;
            }
            set_motors(100-steer, 100+steer);
        }
}

int main(void){
    set_motors(0,0);
    uart0_init();
    init_leds();
    pololu_3pi_init(2000);
    delay(1000);
    inframessen();
    set_motors(120,120);
    delay(700);
    while(infrawerte[0] < 80 || infrawerte[1] < 80) {
        delay(10);
        inframessen();
        set_motors(100,100);
    }
    green_led(1);
    set_motors(-50,-50);
    delay(200);
    set_motors(-50,50);
    delay(370);
    while(1){
        motoransteuerung();
        green_led(read_leds());
        red_led(!read_leds());
        delay(3);
    }
    return 0;
}
