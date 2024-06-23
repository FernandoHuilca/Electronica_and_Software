#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
SoftwareSerial mySerial(10, 11);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Servo miServo;

String owners[] = {"Fernando", "FernandoH", "FernandoH", "FernandoH", "FernandoH", "FernandoH", "Maria"};
const int buttonPin = 9;
const int numOwners = sizeof(owners) / sizeof(owners[0]);
const int ledPin1 = 12;  // Pin del primer LED GREEN
const int ledPin2 = 13; // Pin del segundo LED RED

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(100);
  finger.begin(57600);
  
  if (!finger.verifyPassword()) {
    Serial.println("¡No se encontró el sensor de huellas :(");
    while (true) { delay(1); }
  }

  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  miServo.write(0); // Inicializa el servo en posición 0 grados
  Serial.println("Esperando un dedo válido o que se presione el botón...");
}

void loop() {
  int fingerID = getFingerprintIDez();
  int buttonState = digitalRead(buttonPin);
  static int lastButtonState = LOW; // Variable estática para mantener el estado anterior del botón
  static bool servoPosition = false; // Variable para mantener el estado del servo
  
  if (fingerID != -1 || (buttonState == HIGH && lastButtonState == LOW)) {
    
    if (!servoPosition) {
      miServo.attach(8); // Adjunta el servo para enviarle corriente
      miServo.write(180); // Mueve el servo a 180 grados si se detecta una huella válida o se presiona el botón
      // Encender el primer LED
      digitalWrite(ledPin1, HIGH);
      
      if(fingerID != -1){
        int fingerID = finger.fingerID;
        String owner = (fingerID >= 0 && fingerID < numOwners) ? owners[fingerID] : "Desconocido";
        lcd.clear();
        delay(300);
        lcd.setCursor(0, 0);
        lcd.print("Bienvenido Sr.  ");
        lcd.setCursor(0, 2);
        lcd.print("----- " + owner + "  "); 
        delay(1000);
        // Apagar el primer LED
       digitalWrite(ledPin1, LOW);
       delay(2000);
      } else {
        lcd.setCursor(0, 2);
        lcd.print("                "); 
        lcd.setCursor(0, 0);
        lcd.setCursor(0, 0);
        lcd.print("Open...         ");
        delay(1500);
        // Apagar el primer LED
       digitalWrite(ledPin1, LOW);
       delay(1100);
        
      }
    } else {
      miServo.attach(8); // Adjunta el servo para enviarle corriente
      miServo.write(0); // Mueve el servo a 0 grados si se detecta una huella válida o se presiona el botón nuevamente
      // Encender el primer LED
      digitalWrite(ledPin1, HIGH);
      lcd.setCursor(0, 2);
      lcd.print("                "); 
      lcd.setCursor(0, 0);
      lcd.print("Close...        ");
      delay(1500);
      // Apagar el primer LED
       digitalWrite(ledPin1, LOW);
      delay(1200);
    }
    servoPosition = !servoPosition; // Invierte el estado del servo
    lcd.clear();
    delay(300);
    lcd.setCursor(0, 2);
    lcd.print(" -------------- ");
    lcd.setCursor(0, 0);
    lcd.print("Esperando Huella");    
    delay(500); 
    miServo.detach(); // Detacha el servo para apagarlo cuando no se esté utilizando
    
  }
  lastButtonState = buttonState;
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    digitalWrite(ledPin2, HIGH);
    lcd.clear();
        delay(250);
        lcd.setCursor(0, 0);
        lcd.print("Acceso Denegado.");
        lcd.setCursor(0, 2);
        lcd.print("---- desconocido"); 
        delay(750); // Esperar 1 segundo
        // Apagar el segundo LED
        //digitalWrite(ledPin2, LOW);
        delay(100);
    return -1;
  }
  
  int fingerID = finger.fingerID;
  String owner = (fingerID >= 0 && fingerID < numOwners) ? owners[fingerID] : "Desconocido";

  Serial.print("Se encontró el ID #"); 
  Serial.print(fingerID); 
  Serial.print(" asociado a "); 
  Serial.println(owner);
  
  digitalWrite(ledPin2, LOW);
  return fingerID;
}
