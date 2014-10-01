/* 
ableform_driver_v3   author: Ted Moallem  date: September 2014;
ableform_driver_v1   author: Ted Moallem  date: December 2013; _v2, March 2014
based on 
BRL4ALL_dot_stepperX, Ted Moallem, July 2011
  //multi-button checker with debouncingmulti-button checker with debouncingmulti-button checker with debouncing
  //http:// www.adafruit.com/blog/2009/10/20/example-code-for-multi-button-checker-with-debouncing/
  // also see http:// www.gammon.com.au/forum/?id=11955   regarding debouncing, pullup/pulldown, etc.
*/

#include <AFMotor.h>
#include <Servo.h> 

#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty

//byte buttons[] = {14, 15, 16, 17, 18, 19}; // the analog 0-5 pins are also known as digital 14-19
//#define NUMBUTTONS sizeof(buttons)
#define NUMBUTTONS 6
// we will track if a button is just pressed, just released, or 'currently pressed' 
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS], brlarr[NUMBUTTONS];
byte Btrig = 0;

#define NUMaPINS 6   //number of analog input pins

//AF_Stepper motor(StepsPerRev, MotorshieldPort#);
AF_Stepper feed(48, 2);
AF_Stepper rover(48, 1);
Servo UDcam;
Servo BRLpin;

//emboss servo
int Vhome = 15; //100; //20; //57;
int Vdown = 95; //10; //75; //27; //17;

//dot servo
int Bhome = 130;  //145;     //MUST BE >= 6*Bdelta
int Bdelta = 20; //18;   //shift ~2.5mm 

int spA = 4 * 1;  //Intra-cell spacing
int spB = 6 * 1;  //Inter-cell spacing
int spFd = 70;    //Inter-line spacing

int dlyA = 300; //delay, motor to remain in transient position
int dlyB = 350; //

int iByte = 0;	// for incoming serial data


/* ///////// SETUP SETUP SETUP SETUP SETUP SETUP SETUP ///////// */
void setup() {
  byte i;

  Serial.begin(9600); 
  Serial.println("ableform tactile printer, awake!!");
  feed.setSpeed(60);  // motor speed (rpm)
  rover.setSpeed(100);  // motor speed (rpm)  //SINGLE,DOUBLE,INTERLEAVE,MICROSTEP

  UDcam.attach(9);  // SERVO2 on motorshield;  attaches the servo on pin 9 to the servo object 
  UDcam.write(Vhome);
  BRLpin.attach(10);  // SER1 on motorshield;  attaches the servo on pin 10 to the servo object  
  BRLpin.write(Bhome);

  /*  // Make input & enable pull-up resistors on switch pins
  for (i=0; i< NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], HIGH);
  } */

}   //END SETUP() 


/* ///////// LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP ///////// */
void loop() {

  // BUTTONS BUTTONS BUTTONS BUTTONS BUTTONS BUTTONS BUTTONS 
  check_switches();      // when we check the switches we'll get the current state
  
  for (byte i = 0; i < NUMBUTTONS; i++) {
    if (justpressed[i]) {
     //Serial.println(" Just pressed"); 
      // remember, check_switches() will CLEAR the 'just pressed' flag
      brlarr[i] = 1;
      Btrig = 1;
    }
    if (justreleased[i]) {
      //Serial.println(" Just released");
      // remember, check_switches() will CLEAR the 'just pressed' flag
      if (Btrig == 1) {
        trigDots(brlarr);
      }
      Btrig = 0;
    }
    if (pressed[i]) {
      //Serial.print(i, DEC);
      //Serial.println(" pressed");
      // is the button pressed down at this moment
    }
  }  
  
  //MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS 
  
  if (Serial.available() > 0) {
    // read the incoming byte:
    iByte = Serial.read();
    // say what you got:
    Serial.print("I received: ");
    Serial.println(iByte, DEC);

    if (iByte=='b') {                //'f'=102; 'b'=98
      feed.step(spFd,FORWARD,DOUBLE);
      feed.release();
    }
    
    if (iByte == 'f') {                //
      feed.step(spFd,BACKWARD,DOUBLE);
      feed.release();
    }
    

    if (iByte == '-') {                //
      Vdown = Vdown - 1;
      Serial.println(Vdown);
    }
    if (iByte == '=') {                //
      Vdown = Vdown + 1;
      Serial.println(Vdown);
    }
    
    
    
    
/*    
    //MANUAL CALIBRATION CODE
    
    if (iByte==49) {              //1
      BRLpin.write(Bhome);
      delay(dlyB);      
    }
    if (iByte==50) {              //2
      BRLpin.write(Bhome-1*Bdelta);
      delay(dlyB);      
    }
    if (iByte==51) {              //3
      BRLpin.write(Bhome-2*Bdelta);
      delay(dlyB);      
    }
    if (iByte==52) {              //4
      BRLpin.write(Bhome-3*Bdelta);
      delay(dlyB);      
    }
    if (iByte==53) {              //5
      BRLpin.write(Bhome-4*Bdelta);
      delay(dlyB);      
    }
    if (iByte==54) {              //6
      BRLpin.write(Bhome-5*Bdelta);
      delay(dlyB);      
    }
    if (iByte==55) {              //7
      BRLpin.write(Bhome-6*Bdelta);
      delay(dlyB);      
    }
    if (iByte==56) {              //8
      BRLpin.write(Bhome-7*Bdelta);
      delay(dlyB);      
    }
    if (iByte==57) {              //9
      BRLpin.write(Bhome-8*Bdelta);
      delay(dlyB);      
    }
    if (iByte==48) {              //0
      BRLpin.write(Bhome-9*Bdelta);
      delay(dlyB);      
    }
    
      
    if (iByte==113) {              //q
      UDcam.write(Vdown);
      delay(dlyA);
      UDcam.write(Vhome);
      delay(dlyB);
      rover.step(spA,FORWARD,MICROSTEP);
      rover.release();
      delay(dlyB);
      UDcam.write(Vdown);
      delay(dlyA);
      UDcam.write(Vhome);
      delay(dlyB);
      rover.step(spB,FORWARD,MICROSTEP);
      rover.release();
      delay(dlyB);
      
    }
    else if (iByte==97) {              //a
      BRLpin.write(Bhome+10);
      UDcam.write(Vdown);
      delay(dlyA);
      UDcam.write(Vhome);
      delay(dlyA);
      
    }
    else if (iByte==122) {              //z
      BRLpin.write(Bhome+30);
      UDcam.write(Vdown);
      delay(dlyA);
      UDcam.write(Vhome);
      delay(dlyA);
      
    }
    else if (iByte==119) {              //w
      BRLpin.write(Bhome+50);
      UDcam.write(Vdown);
      delay(dlyA);
      UDcam.write(Vhome);
      delay(dlyA);
      
    }
    else if (iByte==115) {              //s
      BRLpin.write(Bhome+70);
      UDcam.write(Vdown);
      delay(dlyA);
      UDcam.write(Vhome);
      delay(dlyA);
      
    }
    else if (iByte==120) {              //x
      BRLpin.write(Bhome+90);
      UDcam.write(Vdown);
      delay(dlyA);
      UDcam.write(Vhome);
      delay(dlyA);   
    }
*/


  }
  
 
 
}  // END LOOP()




void check_switches()
{
  
  
    static int prevPINvalue[NUMaPINS];
    static int currPINvalue[NUMaPINS];
    static byte previousstate[NUMBUTTONS];
    static byte currentstate[NUMBUTTONS];
    static long lasttime;
    byte index, i, j;

    if (millis() < lasttime) {    // we wrapped around, lets just try again
       lasttime = millis();
    }
    if ((lasttime + DEBOUNCE) > millis()) {    // not enough time has passed to debounce
      return; 
    }
    lasttime = millis();  // ok we have waited DEBOUNCE milliseconds, lets reset the timer


    //Read the analog pin values, and infer button states

    for (i = 0; i < NUMaPINS; i++) {
        currPINvalue[i] = analogRead(i);   // read analog pin

        switch(i) {
            case 0:    //set buttons 3 and 2              
                if (currPINvalue[i] > 815) {   
                  currentstate[0] = LOW;  //NOTE: inkeeping with subsequent code, currentstate LOW indicates button pressed, HIGH indicates not pressed
                  currentstate[1] = LOW;
                } else if (currPINvalue[i] > 700) {
                  currentstate[0] = LOW;  
                  currentstate[1] = HIGH;
                } else if (currPINvalue[i] > 500) {
                  currentstate[0] = HIGH;  
                  currentstate[1] = LOW;
                } else {
                  currentstate[0] = HIGH;  
                  currentstate[1] = HIGH;                
                }
                break;
            case 1:    //set buttons 1 and 4
                if (currPINvalue[i] > 815) {   
                  currentstate[2] = LOW;  //NOTE: inkeeping with subsequent code, currentstate LOW indicates button pressed, HIGH indicates not pressed
                  currentstate[3] = LOW;
                } else if (currPINvalue[i] > 700) {
                  currentstate[2] = LOW;  
                  currentstate[3] = HIGH;
                } else if (currPINvalue[i] > 500) {
                  currentstate[2] = HIGH;  
                  currentstate[3] = LOW;
                } else {
                  currentstate[2] = HIGH;  
                  currentstate[3] = HIGH;                
                }
                break;
            case 2:    //set buttons 5 and 6
                if (currPINvalue[i] > 815) {   
                  currentstate[4] = LOW;  //NOTE: inkeeping with subsequent code, currentstate LOW indicates button pressed, HIGH indicates not pressed
                  currentstate[5] = LOW;
                } else if (currPINvalue[i] > 700) {
                  currentstate[4] = LOW;  
                  currentstate[5] = HIGH;
                } else if (currPINvalue[i] > 500) {
                  currentstate[4] = HIGH;  
                  currentstate[5] = LOW;
                } else {
                  currentstate[4] = HIGH;  
                  currentstate[5] = HIGH;                
                }                
                break;
            case 3:    //set <space> or <CR>
                
                break;
            case 4:    //set <backspace> or <revCR>
                
                break;
            case 5:    //set <FWD> or <REV> line feed
                
                break;
            default: 
                break;
        }

        prevPINvalue[i] = currPINvalue[i];  //store previous pin value

    }


  
    for (index = 0; index < NUMBUTTONS; index++) {
        justpressed[index] = 0;       // when we start, we clear out the "just" indicators
        justreleased[index] = 0;
         
//        currentstate[index] = digitalRead(buttons[index]);   // read the button
        
        
        if (currentstate[index] == previousstate[index]) {
            if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
                // just pressed
                justpressed[index] = 1;
            }
            else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
                // just released
                justreleased[index] = 1;
            }
            pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
        }
        //Serial.println(pressed[index], DEC);
        previousstate[index] = currentstate[index];   // keep a running tally of the buttons
    }
}


void trigDots(byte Dbrlarr[]) {
    
    byte column1 = 0;
    byte column2 = 0;
    
  
  
    column1 = Dbrlarr[0]*1 + Dbrlarr[1]*2 + Dbrlarr[2]*4;
    //Serial.print(column1, BIN);
    //Serial.print(' ');  
    column2 = Dbrlarr[5]*1 + Dbrlarr[4]*2 + Dbrlarr[3]*4;
    //Serial.print(column2, BIN);
    //Serial.println(" is BRLchar"); 
  
    for (byte i = 0; i < NUMBUTTONS; i++) {
      Serial.print(Dbrlarr[i]);  
      Serial.print(' ');  
      brlarr[i] = 0;
    }
    Serial.println(" dots selected"); 
      
    makeDots(column1);  //first column of Braille cell
    delay(dlyA);
    rover.step(spA,BACKWARD,MICROSTEP);
    rover.release();
    //delay(dlyB);
    
    makeDots(column2);  //second column of Braille cell
    delay(dlyA);
    rover.step(spB,BACKWARD,MICROSTEP);
    rover.release();
    //delay(dlyB);
    
    //feed.step(15,FORWARD,MICROSTEP);
    //feed.release();
    //delay(dlyB);
   
   
   
  
}


void makeDots(byte Dcol) {
  
    int Btgt = Bhome;
    
    if (Dcol != 0) {   
        switch (Dcol) {
          case 1:  //001
            Btgt = Bhome;
            break;
          case 3:  //011
            Btgt = Bhome - 1*Bdelta;
            break;
          case 7:  //111
            Btgt = Bhome - 2*Bdelta;
            break;
          case 6:  //110
            Btgt = Bhome - 3*Bdelta;
            break;
          case 5:  //101
            Btgt = Bhome - 4*Bdelta;
            break;
          case 2:  //010
            Btgt = Bhome - 5*Bdelta;
            break;
          case 4:  //100
            Btgt = Bhome - 6*Bdelta;
            break;
          default: 
            Serial.println("WTF, Dcol??"); 
        }   
        
        BRLpin.write(Btgt);
        delay(dlyA);            
        UDcam.write(Vdown);
        delay(dlyB);
        UDcam.write(Vhome);
    }  
    
}


