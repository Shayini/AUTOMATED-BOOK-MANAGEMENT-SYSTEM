#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#define SS_PIN 48
#define RST_PIN 49
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define beep_pin 8

String WIFI_SSID = "FSLT";        //Your WiFi ssid
String PASSWORD = "@Thaksha17$";  //Your WiFi password

String host="192.168.1.103";      // IP address of the computer which runs the localhost server(run 'ipconfig' in windows command prompt and get the ip address)
String port="8080";

SoftwareSerial esp8266(32, 33);     //TX, RX
 


LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

int array_size = 1;
byte bookID[4];
byte shelfAddress[4];
int id = 0;
int j = 0;
boolean isAlreadyScanned = false;


//////////////////////////////////////////////////////////////////////////  Line Follower Pin
int S_A = 11;  //speed motor a
int M_A1 = 2; //motor a = +
int M_A2 = 3; //motor a = -
int M_B1 = 4; //motor b = -
int M_B2 = 5; //motor b = +
int S_B = 10;  //speed motor b

int R_S = A0; //sincer R
int S_S = A1; //sincer S
int L_S = A2; //sincer L
//////////////////////////////////////////////////////////////////////////  ***END*** Line Follower Pin


void setup() 
{
  //////////////////////////////////////////////////////////////////////////  Line Follower PinMode
  pinMode(M_B1, OUTPUT);
  pinMode(M_B2, OUTPUT);
  pinMode(M_A1, OUTPUT);
  pinMode(M_A2, OUTPUT);
  pinMode(S_B, OUTPUT);
  pinMode(S_A, OUTPUT);
  
  pinMode(L_S, INPUT);
  pinMode(S_S, INPUT);
  pinMode(R_S, INPUT);
  
//  analogWrite(S_A, 255); 
//  analogWrite(S_B, 255); 
  delay(200);
  //////////////////////////////////////////////////////////////////////////  ***END*** Line Follower PinMode


  
  Serial.begin(9600);   // Initiate a serial communication

  pinMode(beep_pin,OUTPUT);
  digitalWrite(beep_pin,LOW);
  
  lcd.begin(16, 2);
  lcd.setCursor(3, 1);
  lcd.print("Welcome!!");
  lcd.setCursor(0, 0);
  lcd.print("Circuit is Ready");
  delay(1500);
  lcd.clear();  
  
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522


  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("the server.....");

  esp8266.begin(115200); //Baud rate for communicating with ESP8266. Your's might be different.
  esp8266Serial("AT+RST\r\n", 1000); // rst
  delay(1500);
  esp8266Serial("AT+CWMODE=1\r\n", 1000);
  delay(1000);
  esp8266Serial("AT+CWJAP=\"" + WIFI_SSID + "\",\"" + PASSWORD + "\"\r\n", 8000);
  delay(6000);
  esp8266Serial("AT+CIFSR\r\n", 1000); // get ip address
  delay(1000);
  esp8266Serial("AT+CWJAP?\r\n", 1000);



  Serial.println("Scan the book to the reader...");
  Serial.println();
}



void loop() 
{
//  Serial.flush();
//  esp8266.flush();
  boolean isAlreadyScanned = false;

  if (id<array_size)
  {
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
    //some variables we need
    byte buffer[18];
    byte block;
    byte len = 18;
    MFRC522::StatusCode status;
  
    //-------------------------------------------

    
    digitalWrite(beep_pin, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan the book to");
    lcd.setCursor(0, 1);
    lcd.print("the reader......");
    
    delay(300);
  
  
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      return;
    }
    
    
    //Show UID on serial monitor
    Serial.print("UID tag :");
    String content= "";
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(mfrc522.uid.uidByte[i], HEX);
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    //Serial.print("Message : ");
    content.toUpperCase();
    
  
    //B6 76 4A DB = Cryptography and Network Security
    //C6 00 4A DB = The Microcontroller and Embedded System
    //26 D7 5D DB = Data Structures and Algorithms
    //A6 9D 5D DB = Software Design
    
    if (content.substring(1) == "B6 76 4A DB" || content.substring(1) == "C6 00 4A DB" || content.substring(1) == "26 D7 5D DB" || content.substring(1) == "A6 9D 5D DB") //check whether the scanned book belongs to our library
    {
      digitalWrite(beep_pin,HIGH);
      delay(200);
      digitalWrite(beep_pin,LOW);
      delay(100);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Scanning.....");
      
//      lcd.clear();
//      lcd.setCursor(0, 0);
//      lcd.print("ID : ");
//      lcd.print(content.substring(1));
//  
//      delay(1200);
  
      //------------------------------------------- GET BookID
      
      block = 1;
    
      Serial.print(F("BookID: "));
    
      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
      if (status != MFRC522::STATUS_OK)
      {
        Serial.print(F("Authentication failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
      }
    
      status = mfrc522.MIFARE_Read(block, buffer, &len);
      if (status != MFRC522::STATUS_OK)
      {
        Serial.print(F("Reading failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
      }
    
      //Print in the Serial
      for (int i = 0; i < 16; i++)
      {
        Serial.write("H");
        Serial.write(buffer[i]);
      }
      Serial.print("\n");

      for(int i = 0; i < id; i++)
      {
        if(buffer[0] == bookID[i])
        {
          isAlreadyScanned = true;
        }
      }

      if(isAlreadyScanned)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Book Already");
        lcd.setCursor(0, 1);
        lcd.print("Scanned!!!!!");
        Serial.println("Book already scanned!!!!!");
        digitalWrite(beep_pin,HIGH);
        delay(300);
        digitalWrite(beep_pin,LOW);
        delay(300);
        digitalWrite(beep_pin,HIGH);
        delay(300);
        digitalWrite(beep_pin,LOW);
        delay(300);
        digitalWrite(beep_pin,HIGH);
         delay(300);
        digitalWrite(beep_pin,LOW);
      }
      else {
        bookID[id] = buffer[0];

        updateBook(bookID[id]-'0');          // If the book is scanned once the status is updated to Available
        updateBook(0);
        
        //Print in the LCD
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        lcd.print("Book ID: ");
//        lcd.print(char(buffer[0]));
//  
//        delay(1200);
  
        //---------------------------------------- GET Shelf Address
    
        block = 12;
      
        Serial.print(F("Shelf Address: "));
      
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 12, &key, &(mfrc522.uid)); //line 834
        if (status != MFRC522::STATUS_OK)
        {
          Serial.print(F("Authentication failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
        }
      
        status = mfrc522.MIFARE_Read(block, buffer, &len);
        if (status != MFRC522::STATUS_OK)
        {
          Serial.print(F("Reading failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
        }
      
        //Print in the Serial
        for (int i = 1; i < 16; i++)
        {
          Serial.write(buffer[i] );
        }
        Serial.print("\n");
        
        shelfAddress[id] = buffer[1];
        id++;
        
//        //Print in the LCD
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        lcd.print("Shelf: ");
//        lcd.print(char(buffer[1]));
//    
//        delay(1200);
    
        //Print in the LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Book Received");
        Serial.println("Book Received");
        
        delay(1200);
    
        //----------------------------------------
          
      }
    }
    else {                                                                            // If the book does not belongs to this Library
      digitalWrite(beep_pin,HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("This book does");
      lcd.setCursor(0, 1);
      lcd.print("not belongs to");
      delay(1500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("this Library!!!!!");
      Serial.println("This book does not belongs to this Library!!!!!");
      
      delay(1500);
    }
  
  }
  else {                                                                              // If the array is FULL
    if(id!=(array_size + 3))                                                          // Until the id increments to total array size + 3
    {
      digitalWrite(beep_pin,HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Fully Loaded");
      Serial.println("Fully Loaded");
      
      delay(1500);
      id++;
    }
    else {                                                                            // If the id is array size + 3, Print the Shelf Address in the LCD
      digitalWrite(beep_pin,LOW);
      if (j!= array_size)
      {
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      for ( ; j < array_size; j++)
      {
        lcd.print(char(shelfAddress[j]));
        lcd.print(" ");
        Serial.println(char(shelfAddress[j]));
        
        delay(1500);
      }
      Robot();
    }
  }
//  mfrc522.PICC_HaltA();         //comment this if you want to read one tag continuously
  mfrc522.PCD_StopCrypto1();      //comment this if you want to read only one tag
}



//The ESP8266 setup code that has to be written inside the void setup(){}
void setup_ESP8266()
{
  
}



//The RFID, LCD adn Buzzer setup code that has to be written inside the void setup(){}
void setup_RFID_LCD_Buzzer()
{
//  Serial.begin(9600);
  
}



// Sending date through the ESP8266 WiFi Module
String esp8266Serial(String command, const int timeout)
{
  Serial.println(command);
  String response = ""; // a String to hold incoming data

  esp8266.print(command);
  
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read(); // Read the answer of ESP8266 character by character
      Serial.write(c);
      response += c;
    }
  }
  Serial.println();

  return response;
}



////POST request to add the Book details
//void addBook(){
//  String cmd = "AT+CIPSTART=\"TCP\",\"" + host + "\"," + port + "\r\n";
//
//  Serial.println(cmd);
//  esp8266Serial(cmd, 300); // turn on server on port 8080
//  
//  Serial.print("AT+CIPMODE=1\r\n");
//  esp8266Serial("AT+CIPMODE=1\r\n", 100);
//  
//  Serial.print("AT+CIPSEND\r\n");
//  esp8266Serial("AT+CIPSEND\r\n", 100);
//  
//  String data = "{\"bookName\": \"Hellooooooooo\",\"author\": \"HIIIIIIII\",\"shelf\": \"A\",\"copyNo\": 3,\"status\": \"Unavailable\"}";
//  
//  cmd = "POST "; cmd += "/Project/REST-API/addbook"; cmd += " HTTP/1.1\r\n";
//  cmd += "Host: "; cmd += host + ":" + port + "\r\n";
//  cmd += "Content-Type: application/json\r\n";
//  cmd += "Connection: close\r\n";
//  cmd += "Content-Length: 100\r\n";
//  cmd += "\r\n";
//  cmd += data + "\r\n";
//  
//  Serial.print(cmd);
//  esp8266Serial(cmd,800); // send data to server
//
//  Serial.flush();
//  esp8266.flush();
//}


//PUT request to update the Book details
void updateBook(int id){
  Serial.println(id);
  String cmd = "AT+CIPSTART=\"TCP\",\"" + host + "\"," + port + "\r\n";

  Serial.println(cmd);
  esp8266Serial(cmd, 300); // turn on server on port 8080
  
  Serial.print("AT+CIPMODE=1\r\n");
  esp8266Serial("AT+CIPMODE=1\r\n", 100);
  
  Serial.print("AT+CIPSEND\r\n");
  esp8266Serial("AT+CIPSEND\r\n", 100);
    
  cmd = "PUT "; cmd += "/Project/REST-API/updatebook/"; cmd += id; cmd += " HTTP/1.1\r\n";
  cmd += "Host: "; cmd += host + ":" + port + "\r\n";
  cmd += "Content-Type: application/json\r\n";
  cmd += "Connection: close\r\n";
  cmd += "Content-Length: 100\r\n";
  cmd += "\r\n";
  
  Serial.print(cmd);
  esp8266Serial(cmd,800); // send data to server

  Serial.flush();
  esp8266.flush();
}





//////////////////////////////////////////////////////////////////////////  Line Follower Run
void Robot(){
  if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 0)){forword();}

  if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 0)){turnLeft();}
  if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) ==0)&&(digitalRead(R_S) == 0)) {turnLeft();}
  
  if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1)){turnRight();}
  if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 0)&&(digitalRead(R_S) == 1)){turnRight();}
  
  if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1)){Stop();}
//  if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 0)&&(digitalRead(R_S) == 0)){
//    analogWrite(S_A, 75); 
//    analogWrite(S_B, 75);
//  }
}


void forword(){
  analogWrite(S_A, 100); 
  analogWrite(S_B, 100);
  
  digitalWrite(M_A1, LOW);
  digitalWrite(M_A2, HIGH);
  digitalWrite(M_B1, HIGH);
  digitalWrite(M_B2, LOW);
}


//void reverse(){
  //analogWrite(S_A, 120); 
  //analogWrite(S_B, 120);
  
  //digitalWrite(M_A1, HIGH);
  //digitalWrite(M_A2, LOW);
  //digitalWrite(M_B1, LOW);
  //digitalWrite(M_B2, HIGH);
//}


//void sharpRightTurn() {
  //analogWrite(S_A, 100); 
  //analogWrite(S_B, 50);
  
  //digitalWrite(M_A1, LOW);
  //digitalWrite(M_A2, HIGH);
  //digitalWrite(M_B1, LOW);
  //digitalWrite(M_B2, HIGH);
//}


//void sharpLeftTurn() {
  //analogWrite(S_A, 50); 
  //analogWrite(S_B, 100);
  
  //digitalWrite(M_A1, HIGH);
  //digitalWrite(M_A2, LOW);
  //digitalWrite(M_B1, HIGH);
  //digitalWrite(M_B2, LOW);
//}


void turnLeft(){
  analogWrite(S_A, 250); 
  analogWrite(S_B, 250);
  
  digitalWrite(M_A1, LOW);
  digitalWrite(M_A2, LOW);
  digitalWrite(M_B1, HIGH);
  digitalWrite(M_B2, LOW);  
}


void turnRight(){
  analogWrite(S_A, 250); 
  analogWrite(S_B, 250);
  
  digitalWrite(M_A1, LOW);
  digitalWrite(M_A2, HIGH);
  digitalWrite(M_B1, LOW);
  digitalWrite(M_B2, LOW);
}


void Stop(){
  //analogWrite(S_A, 0); 
  //analogWrite(S_B, 0);
  
  digitalWrite(M_A1, LOW);
  digitalWrite(M_A2, LOW);
  digitalWrite(M_B1, LOW);
  digitalWrite(M_B2, LOW);
  delay(1000);
}
//////////////////////////////////////////////////////////////////////////  ***END*** Line Follower Run
