#include <SoftwareSerial.h>

String WIFI_SSID = "FSLT";        //Your WiFi ssid
String PASSWORD = "@Thaksha17$";  //Your WiFi password

String host="192.168.1.105";      // IP address of the computer which runs the localhost server(run 'ipconfig' in windows command prompt and get the ip address)
String port="8080";

int count = 0;

SoftwareSerial esp8266(3, 2);     //TX, RX

void setup()
{
  Serial.begin(9600);
  esp8266.begin(115200); //Baud rate for communicating with ESP8266. Your's might be different.
  esp8266Serial("AT+RST\r\n", 1000); // rst
  delay(1500);
  esp8266Serial("AT+CWMODE=1\r\n", 1000);
  delay(1000);
  esp8266Serial("AT+CWJAP=\"" + WIFI_SSID + "\",\"" + PASSWORD + "\"\r\n", 8000);
  delay(8000);
  esp8266Serial("AT+CIFSR\r\n", 1000); // get ip address
  delay(1000);
  esp8266Serial("AT+CWJAP?\r\n", 1000);
}

void loop()
{
  Serial.flush();
  esp8266.flush();
  
  Serial.println("HI");
  Serial.println(count);

  if(count == 0){
    addBook();
    updateBook(3);
    count++;
  }


  if (esp8266.available())
  {
    Serial.println(".................");

    if (esp8266.find("+IPD,"))
    {
      String msg;
      msg = esp8266.readStringUntil('.');
      String command1 = msg.substring(0, 3);
      String command2 = msg.substring(4);
    }
  }
}



//POST request to add the Book details
void addBook(){
  String cmd = "AT+CIPSTART=\"TCP\",\"" + host + "\"," + port + "\r\n";

  Serial.println(cmd);
  esp8266Serial(cmd, 300); // turn on server on port 8080
  
  Serial.print("AT+CIPMODE=1\r\n");
  esp8266Serial("AT+CIPMODE=1\r\n", 100);
  
  Serial.print("AT+CIPSEND\r\n");
  esp8266Serial("AT+CIPSEND\r\n", 100);
  
  String data = "{\"bookName\": \"Hellooooooooo\",\"author\": \"HIIIIIIII\",\"shelf\": \"A\",\"copyNo\": 3,\"status\": \"Unavailable\"}";
  
  cmd = "POST "; cmd += "/Project/REST-API/addbook"; cmd += " HTTP/1.1\r\n";
  cmd += "Host: "; cmd += host + ":" + port + "\r\n";
  cmd += "Content-Type: application/json\r\n";
  cmd += "Connection: close\r\n";
  cmd += "Content-Length: 100\r\n";
  cmd += "\r\n";
  cmd += data + "\r\n";
  
  Serial.print(cmd);
  esp8266Serial(cmd,800); // send data to server

  Serial.flush();
  esp8266.flush();
}


//PUT request to update the Book details
void updateBook(int id){
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










String esp8266Serial(String command, const int timeout)
{
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
