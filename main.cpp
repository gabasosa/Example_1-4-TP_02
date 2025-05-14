#include "mbed.h"
#include "arm_book_lib.h"

#define BUTTON_A_AND_B_PRESSED 0b0011
#define BUTTON_C_AND_D_PRESSED 0b1100

/* Versión original
int main()
{
    DigitalIn gasDetector(D2);
    DigitalIn overTempDetector(D3);
    DigitalIn aButton(D4);
    DigitalIn bButton(D5);
    DigitalIn cButton(D6);
    DigitalIn dButton(D7);

    DigitalOut alarmLed(LED1);

    gasDetector.mode(PullDown);
    overTempDetector.mode(PullDown);
    aButton.mode(PullDown);
    bButton.mode(PullDown);
    cButton.mode(PullDown);
    dButton.mode(PullDown);

    alarmLed = OFF;

    bool alarmState = OFF;

    while (true) {

        if ( gasDetector || overTempDetector ) {
            alarmState = ON;
        }

        alarmLed = alarmState;

        if ( aButton && bButton && !cButton && !dButton) {
            alarmState = OFF;
        }
    }
}
*/

/* Con Busin
int main()
{
    // Creo dos objetos BusIn para agrupar entradas relacionadas
    BusIn detectors(D2, D3);                 // gasDetector en bit 0 y overTempDetector bit 1
    BusIn buttons(D4, D5, D6, D7);           // aButton en bit 0 ... dButton en bit 3

    DigitalOut alarmLed(LED1);

    detectors.mode(PullDown);    // aplica a todos los pines en el BusIn
    buttons.mode(PullDown);

    alarmLed = OFF;

    bool alarmState = OFF;

    while (true) {
        // Reviso si gasDetector u overTempDetector están activados
        if ( detectors.read() != 0 ) {
            alarmState = ON;
        }

        alarmLed = alarmState;

        // Apago la alarma si A y B están presionados y C y D no presionados
        int buttonState = buttons.read();
        if ( (buttonState & BUTTON_A_AND_B_PRESSED) == BUTTON_A_AND_B_PRESSED &&
             (buttonState & BUTTON_C_AND_D_PRESSED) == 0x00 ) {
            alarmState = OFF;
        }

    }
} */


// Con PortIn y PortOut

// Máscaras para las entradas
#define GAS_MASK     0b1000000000000000  // PF_15 (D2)
#define A_MASK       0b0100000000000000  // PF_14 (D4)
#define D_MASK       0b0010000000000000  // PF_13 (D7)

#define TEMP_MASK    0b0010000000000000  // PE_13 (D3)
#define B_MASK       0b0000100000000000  // PE_11 (D5)
#define C_MASK       0b0000001000000000  // PE_9  (D6)

#define LED_MASK     0b00000001          // PB_0 (LED1)

// Puertos agrupados
PortIn inputsF(PortF, GAS_MASK | A_MASK | D_MASK);
PortIn inputsE(PortE, TEMP_MASK | B_MASK | C_MASK);
PortOut alarmLed(PortB, LED_MASK);

// Pull-downs
DigitalIn gasPin(PF_15);
DigitalIn aPin(PF_14);
DigitalIn dPin(PF_13);

DigitalIn tempPin(PE_13);
DigitalIn bPin(PE_11);
DigitalIn cPin(PE_9);

int main()
{
    // Activar pull-downs
    gasPin.mode(PullDown);
    aPin.mode(PullDown);
    dPin.mode(PullDown);
    tempPin.mode(PullDown);
    bPin.mode(PullDown);
    cPin.mode(PullDown);

    bool alarmState = OFF;

    while (true) {
        int pf = inputsF.read();
        int pe = inputsE.read();

        // Extraer bits individuales (sin shifts)
        int gas = (pf & GAS_MASK) ? 1 : 0;
        int a   = (pf & A_MASK)   ? 1 : 0;
        int d   = (pf & D_MASK)   ? 1 : 0;

        int temp = (pe & TEMP_MASK) ? 1 : 0;
        int b    = (pe & B_MASK)    ? 1 : 0;
        int c    = (pe & C_MASK)    ? 1 : 0;

        int buttonState = a | (b << 1) | (c << 2) | (d << 3);

        // Activar alarma si hay gas o sobretemperatura
        if (gas || temp) {
            alarmState = ON;
        }

        // Desactivar si A y B están presionados y C y D no
        if ((buttonState & BUTTON_A_AND_B_PRESSED) == BUTTON_A_AND_B_PRESSED &&
            (buttonState & BUTTON_C_AND_D_PRESSED) == 0) {
            alarmState = OFF;
        }

        // Reflejar estado de la alarma en el LED
        alarmLed = alarmState ? LED_MASK : 0;

        ThisThread::sleep_for(100ms);
    }
}
