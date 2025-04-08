const byte READY = 1;
  const byte KOIN_MASUK1 = 2;
  const byte KOIN_MASUK2 = 3;
  const byte SALDO_1000 = 4;
  const byte SALDO_2000 = 5;
  const byte KOIN_KELUAR = 6;
  byte state = READY;
  
  #define ENA_R 14
  #define ENA_L 12            
  #define MR_1  15       
  #define MR_2  13
  #define MR_3  0       
  #define MR_4  2               
  #define TRIGGER_PIN  5  
  #define ECHO_PIN     4  
  #define MAX_DISTANCE 250
  
  #include <NewPing.h> 
  #include <ESP8266WiFi.h>
  #include <LiquidCrystal_I2C.h>    
  LiquidCrystal_I2C lcd(0x3F,16,2);
     
  const char* ssid = "home";
  const char* password = "12345678";
  
  unsigned long currentTime = millis();
  unsigned long previousTime = 0; 
  const long timeoutTime = 2000;
  WiFiServer server(80);

  String header;

  NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 

  
  void setup() {
   lcd.begin(16,2);      
   lcd.backlight();  
   
   pinMode(ENA_R, OUTPUT);
   pinMode(ENA_L, OUTPUT);
   pinMode(MR_1, OUTPUT);
   pinMode(MR_2, OUTPUT);
   pinMode(MR_3, OUTPUT);
   pinMode(MR_4, OUTPUT);
   
   Serial.begin(115200);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
  }
void item1(int t){
  digitalWrite(MR_3, LOW);
  digitalWrite(MR_4, HIGH);
  analogWrite(ENA_L, 150);
  delay(t);
  }

void item2(int t){
  digitalWrite(MR_1, LOW);
  digitalWrite(MR_2, HIGH);
  analogWrite(ENA_R, 150);
  delay(t);
  }

void brake(int t){  
   
  digitalWrite(MR_1, LOW);
  digitalWrite(MR_2, LOW);
  digitalWrite(MR_3, LOW);
  digitalWrite(MR_4, LOW);
  analogWrite(ENA_R, 0);
  delay(t);
 }
  
void loop(){ 
   
  lcd.setCursor(0,0);  
  lcd.print("   Helo  ");  
  lcd.setCursor(0,1);  
  lcd.print("  Friend ");  
      
  int x = sonar.ping_cm();
  if (state == READY) {
    if (x < 20) {
      state = KOIN_MASUK1;
      Serial.print("Distance: ");
      Serial.print(x);
      Serial.print(" cm");
      delay(1000);
    }
    else{
      state = KOIN_KELUAR;
      }
  }
  else if (state == KOIN_MASUK1) {
        state = SALDO_1000;
        Serial.println("Balance : Rp1000");
        Serial.print("Distance: ");
        Serial.print(x);
        Serial.print(" cm");
        delay(1000);
  }
  if (state == SALDO_1000) {
    if (x < 20) {
      state = KOIN_MASUK2;
    }
    else{
      state = SALDO_1000; 
      }
  }
  else if (state == KOIN_MASUK2) {
      state = SALDO_2000;
      Serial.println("Balance : Rp2000");
  }
  else if (state == KOIN_KELUAR){
    if(x >= 20){
      
      state = READY;
      Serial.println("Balance : Rp0");
      }
  }
  
  WiFiClient client = server.available();   

  if (client) {                             
    Serial.println("New Client.");          
    String currentLine = "";                
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();         
      if (client.available()) {            
        char c = client.read();             
        Serial.write(c);                   
        header += c;
        if (c == '\n') {                  

          if (currentLine.length() == 0) {

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /item2") >= 0) {
              if(state == SALDO_1000){
                Serial.println("Thank You");
                state = KOIN_KELUAR;
                item2(820);
                brake(4000);
                }
              else if(state == SALDO_2000){
                Serial.println("Thank You");
                state = SALDO_1000;
                item2(820);
                brake(4000);



                }
              else{
                brake(4000);
                }

            }
            if (header.indexOf("GET /item1") >= 0) {
              if(state == SALDO_2000){
                Serial.println("Thank You");
                state = KOIN_KELUAR;
                item1(820);
                brake(4000);
              }
              else{
                brake(4000);
                }

            }            
            
            // Web Page Heading
            client.println("<body><center><h1>Final Exam IoT Group 6</h1></center><br>");
            client.println("<center><a href=\"/item1\"> Item 1 : Rp 2000 </a><br></center>");
            client.println("<center><a href=\"/item2\"> Item 2 : Rp 1000 </a></center");
            client.println("</body></html>");
            
            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;     
        }
      }
    }

    header = "";

    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
