/****************************************************/
/* This is only one example of code structure       */
/* OFFCOURSE this code can be optimized, but        */
/* the idea is let it so simple to be easy catch    */
/* where can do changes and look to the results     */
/****************************************************/
//set your clock speed
//#define F_CPU 16000000UL
//these are the include files. They are outside the project folder
//#include <avr/io.h>
//#include <iom1284p.h>
// #include <util/delay.h>
// #include <avr/interrupt.h>
#define VFD_in 7// If 0 write LCD, if 1 read of LCD
#define VFD_clk 8 // if 0 is a command, if 1 is a data0
#define VFD_stb 9 // Must be pulsed to LCD fetch data of bus

#define BUTTON_PIN 2 //Att check wich pins accept interrupts... Uno is 2 & 3
volatile byte buttonReleased = false;

unsigned int segments[] ={
  // Here I'm forced to use the "0" as 10, because the 7 segments sart in "1"
  // This table is inverted
  //    This not respect the table for 7 segm like "abcdefgh"  //
  //    .decgfba         
      0b01110111,      //0 
      0b00010010,      //1
      0b01101011,      //2
      0b01011011,      //3 
      0b00011110,      //4 
      0b01011101,      //5 
      0b01111101,      //6 
      0b00010011,      //7 
      0b01111111,      //8
      0b00011111,      //9 
      0b00000000       //10 // empty display
  };



void MN12510F_init(void){
    //Commands: 000-AAAA  WRITE INSTRUCTION: COMMAND = “000”  //(-) don't matter
    //Commands: 010-AAAA  TWO-BYTE WRITE INSTRUCTION: COMMAND = “010” //(-) don't matter
    //Commands: 011-AAAA  BYTE READ INSTRUCTION: COMMAND = “011” 
    //Commands: 100-AAAA  CONTINUOUS TRANSMISSION/RECEPTION INSTRUCTION: COMMAND= “100”

    //Register: VFD DISPLAY CONTROL REGISTER (VFDCNT): OO (HEX)
    //Register: DGT SIGNAL DUTY AND DGT DIGITS CONTROL REGISTER (DGTCNT): 01 (HEX)
    //Register: SG DIGITS CONTROL REGISTER (SGCNT): 02 (HEX) 
    //Register: PORT 0/1 OUTPUT SELECT REGISTER (P01CN1): 03 (HEX)
    //Register: PORT 2 DIRECTION CONTROL REGISTER (P2DIR): 04 (HEX) 
    //Register: PORT 2 OUTPUT SELECT REGISTER (P2CNT):05 (HEX)
    //Register: PORT 3 DIRECTION CONTROL REGISTER (P3DIR): 06 (HEX)
    //Register: PORT 3 INPUT SELECT REGISTER (P3CNT): 07 (HEX)
    //Register: KEY SCAN CONTROL REGISTER (KEYCNT): 08 (HEX) 
    //Register: RESET REGISTER (RESET): 09 (HEX)
    //Register: PORT 0/1 OUTPUT DATA REGISTER (P01DATA): 0C (HEX) 
    //Register: PORT 2 DATA REGISTER (P2DATA): 0D (HEX) 
    //Register: PORT 3 DATA REGISTER (P3DATA): 0E (HEX) 

    delayMicroseconds(200); //power_up delay
    // Note: Allways the first byte in the input data after the STB go to LOW is interpret as command!!!
    // Configure VFD display (grids)

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001001); cmd_without_stb(0b00000001); //0x09 Register Reset Bit "0" = 1: Reset, 0: Cancel reset
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4); 

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001000); cmd_without_stb(0b00001001); //0x08 Disable timming Key scan, 
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4); 

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00000000); cmd_without_stb(0b00000001); //0x00 Bit 0 set display to on and bit 3: all seg on!!!
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4); 
    
    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00000001); cmd_without_stb(0b10011001); //0x01 4 bits at lefte to 10 Grids, 4 bits at right to dutty cycle 10/16
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);
    
    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00000010); cmd_without_stb(0b00001110); //0x02 SG DIGITS CONTROL REGISTER (SGCNT): 02 (HEX) Set of number segments!
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00000011); cmd_without_stb(0b11111111); //0x03 PORT 0/1 OUTPUT SELECT REGISTER (P01CN1): 03 (HEX)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00000100); cmd_without_stb(0b00001111); //0x04 PORT 2 DIRECTION CONTROL REGISTER (P2DIR): 04 (HEX)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00000101); cmd_without_stb(0b00001110); //0x05 PORT 2 OUTPUT SELECT REGISTER (P2CNT):05 (HEX) 
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00000110); cmd_without_stb(0b00011111); //0x06 PORT 3 DIRECTION CONTROL REGISTER (P3DIR): 06 (HEX) 
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001100); cmd_without_stb(0b11111111); //0x0C PORT 0/1 OUTPUT DATA REGISTER (P01DATA): 0C (HEX)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001101); cmd_without_stb(0b00001111); //0x0D PORT 2 DATA REGISTER (P2DATA): 0D (HEX) (LED port 20. Status: 1=on, 0=off)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001110); cmd_without_stb(0b00011111); //0x0E PORT 3 DATA REGISTER (P3DATA): 0E (HEX) (LED's port 33 & 34. Status: 1=on, 0=off)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);
  
}
void cmd_without_stb(unsigned char a){
  // send without stb
  unsigned char data = 0x00; //value to transmit, binary 10101010
  unsigned char mask = 0x80; //our bitmask
  
  data=a;
  //This don't send the strobe signal, to be used in burst data send
   for (mask = 0B10000000; mask > 0; mask >>= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_in, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_in, LOW);
     }
    delayMicroseconds(5);
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(5);
   }
}
void setup() {
    // put your setup code here, to run once:
    // initialize digital pin LED_BUILTIN as an output.
    
    Serial.begin(115200);

 cli();           // disable all interrupts
    
    pinMode(VFD_in, OUTPUT);
    pinMode(VFD_clk, OUTPUT);
    pinMode(VFD_stb, OUTPUT);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),
                  buttonReleasedInterrupt,
                  FALLING);
  // initialize timer1 
 

  MN12510F_init();
  //only here I active the enable of interrupts to allow run the test of VFD
  //interrupts();             // enable all interrupts
  sei();
}
void msgHiPeople (void){
  //Here I let the sequence of all Digits, you can skeep the first 5 grids send 10 bytes follwed filled of 0x00.
                  digitalWrite(VFD_stb, LOW);
                  delayMicroseconds(40);
                  //
                  cmd_without_stb(0b01000000); //cmd 3 wich define the start address (00H to 15H)
                  //Grid 0x00
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                   //Grid 0x01
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                   //Grid 0x02
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                   //Grid 0x03
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                   //Grid 0x04
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits 
                   //Grid 0x05
                        cmd_without_stb(0b00001110); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b01001111); // Data to fill table 5*16 = 80 bits 
                    //Grid 0x06
                        cmd_without_stb(0b01111110); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b01100111); // Data to fill table 5*16 = 80 bits
                    //Grid 0x07
                        cmd_without_stb(0b01100111); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b01001111); // Data to fill table 5*16 = 80 bits 
                    //Grid 0x08
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits 
                    //Grid 0x09
                        cmd_without_stb(0b00110111); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000110); // Data to fill table 5*16 = 80 bits
      
                  digitalWrite(VFD_stb, HIGH);  
                  delayMicroseconds(40);
}
void clear (void){
  for(uint8_t g = 0x00; g < 16; g++){
                for (int s = 0x00; s < 16; s++){   
                  digitalWrite(VFD_stb, LOW);
                  delayMicroseconds(4);
                  //
                  cmd_without_stb((0b01000000 | g )); //cmd 3 wich define the start address (00H to 15H)
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        delay(1);
                  digitalWrite(VFD_stb, HIGH);  
                }
    }          
}
void findSegments (void){
  uint8_t m = 0x00;
  uint8_t grid = 0x00;
  //       
  for(uint8_t g = 0x00; g < 16; g++){
    m=0x00;
                for (int s = 0x00; s < 16; s++){
                    // This cycle while is used to controle button advance segments test!
                   while(1){
                            if(!buttonReleased){
                              delay(200);
                            }
                            else{
                              delay(15);
                               buttonReleased = false;
                               break;
                               }
                         }
                  digitalWrite(VFD_stb, LOW);
                  delayMicroseconds(4);
                  //
                  cmd_without_stb((0b01000000 | g )); //cmd 3 wich define the start address (00H to 15H)

                      if(s < 8){
                        cmd_without_stb(0b00000001 << s); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                         //Serial.println("s: "); Serial.println(s, HEX);
                      }
                      else{
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000001 << m); // Data to fill table 5*16 = 80 bits
                        m++;
                          //Serial.println("m: "); Serial.println(m, HEX);
                      }
                  delay(1);
                  digitalWrite(VFD_stb, HIGH);
                  //
                  Serial.print("grid:  ");
                  Serial.print(grid, DEC);
                  //seg++;
                  Serial.print(";   Segment:  ");
                  Serial.println(s, DEC);
                  
                  delay(50);
                }
                grid++;     
  }           
}
void testing (void){
    uint8_t m = 0x00;
    unsigned char val = 0x00;
    digitalWrite(VFD_stb, LOW);
    delay(2);
                  
      cmd_without_stb(0x40); //cmd 3 wich define the start address (00H to 15H)
            for(uint8_t g = 0x00; g < 16; g++){
                          cmd_without_stb(0b11111111); // Data to fill table 5*16 = 80 bits
                          delay(2);
                          cmd_without_stb(0b11111111); // Data to fill table 5*16 = 80 bits
                          delay(2);
            }     
          digitalWrite(VFD_stb, HIGH);
          delay(20);
          //  digitalWrite(VFD_stb, LOW);
          //  delay(2);                             
}
void ctrlLEDport20(void){
    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001101); cmd_without_stb(0b00001110); //0x0D PORT 2 DATA REGISTER (P2DATA): 0D (HEX) (LED port 20. Status: 1=on, 0=off)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);
    delay(500);

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001101); cmd_without_stb(0b00001111); //0x0D PORT 2 DATA REGISTER (P2DATA): 0D (HEX) (LED port 20. Status: 1=on, 0=off)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);
    delay(500);
}
void ctrlLEDport33(void){
    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001110); cmd_without_stb(0b00001111); //0x0E PORT 3 DATA REGISTER (P3DATA): 0E (HEX) (LED's port 33 & 34. Status: 1=on, 0=off)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);
    delay(500);

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001110); cmd_without_stb(0b00011111); //0x0E PORT 3 DATA REGISTER (P3DATA): 0E (HEX) (LED's port 33 & 34. Status: 1=on, 0=off)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);
    delay(500);
}
void ctrlLEDport34(void){
    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001110); cmd_without_stb(0b00010111); //0x0E PORT 3 DATA REGISTER (P3DATA): 0E (HEX) (LED's port 33 & 34. Status: 1=on, 0=off)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);
    delay(500);

    digitalWrite(VFD_stb, LOW); delayMicroseconds(4);
    cmd_without_stb(0b00001110); cmd_without_stb(0b00011111); //0x0E PORT 3 DATA REGISTER (P3DATA): 0E (HEX) (LED's port 33 & 34. Status: 1=on, 0=off)
    digitalWrite(VFD_stb, HIGH); delayMicroseconds(4);
    delay(500);
}
void loop() {
  for(uint8_t j = 0x00; j< 4; j++){
      clear();
      delay(500);
      msgHiPeople();
      delay(500);
  }
          for(uint8_t i = 0x00; i < 4; i++){
            ctrlLEDport20();
            ctrlLEDport33();
            ctrlLEDport34();
          }
      clear();
      findSegments();
      delay(500);
}
void buttonReleasedInterrupt() {
  buttonReleased = true; // This is the line of interrupt button to advance one step on the search of segments!
}

// ISR(TIMER1_COMPA_vect)   {  //This is the interrupt request
//                             // https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
//       secs++;
// } 
