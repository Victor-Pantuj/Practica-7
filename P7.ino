#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Keypad.h>
#include <RTClib.h>

const int servoPin = 10;
const int numRows = 4;
const int numCols = 4;
const byte rowPins[numRows] = {6, 7, 8, 9};
const byte colPins[numCols] = {2, 3, 4, 5};
char keypadKeys[numRows][numCols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};
const int lcdColumns = 16;
const int lcdRows = 2;
const int lcdAddr = 0x27;
RTC_DS1307 rtc;

Servo servo;
LiquidCrystal_I2C lcd(lcdAddr, lcdColumns, lcdRows);
Keypad keypad = Keypad(makeKeymap(keypadKeys), rowPins, colPins, numRows, numCols);
int pin[6] = {31416};
int pinIndex = 0;

void mostrarMensaje(String mensaje) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mensaje);
  lcd.setCursor(0, 1);
  lcd.print(rtc.now().toString("YYYY/MM/DD hh:mm:ss"));
}

void cerrarPuerta() {
  servo.write(0);
  mostrarMensaje("Puerta cerrada");
}

void abrirPuerta() {
  servo.write(90);
  mostrarMensaje("Contraseña correcta, Puerta abierta");
}

void verificarPin() {
  if (pinIndex == 4) {
    if (keypad.getState() == PRESSED && keypad.isPressed('#')) {
      int ingresado[6];
      for (int i = 0; i < 4; i++) {
        ingresado[i] = keypad.waitForKey() - '0';
        lcd.print(ingresado[i]);
      }
      bool esCorrecto = true;
      for (int i = 0; i < 4; i++) {
        if (ingresado[i] != pin[i]) {
          esCorrecto = false;
        }
      }
      if (esCorrecto) {
        abrirPuerta();
      } else {
        mostrarMensaje("Contraseña incorrecta");
      }
      delay(2000);
      mostrarMensaje("Puerta cerrada");
      pinIndex = 0;
    }
  }
}

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();
  servo.attach(servoPin);
  rtc.begin();
  mostrarMensaje("Iniciando");

  if (!rtc.isrunning()) {
    mostrarMensaje("Error: RTC no encontrado");
    while (true);
  }

cerrarPuerta();
}

void loop() {
char key = keypad.getKey();
if (key != NO_KEY) {
lcd.print(key);
if (key >= '0' && key <= '9') {
  pin[pinIndex] = key - '0';
  pinIndex++;
  verificarPin();
} else {
  pinIndex = 0;
  mostrarMensaje("");
}
}
}