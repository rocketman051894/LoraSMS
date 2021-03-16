#include <SPI.h>
#include <RFM69.h>         //get it here: https://www.github.com/lowpowerlab/rfm69
#include <RFM69_ATC.h>     //get it here: https://github.com/lowpowerlab/RFM69
#include <LowPower.h>

#define NETWORKID     100  //the same on all nodes that talk to each other
#define RECEIVER      1    //unique ID of the gateway/receiver
#define SENDER        2
#define NODEID        SENDER  //change to "SENDER" if this is the sender node (the one with the button)
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW_HCW
//#define ENABLE_ATC      //comment out this line to disable AUTO TRANSMISSION CONTROL
#define ATC_RSSI        -75
//*********************************************************************************************

String BluetoothData = "";
int LED = A1;
int loraCE = A0;
int loraReset = 2;

#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio(loraCE, 5, true);
#endif


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);
  Serial.begin(115200);

   radio.initialize(FREQUENCY,NODEID,NETWORKID);
#ifdef IS_RFM69HW_HCW
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
#endif
  radio.encrypt(ENCRYPTKEY);
  
#ifdef ENABLE_ATC
  radio.enableAutoPower(ATC_RSSI);
#endif

  pinMode(loraCE, OUTPUT);
  pinMode(loraReset, OUTPUT);
  digitalWrite(loraCE, HIGH);
  digitalWrite(loraReset, LOW);
  
}

// the loop function runs over and over again forever
void loop() {
//  if (Serial.available()) {
//    BluetoothData = Serial.readString();

//    if(BluetoothData.indexOf("r") != -1){
      //digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)         
      Serial.print("Can Send? ");
      Serial.println(radio.canSend());
      Serial.print("RSSI: ");
      Serial.println(radio.readRSSI());
      radio.sendWithRetry(RECEIVER, "Hi", 2);//target node Id, message as string or byte array, message length
      Serial.println("HI Sent...");      
//    }

    if(BluetoothData.indexOf("w") != -1){
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);    // turn the LED off by making 
      Serial.println("OFF");
    }

    BluetoothData = "";
//  }

  if (radio.receiveDone())
  {
    //print message received to serial
    Serial.print('[');Serial.print(radio.SENDERID);Serial.print("] ");
    Serial.print((char*)radio.DATA);
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
    Serial.println();
    
    //check if received message is 2 bytes long, and check if the message is specifically "Hi"
    if (radio.DATALEN==2 && radio.DATA[0]=='H' && radio.DATA[1]=='i')
    {
      digitalWrite(LED, HIGH);
    }
   
    //check if sender wanted an ACK
    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.print(" - ACK sent");
    }
  }
  
  radio.receiveDone(); //put radio in RX mode
  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
  
  //delay(10);
}
