#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

//////////////////////////////////////////////////////////////////////////  RFID Init
#define SS_PIN 48
#define RST_PIN 49
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
//////////////////////////////////////////////////////////////////////////  ***END*** RFID Init



//////////////////////////////////////////////////////////////////////////  LEDs Init
#define LED1 43
#define LED2 41
char LED_Set[] = {LED1, LED2};
//////////////////////////////////////////////////////////////////////////  ***END*** LEDs Init



//////////////////////////////////////////////////////////////////////////  Buzzer Init
#define beep_pin 8
//////////////////////////////////////////////////////////////////////////  ***END*** Buzzer Init



//////////////////////////////////////////////////////////////////////////  Path Detection Init
#define INFINITY 9999
#define SIZE 8                    //No of vertices

int final[SIZE];
int final_dir[SIZE];
int sizeOfElements = 0;

boolean mapCreated = false;
boolean pathSet = false;
int count = 0;

struct prev_node {
  char direction[SIZE];
};

struct cur_node {
  struct prev_node pre[SIZE];
};

struct cur_node nodes[SIZE];
int G[SIZE][SIZE] = {
                      {0, 1, 0, 0, 0, 0, 0, 0},
                      {1, 0, 1, 0, 1, 0, 0, 0},
                      {0, 1, 0, 1, 0, 0, 0, 0},
                      {0, 0, 1, 0, 1, 0, 0, 0},
                      {0, 1, 0, 1, 0, 1, 0, 1},
                      {0, 0, 0, 0, 1, 0, 1, 0},
                      {0, 0, 0, 0, 0, 1, 0, 1},
                      {0, 0, 0, 0, 1, 0, 1, 0}
                    };
//////////////////////////////////////////////////////////////////////////  ***END*** Path Detection Init




//////////////////////////////////////////////////////////////////////////  Wifi Module Init
String WIFI_SSID = "Dialog 23";        //Your WiFi ssid
String PASSWORD = "Kapil@1996";  //Your WiFi password

String host="192.168.8.100";      // IP address of the computer which runs the localhost server(run 'ipconfig' in windows command prompt and get the ip address)
String port="8080";

SoftwareSerial esp8266(32, 33);     //TX, RX
//////////////////////////////////////////////////////////////////////////  ***END*** Wifi Module Init



//////////////////////////////////////////////////////////////////////////  LCD Init
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);    // Define LCD display pins RS, E, D4, D5, D6, D7
//////////////////////////////////////////////////////////////////////////  ***END*** LCD Init



//////////////////////////////////////////////////////////////////////////  Push Button Init
int push_pin = 20;
//////////////////////////////////////////////////////////////////////////  ***END*** Push Button Init



int array_size = 2;
byte bookID[4];
byte shelfAddress[4];
int id = 0;
int j = 0;
boolean isAlreadyScanned = false;
char turns[15];
int noOfScans = 0;
int shelfNo = 1;



//////////////////////////////////////////////////////////////////////////  Line Follower Init
int S_A = 11;  //speed motor a
int M_A1 = 2; //motor a = +
int M_A2 = 3; //motor a = -
int M_B1 = 4; //motor b = -
int M_B2 = 5; //motor b = +
int S_B = 10;  //speed motor b

int R_S = A0; //sincer R
int S_S = A1; //sincer S
int L_S = A2; //sincer L
//////////////////////////////////////////////////////////////////////////  ***END*** Line Follower Init

void createMap();


void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication

  //////////////////////////////////////////////////////////////////////////  Path Detection Setup
  int row, col;
  
  createMap();  
  
  for(row = 0; row < SIZE; row++) {
    for(col = 0; col < SIZE; col++) {
      Serial.print(G[row][col]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();
  
  mapCreated = true;
  //////////////////////////////////////////////////////////////////////////  ***END*** Path Detection Setup


 //////////////////////////////////////////////////////////////////////////  LEDs Setup
  pinMode(LED1 , OUTPUT);
  pinMode(LED2 , OUTPUT);
  //////////////////////////////////////////////////////////////////////////  ***END*** LEDs Setup
  
  
  //////////////////////////////////////////////////////////////////////////  Line Follower Setup
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
  //////////////////////////////////////////////////////////////////////////  ***END*** Line Follower Setup
  
  //////////////////////////////////////////////////////////////////////////  Buzzer Setup
  pinMode(beep_pin,OUTPUT);
  digitalWrite(beep_pin,LOW);
  //////////////////////////////////////////////////////////////////////////  ***END*** Buzzer Setup

  //////////////////////////////////////////////////////////////////////////  Push Button Setup
  pinMode(push_pin,INPUT);
  //////////////////////////////////////////////////////////////////////////  ***END*** Push Button Setup

  //////////////////////////////////////////////////////////////////////////  LCD Setup
  lcd.begin(16, 2);
  lcd.setCursor(3, 1);
  lcd.print("Welcome!!");
  lcd.setCursor(0, 0);
  lcd.print("Circuit is Ready");
  delay(1500);
  lcd.clear();  
  //////////////////////////////////////////////////////////////////////////  ***END*** LCD Setup

  SPI.begin();      // Initiate  SPI bus

  //////////////////////////////////////////////////////////////////////////  RFID Setup
  mfrc522.PCD_Init();   // Initiate MFRC522
  //////////////////////////////////////////////////////////////////////////  ***END*** RFID Setup
  
  //////////////////////////////////////////////////////////////////////////  LCD Setup 2
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("the server.....");
  //////////////////////////////////////////////////////////////////////////  ***END*** LCD Setup 2

  //////////////////////////////////////////////////////////////////////////  Wifi Module Setup
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
  //////////////////////////////////////////////////////////////////////////  ***END*** Wifi Module Setup

  
  Serial.println("Scan the book to the reader...");
  Serial.println();
}



void loop() 
{
//  Serial.flush();
//  esp8266.flush();
  boolean isAlreadyScanned = false;
  Serial.println("ID..............");
  Serial.println(id);
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

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Place it in");
        lcd.setCursor(0, 1);
        lcd.print("Shelf ");
        lcd.print(shelfNo);
        Serial.print("Place it in Shelf ");
        Serial.print(shelfNo);
        Serial.println();
        shelfNo++;

        delay(1500);
    
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
    if(id < (array_size + 3))                                                          // Until the id increments to total array size + 3
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

      
      if(id==(array_size + 3)) {
        char sn = 'S', pn = 'S', en = 'S';
        for(char l = 'A'; l<='C'; l++) {
          bool isInTheArray = false;
          for(int m = 0; m<sizeof(shelfAddress); m++) {
            if(char(shelfAddress[m]) == l) {
              isInTheArray = true;
            }
          }
          if(isInTheArray) {
            Serial.println();
            Serial.print(l);
            en = l;
            Serial.println(pn);
            pn = printPath(sn, pn, en);
            Serial.println(pn);
            sn = l;
            Robot();
            for(int n = 0; n < 5; n++) {
              digitalWrite(beep_pin,HIGH);
              delay(500);
              digitalWrite(beep_pin,LOW);
              delay(500);
            }
            for (int n = 0; n < array_size; n++) {
              if(char(shelfAddress[n]) == l) {
                digitalWrite(char(LED_Set[n]) , HIGH);
              }      
//              delay(1500);
            }
            Serial.println("waiting");
            buttonWait(13);                     //Wait until push buttion(connected to pin 13) is pressed
            delay(1000);
            for (int n = 0; n < array_size; n++) {
              digitalWrite(char(LED_Set[n]) , LOW);
//              delay(1500);
            }
            Serial.println("Done");
          }
        }
        Serial.println("Return");
        pn = printPath(sn, pn, 'S');
        int k = 0;
        for(k = 0; turns[k]!='z'; k++) {        //uTurn when it returns to the starting point
          Serial.print(turns[k]);
          Serial.print("->");
        }
        turns[k]='u';
        turns[k+1] = 'z';
        
        Robot();
        id++;
      }
      
      id = 0;                             //Reset every variables
      j = 0;
      isAlreadyScanned = false;
      noOfScans = 0;
      shelfNo = 1;
    }
  }
  mfrc522.PICC_HaltA();           //comment this if you want to read one tag continuously
  mfrc522.PCD_StopCrypto1();      //comment this if you want to read only one tag
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





//////////////////////////////////////////////////////////////////////////  Line Follower Function
void Robot(){
  noOfScans++;
  if(noOfScans == 1) {
    for(int i = 0; turns[i]!='z'; i++) {
      if(turns[i] == 's' || turns[i] == 'l' || turns[i] == 'r') {
        while(!((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1))) {
          if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 0)){forword();}
          
          if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 0)){turnLeft();}
          if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) ==0)&&(digitalRead(R_S) == 0)) {turnLeft();}
        
          if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1)){turnRight();}
          if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 0)&&(digitalRead(R_S) == 1)){turnRight();}
        }
        if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1)){
          Stop();
          delay(500);
          if(turns[i+1] == 'l') {
            turnLeft();
            delay(250);
          } else if(turns[i+1] == 'r') {
            turnRight();
            delay(250);
          } else {
            forword();
          }
        }
        turns[i] = 'z';
      }
    }
  } else {
    for(int i = 0; turns[i]!='z'; i++) {
      if(turns[i] == 's') {
        while(!((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1))) {
          if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 0)){forword();}
          
          if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 0)){turnLeft();}
          if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) ==0)&&(digitalRead(R_S) == 0)) {turnLeft();}
        
          if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1)){turnRight();}
          if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 0)&&(digitalRead(R_S) == 1)){turnRight();}
        }
        if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1)){
          Stop();
          delay(500);
          if(turns[i+1] == 'l') {
            turnLeft();
            delay(250);
          } else if(turns[i+1] == 'r') {
            turnRight();
            delay(250);
          } else {
            forword();
          }
        }
        turns[i] = 'z';
      }
      if(turns[i] == 'l' || turns[i] == 'r' || turns[i] == 'u') {
        if(turns[i] == 'l') {
          turnLeft();
          delay(250);
        } else if(turns[i] == 'r') {
          turnRight();
          delay(250);
        } else if(turns[i] == 'u') {
          uTurn();
//          while ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 0)&&(digitalRead(R_S) == 0)){turnLeft();}
          if(turns[i+1] == 'z') {
            Stop();
            return;
          }
        }
        while(!((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1))) {
          if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 0)){forword();}
          
          if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 0)){turnLeft();}
          if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) ==0)&&(digitalRead(R_S) == 0)) {turnLeft();}
        
          if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1)){turnRight();}
          if ((digitalRead(L_S) == 0)&&(digitalRead(S_S) == 0)&&(digitalRead(R_S) == 1)){turnRight();}
        }
        if ((digitalRead(L_S) == 1)&&(digitalRead(S_S) == 1)&&(digitalRead(R_S) == 1)){
          Stop();
          delay(500);
          if(turns[i+1] == 'l') {
            turnLeft();
            delay(250);
          } else if(turns[i+1] == 'r') {
            turnRight();
            delay(250);
          } else {
            forword();
          }
        }
        turns[i] = 'z';
      }
    }
  }
  Stop();
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


void uTurn(){
  analogWrite(S_A, 250); 
  analogWrite(S_B, 250);
  digitalWrite(M_A1, HIGH);
  digitalWrite(M_A2, LOW);
  digitalWrite(M_B1, HIGH);
  digitalWrite(M_B2, LOW);
  delay(900);
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
//////////////////////////////////////////////////////////////////////////  ***END*** Line Follower Function




//////////////////////////////////////////////////////////////////////////  Path Detection Function
int findPath(int starting_node, int previous_node, int ending_node) {
  int cost[SIZE][SIZE], distance[SIZE], pred[SIZE], visited[SIZE];
  int count, mindistance, nextnode, row, col;
  
  //pred[] stores the predecessor of each node
  //count gives the number of nodes seen so far

  //create the cost matrix
  for(row = 0; row < SIZE; row++) {
    for(col = 0; col < SIZE; col++) {
      if(G[row][col] == 0) {
        cost[row][col] = INFINITY;
      } else {
        cost[row][col] = G[row][col];
      }
    }
  }

  //initialize pred[],distance[] and visited[]
  for(row = 0; row < SIZE; row++) {
    distance[row] = cost[starting_node][row];
    pred[row] = starting_node;
    visited[row] = 0;
  }
  
  distance[starting_node] = 0;
  visited[starting_node] = 1;
  count = 1;
  
  while(count < (SIZE - 1)) {
    mindistance = INFINITY;
    
    //nextnode gives the node at minimum distance
    for(row = 0; row < SIZE; row++) {
      if((distance[row] < mindistance) && !visited[row]) {
        mindistance = distance[row];
        nextnode = row;
      }
    }
  
    //check if a better path exists through nextnode            
    visited[nextnode] = 1;
    for(row = 0; row < SIZE; row++) {
      if(!visited[row]) {
        if((mindistance + cost[nextnode][row]) < distance[row]) {
          distance[row] = mindistance + cost[nextnode][row];
          pred[row] = nextnode;
        }
      }
    }
    count++;
  }

  // //print the path, direction and distance of each node
  // for(row = 0; row < SIZE; row++) {
  //     if(row != starting_node) {
  //         Serial.println("\nDistance of node%d = %d", row, distance[row]);
          
  //         int i = 0;
  //         int final[SIZE];
  //         int final_dir[SIZE];
  //         col = row;
  //         final[i] = col;
  //         do {
  //             i++;
  //             col = pred[col];
  //             final[i] = col;
  //         }while(col != starting_node);

  //         //print path
  //         Serial.println("\nPath = ");
  //         Serial.println("%d", final[i]);
  //         for(int k = i-1; k>=0; k--) {
  //             Serial.println("->%d", final[k]);
  //         }

  //         //print direction
  //         final_dir[i] = nodes[final[i]].pre[previous_node].direction[final[i-1]];
  //          Serial.println("\n%c", final_dir[i]);
  //         for(int k = i-1; k > 0; k--) {
  //             final_dir[k] = nodes[final[k]].pre[final[k+1]].direction[final[k-1]];
  //              Serial.println("->%c", final_dir[k]);
  //         }

  //  }
  // }
  // Serial.println("\n");

  //print the path, direction and distance of a perticular end node
  row = ending_node;
  if(row != starting_node) {
    Serial.print("\nDistance of node");
    Serial.print(row);
    Serial.print(" = ");
    Serial.print(distance[row]);
    
    int i = 0;
    col = row;
    final[i] = col;
    do {
      i++;
      col = pred[col];
      final[i] = col;
    }while(col != starting_node);
    
    //print path
    Serial.println("\nPath = ");
    Serial.print(final[i]);
    for(int k = i-1; k >= 0; k--) {
      Serial.print("->");
      Serial.print(final[k]);
    }
    Serial.println();

    //print direction
    final_dir[0] = 'A';
    Serial.print(previous_node);
    final_dir[i-1] = nodes[final[i]].pre[previous_node].direction[final[i-1]];
    int k = 0;
    turns[k] = final_dir[i-1];
    k++;
    Serial.print(char(final_dir[i-1]));
    for(int m = i-2; m >= 0; m--) {
      final_dir[m] = nodes[final[m+1]].pre[final[m+2]].direction[final[m]];
      Serial.print("->");
      Serial.print(char(final_dir[m]));
      turns[k] = char(final_dir[m]);
      k++;
    }
    turns[k] = 'z';
    Serial.println();
    sizeOfElements = i;

    for(int m = 0; turns[m]!='z'; m++) {
      Serial.print(turns[m]);
      Serial.print("->");
    }
  }

  return final[1];
}



char printPath(char sn, char pn, char en) {
  if(mapCreated){
    char nodes_char[] = {'S', 'u', 'A', 'B', 'v', 'w', 'C', 'x'};
    char nodes_int[] = {0, 1, 2, 3, 4, 5, 6, 7};
    int starting_node, previous_node, ending_node;

    for(int i = 0; i<sizeof(nodes_char); i++) {
      if(sn == nodes_char[i]) {
        starting_node = nodes_int[i];
      }
      if(pn == nodes_char[i]) {
        previous_node = nodes_int[i];
      }
      if(en == nodes_char[i]) {
        ending_node = nodes_int[i];
      }
    }
  
    int pn2 = findPath(starting_node, previous_node, ending_node);
    Serial.println();

    return nodes_char[pn2];
  }

  return pn;
}


void createMap() {
  nodes[0].pre[0].direction[1] = 's';
  nodes[0].pre[1].direction[1] = 'u';

  nodes[1].pre[0].direction[0] = 'u';
  nodes[1].pre[0].direction[2] = 'l';
  nodes[1].pre[0].direction[4] = 's';
  nodes[1].pre[2].direction[0] = 'r';
  nodes[1].pre[2].direction[2] = 'u';
  nodes[1].pre[2].direction[4] = 'l';
  nodes[1].pre[4].direction[0] = 's';
  nodes[1].pre[4].direction[2] = 'r';
  nodes[1].pre[4].direction[4] = 'u';

  nodes[2].pre[1].direction[1] = 'u';
  nodes[2].pre[1].direction[3] = 'r';
  nodes[2].pre[3].direction[1] = 'l';
  nodes[2].pre[3].direction[3] = 'u';

  nodes[3].pre[2].direction[2] = 'u';
  nodes[3].pre[2].direction[4] = 'r';
  nodes[3].pre[4].direction[2] = 'l';
  nodes[3].pre[4].direction[4] = 'u';

  nodes[4].pre[1].direction[1] = 'u';
  nodes[4].pre[1].direction[3] = 'l';
  nodes[4].pre[1].direction[5] = 'r';
  nodes[4].pre[1].direction[7] = 's';
  nodes[4].pre[3].direction[1] = 'r';
  nodes[4].pre[3].direction[3] = 'u';
  nodes[4].pre[3].direction[5] = 's';
  nodes[4].pre[3].direction[7] = 'l';
  nodes[4].pre[5].direction[1] = 'l';
  nodes[4].pre[5].direction[3] = 's';
  nodes[4].pre[5].direction[5] = 'u';
  nodes[4].pre[5].direction[7] = 'r';
  nodes[4].pre[7].direction[1] = 's';
  nodes[4].pre[7].direction[3] = 'r';
  nodes[4].pre[7].direction[5] = 'l';
  nodes[4].pre[7].direction[7] = 'u';

  nodes[5].pre[4].direction[4] = 'u';
  nodes[5].pre[4].direction[6] = 'l';
  nodes[5].pre[6].direction[4] = 'r';
  nodes[5].pre[6].direction[6] = 'u';

  nodes[6].pre[5].direction[5] = 'u';
  nodes[6].pre[5].direction[7] = 'l';
  nodes[6].pre[7].direction[5] = 'r';
  nodes[6].pre[7].direction[7] = 'u';

  nodes[7].pre[4].direction[4] = 'u';
  nodes[7].pre[4].direction[6] = 'r';
  nodes[7].pre[6].direction[4] = 'l';
  nodes[7].pre[6].direction[6] = 'u';
}
//////////////////////////////////////////////////////////////////////////  ***END*** Path Detection Function



void buttonWait(int buttonPin){
  int buttonState = 0;
  while(1){
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {
      return;
    }
  }
}
