#include <avr/io.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>

//#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {} //definizione della funzione reset

#define TYPICAL_DELAY 25

#define ERR_NO_OK 0
#define ERR_PASSWORD_REQUIRED 1

#define ITSOK 9999

int InitGsm();
String ReadAllSms();
int WriteSms(String da, String message);
String ReadThings();

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
  while(mySerial.available()) {
    Serial.write(mySerial.read());
  }
  
  while(Serial.available()) {
    mySerial.write(Serial.read());
  }
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