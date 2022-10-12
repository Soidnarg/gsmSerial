#include <avr/io.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>

#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {} //definizione della funzione reset

#define ERR_NO_OK 0
#define ERR_PASSWORD_REQUIRED 1

#define ITSOK 9999

int InitGsm();
String ReadAllSms();
String WriteSms(String da, String message);
String ReadThings();


SoftwareSerial mySerial(2, 3);

void setup() {
  int ris;
  // put your setup code here, to run once:
  mySerial.begin(9600);
  Serial.begin(9600);
  delay(10000); 
  ris = InitGsm();
  if(ris != ITSOK) {
    Serial.println(ris);
    Reset_AVR();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}

String ReadThings() {
  String things;
  while(mySerial.available()) {
    things.concat(mySerial.read());
  }  
  return things;
}

int InitGsm() {
  String answer;
  mySerial.println("ATE0");
  delay(25);
  answer = ReadThings();
  if(answer.indexOf("\r\nOK") == -1) {
    return ERR_NO_OK;  
  }
  
  mySerial.println("AT+CPIN?");
  delay(5000);
  answer=ReadThings();
  if(answer.indexOf("\r\nOK") == -1) {
    return ERR_NO_OK;  
  } else if(answer.indexOf("SIM PIN") != -1 || answer.indexOf("SIM PUK") != -1) {
    return ERR_PASSWORD_REQUIRED;
  }

  mySerial.println("AT+CMGF=1");
  delay(25);
  answer = ReadThings();
  if(answer.indexOf("\r\nOK") == -1) {
    return ERR_NO_OK;  
  }

  
    
  return ITSOK;
}

String ReadAllSms() {
  mySerial.println("AT+CMGL=\"ALL\", 1");
  delay(25);
  return ReadThings();
}

String WriteSms(String da, String message) {
  String answer;

  mySerial.println("AT+CMGS=\"%s\"\r", da);
  delay(25);  
  if(answer.indexOf(">") == -1) {
    return ERR_NO_OK;
  } else {
    mySerial.println("%s%c", message, 0x1A);
    delay(25);
    return ReadThings;
  }
}