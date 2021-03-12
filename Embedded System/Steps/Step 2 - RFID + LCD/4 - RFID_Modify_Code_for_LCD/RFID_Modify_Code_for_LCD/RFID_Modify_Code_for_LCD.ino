/*
 * RFID
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
 
#define SS_PIN 10                   // Can change these pins for RFID
#define RST_PIN 9                   // Can change these pins for RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define beep_pin 8                  // Assigning pin for buzzer

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

byte array_size = 4;
byte bookID[4];
byte shelfAddress[4];
byte id = 0;
byte j = 0;
boolean isAlreadyScanned = false;
 
void setup() 
{
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
  Serial.println("Scan the book to the reader...");
  Serial.println();

}
void loop() 
{
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
      for (uint8_t i = 0; i < 16; i++)
      {
        Serial.write(buffer[i]);
      }
      Serial.print("\n");

      for(uint8_t i = 0; i < id; i++)
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
        for (uint8_t i = 1; i < 16; i++)
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
    else {
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
  else
  {
    if(id!=(array_size + 3))
    {
      digitalWrite(beep_pin,HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Fully Loaded");
      Serial.println("Fully Loaded");
      
      delay(1500);
      id++;
    }
    else
    {
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
    }
  }
//  mfrc522.PICC_HaltA();         //comment this if you want to read one tag continuously
  mfrc522.PCD_StopCrypto1();      //comment this if you want to read only one tag
}
