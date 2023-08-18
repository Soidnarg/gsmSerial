#include <avr/io.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>
#include "sim800.h"
#include "gsmSerial.h"

int InitGsm();
String ReadAllSms();
int WriteSms(String da, String message);
String ReadThings();
void ManualComunication();

typedef struct Gestore{  
  String name;
  String apn;
  String mmscMms;
  String mmscInternet;
  int mcc;
  int mnc;
  String apnTypeMms;
  String apnTypeInternet;
  String apnProtocol;
  String apnProtocolRoaming;
  String operatorType;
  String valueVirtualOperator;    
  String sms;  
} Gestore;

Gestore Iliad = {
  "Iliad",
  "Iliad",
  "http://mms.iliad.it",
  "http://mms.iliad.it",
  222,
  50,
  "default, supl, mms, dun",
  "default, supl, mms, dun",
  "Ipv4",
  "Ipv4",
  "gid",
  "F003",
  "+393519999600",
};

SoftwareSerial mySerial(2, 3);

void setup() {
  int ris;
  // put your setup code here, to run once:
  mySerial.begin(9600);
  Serial.begin(9600);
  delay(5000); 
/*  
  ris = InitGsm();
  if(ris != ITSOK) {
    Serial.println(ris);
    Reset_AVR();
  }
*/
}

void loop() {
  // put your main code here, to run repeatedly:
  ManualComunication();
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
  delay(TYPICAL_DELAY);
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
  delay(TYPICAL_DELAY);
  answer = ReadThings();
  if(answer.indexOf("\r\nOK") == -1) {
    return ERR_NO_OK;  
  }

  mySerial.println("AT+CSCA=\"" + Iliad.sms + "\"\r");
  delay(TYPICAL_DELAY);
  answer = ReadThings();
  if(answer.indexOf("\r\nOK") == -1) {
    return ERR_NO_OK;  
  }

  mySerial.println("AT+CGATT=1\r");
  delay(TYPICAL_DELAY);
  answer = ReadThings();
  if(answer.indexOf("\r\nOK") == -1) {
    return ERR_NO_OK;  
  }
    
  mySerial.println("AT+CGACT=1\r");
  delay(TYPICAL_DELAY);
  answer = ReadThings();
  if(answer.indexOf("\r\nOK") == -1) {
    return ERR_NO_OK;  
  }
    
  mySerial.println("AT+CGREG=2\r");
  delay(TYPICAL_DELAY);
  answer = ReadThings();
  if(answer.indexOf("\r\nOK") == -1) {
    return ERR_NO_OK;  
  }
  
  return ITSOK;
}

String ReadAllSms() {
  mySerial.println("AT+CMGL=\"ALL\", 1");
  delay(TYPICAL_DELAY);
  return ReadThings();
}

int WriteSms(String da, String message) {
  String answer;

  mySerial.println("AT+CMGS=\"" + da + "\"\r");
  delay(TYPICAL_DELAY);
  answer = ReadThings();
  if(answer.indexOf(">") == -1) {
    return ERR_NO_OK;
  } else {
    mySerial.println(message + 0x1A);
    delay(TYPICAL_DELAY);
    answer = ReadThings();
    if(answer.indexOf("\r\nOK") == -1) {
      return ERR_NO_OK;
    } else {
      return sizeof(message);
    }
  }
}

void Retry() {
  mySerial.println("A/");
  delay(TYPICAL_DELAY);
}

int Answer() {
  String answer;
  
  mySerial.println("ATA");
  delay(TYPICAL_DELAY);
  answer = ReadThings();
  if(answer.indexOf("OK") != -1) {
    return OK
  } else if(answer.indexOf("CONNECT") != -1) {
    return CONNECT
  } else return ERR_NO_OK;
}

void ManualComunication() {
  while(mySerial.available()) {
    Serial.write(mySerial.read());
  }
  
  while(Serial.available()) {
    mySerial.write(Serial.read());
  }
}