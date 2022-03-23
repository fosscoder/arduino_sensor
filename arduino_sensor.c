/*
 Created by Rui Santos
 Visit: http://randomnerdtutorials.com for more arduino projects

 Arduino with Ethernet Shield
 */
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>
// #include <Servo.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define ONE_WIRE_BUS_INT 4

const int analogPin =A0;
float inputP1;
float outputP1;
float flow;
float returnt;
float offsetV = 102;
int raw;
float voltage;
float pressure;

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
OneWire oneWireInt(ONE_WIRE_BUS_INT);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DallasTemperature sensors2(&oneWireInt);

int relay1 = A3;
int relay2 = A4;
int led = 4;
//Servo microservo; 
int pos = 0; 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 1, 111 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 1, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(8111);                             //server port     
String readString;

void setup() {

  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("MyBoiler Monitoring");
  // Start up the library
  sensors.begin();
  sensors2.begin();
  Serial.println();



  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  
  
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(led, OUTPUT);
  //microservo.attach(7);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {

  // put your main code here, to run repeatedly:

  //DIFFERENTIAL TEMPERATURE SECTION
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  sensors2.requestTemperatures();
  //Serial.println("DONE");

  Serial.print("Flow Temp:       ");
  flow = sensors.getTempCByIndex(0);
  Serial.print(flow);
  Serial.println(" °C");
  
  //Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? 
    // You can have more than one IC on the same bus. 
    // 0 refers to the first IC on the wire

  Serial.print("Return Temp:     ");
  returnt = sensors.getTempCByIndex(1);
  Serial.print(returnt);
  Serial.println(" °C");
  //Serial.println(sensors.getTempCByIndex(1));  

  //Serial.print("Temperature for Device Int is: ");
  //Serial.print(sensors2.getTempCByIndex(0));

  Serial.print("Differential     ");
  Serial.print(flow-returnt);
  Serial.println("  °C");
   //DIFFERENTIAL TEMPERATURE SECTION END 

//PRESSURE SECTION xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  //https://www.arduino.cc/en/Reference/analogRead

  raw = analogRead(A0); 
  pressure = ((float)raw-offsetV) * 12/(921-offsetV);

  //Serial.print("Voltage is:       ");
  //Serial.println(voltage);
  //pressure = (voltage-0.49) * 51.28;
  Serial.print("Pressure is:     ");
  Serial.print(pressure);
  Serial.println("  BAR");
  //Serial.print("  BAR");
  Serial.println();
  Serial.println();
  Serial.println(raw);
  Serial.println();

//PRESSURE SECTION END   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

  
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {   
      if (client.available()) {
        char c = client.read();
     
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
         }

         //if HTTP request has ended
         if (c == '\n') {          
           Serial.println(readString); //print to serial monitor for debuging
     
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           //
           client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 2");  // refresh the page automatically every 5 sec
           //
           client.println();     
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<TITLE>MyBoiler Remote Monitoring Project Prototype</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>MyBoiler Remote Monitoring Project Prototype</H1>");
           client.println("<hr />");
           client.println("<br />");  
           client.println("<H2>Arduino with Ethernet Shield and Differential Temp and Pressure Monitoring</H2>");
           client.println("<br />");  
           client.println("<a href=\"/?button1on\"\">Relay1 On</a>");
           client.println("<a href=\"/?button1off\"\">Relay1 Off</a><br />");   
           client.println("<br />");     
           client.println("<br />"); 
           client.println("<a href=\"/?button2on\"\">Relay2 On</a>");
           client.println("<a href=\"/?button2off\"\">Relay2 Off</a><br />"); 
           //
           client.println("<br />");
           client.println("Flow Temperature____________");    
           client.println(flow);
           client.println("<br />");
           client.println("Return Temperature__________");  
           client.println(returnt);
           client.println("<br />");
           client.print("Differential__________________");
           client.print(flow-returnt);
           client.println("<br />");
           client.println("Pressure____________________");  
           client.println(pressure);
           client.println("<br />");
           client.println("<br />");
           
           if (flow > 40){
              client.println("Boiler has been running recently");
              client.println("<br />");
               
           }

           if (flow < 30){
            client.println("Boiler is off");
            client.println("<br />");
           }

           if (pressure < 0.5){
            client.println("Pressure is low");
            client.println("<br />");
            
           }
           if (pressure > 0.5 & pressure <2.5){
            client.println("Pressure is Acceptable");
            client.println("<br />");
            
           }
           if (pressure > 2.5){
            client.println("Pressure is too High");
            client.println("<br />");
            
           }
           
           //
           //client.println("<p>Created by Rui Santos. Visit http://randomnerdtutorials.com for more projects!</p>");  
           client.println("<br />"); 
           //client.println("<iframe width='900' height='800' frameborder='0' scrolling='no' src='//plot.ly/~myboiler/6.embed'></iframe>");
           client.println("</BODY>");
           client.println("</HTML>");
     
           delay(1);
           //stopping client
           client.stop();
           //controls the Arduino if you press the buttons
           if (readString.indexOf("?button1on") >0){
               //digitalWrite(led, HIGH);
               digitalWrite(relay1, LOW);
           }
           if (readString.indexOf("?button1off") >0){
               digitalWrite(relay1, HIGH);
           }
           if (readString.indexOf("?button2on") >0){
               digitalWrite(relay2, LOW);
           }
           if (readString.indexOf("?button2off") >0){
                for(pos = 180; pos>=1; pos-=3)     // goes from 180 degrees to 0 degrees 
               digitalWrite(relay2, HIGH);
           }
            //clearing string for next read
            readString="";  
           
         }
       }
    }
}
delay(1000); 


}
