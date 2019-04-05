
#include <Stepper.h>

const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

//clipping indicator variables
boolean clipping = 0;
char input='7';
int mycount=5000;
float basefreq;
//data storage variables
byte newData = 0;
byte prevData = 0;
unsigned int time = 0;//keeps time and sends vales to store in timer[] occasionally
int timer[10];//storage for timing of events
int slope[10];//storage for slope of events
unsigned int totalTimer;//used to calculate period
unsigned int period;//storage for period of wave
byte index = 0;//current storage index
float frequency;//storage for frequency calculations
int maxSlope = 0;//used to calculate max slope as trigger point
int newSlope;//storage for incoming slope data

//variables for decided whether you have a match
byte noMatch = 0;//counts how many non-matches you've received to reset variables if it's been too long
byte slopeTol = 100;//slope tolerance- adjust this if you need
int timerTol = 10;//timer tolerance- adjust this if you need

//variables for amp detection
unsigned int ampTimer = 0;
byte maxAmp = 0;
byte checkMaxAmp;
byte ampThreshold = 30;//raise if you have a very noisy signal

void setup(){
  
  Serial.begin(9600);
  myStepper.setSpeed(60);
  pinMode(13,OUTPUT);//led indicator pin
  pinMode(12,OUTPUT);//output pin
  
  cli();//diable interrupts
  
  //set up continuous sampling of analog pin 0 at 38.5kHz
 
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  
  sei();//enable interrupts
}

ISR(ADC_vect) {//when new ADC value ready
  
  PORTB &= B11101111;//set pin 12 low
  prevData = newData;//store previous value
  newData = ADCH;//get value from A0
  if (prevData < 127 && newData >=127){//if increasing and crossing midpoint
    newSlope = newData - prevData;//calculate slope
    if (abs(newSlope-maxSlope)<slopeTol){//if slopes are ==
      //record new data and reset time
      slope[index] = newSlope;
      timer[index] = time;
      time = 0;
      if (index == 0){//new max slope just reset
        PORTB |= B00010000;//set pin 12 high
        noMatch = 0;
        index++;//increment index
      }
      else if (abs(timer[0]-timer[index])<timerTol && abs(slope[0]-newSlope)<slopeTol){//if timer duration and slopes match
        //sum timer values
        totalTimer = 0;
        for (byte i=0;i<index;i++){
          totalTimer+=timer[i];
        }
        period = totalTimer;//set period
        //reset new zero index values to compare with
        timer[0] = timer[index];
        slope[0] = slope[index];
        index = 1;//set index to 1
        PORTB |= B00010000;//set pin 12 high
        noMatch = 0;
      }
      else{//crossing midpoint but not match
        index++;//increment index
        if (index > 9){
          reset();
        }
      }
    }
    else if (newSlope>maxSlope){//if new slope is much larger than max slope
      maxSlope = newSlope;
      time = 0;//reset clock
      noMatch = 0;
      index = 0;//reset index
    }
    else{//slope not steep enough
      noMatch++;//increment no match counter
      if (noMatch>9){
        reset();
      }
    }
  }
    
  if (newData == 0 || newData == 1023){//if clipping
    clipping = 1;//currently clipping
    Serial.println("clipping");
  }
  
  time++;//increment timer at rate of 38.5kHz
  
  ampTimer++;//increment amplitude timer
  if (abs(127-ADCH)>maxAmp){
    maxAmp = abs(127-ADCH);
  }
  if (ampTimer==1000){
    ampTimer = 0;
    checkMaxAmp = maxAmp;
    maxAmp = 0;
  }
  
}

void reset(){//clean out some variables
  index = 0;//reset index
  noMatch = 0;//reset match couner
  maxSlope = 0;//reset slope
}


void checkClipping(){//manage clipping indication
  if (clipping){//if currently clipping
    clipping = 0;
  }
}

void clockwise() {
    myStepper.step(-25);
}
void anticlockwise() {
    myStepper.step(25);
}


void MatchE2(){
  if(frequency>70&&frequency<90){
      if(abs(frequency-82.4)<3){Serial.print("In Tune:");Serial.println(frequency);}
      else if(frequency<82.4){Serial.print("Tune UP:");Serial.println(frequency);clockwise();}
      else if(frequency>82.4){Serial.print("Tune DOWN:");Serial.println(frequency);anticlockwise();}
    }
   }
void MatchA(){
  if(frequency>100&&frequency<120){
      if(abs(frequency-110)<3){Serial.print("In Tune:");Serial.println(frequency);}
      else if(frequency<110){Serial.print("Tune UP:");Serial.println(frequency);clockwise();}
      else if(frequency>110){Serial.print("Tune DOWN:");Serial.println(frequency);anticlockwise();}
    }
  }
void MatchD(){
  if(frequency>135&&frequency<155){
      if(abs(frequency-146.8)<3){Serial.print("In Tune:");Serial.println(frequency);}
      else if(frequency<146.8){Serial.print("Tune UP:");Serial.println(frequency);clockwise();}
      else if(frequency>146.8){Serial.print("Tune DOWN:");Serial.println(frequency);anticlockwise();}
    }
  }
void MatchG(){
    if(frequency>186&&frequency<205){
      if(abs(frequency-196)<3){Serial.print("In Tune:");Serial.println(frequency);}
      else if(frequency<196){Serial.print("Tune UP:");Serial.println(frequency);clockwise();}
      else if(frequency>196){Serial.print("Tune DOWN:");Serial.println(frequency);anticlockwise();}
    }
    
  }
void MatchB(){
    if(frequency>235&&frequency<255){
      if(abs(frequency-246.9)<3){Serial.print("In Tune:");Serial.println(frequency);}
      else if(frequency<246.9){Serial.print("Tune UP:");Serial.println(frequency);clockwise();}
      else if(frequency>246.9){Serial.print("Tune DOWN:");Serial.println(frequency);anticlockwise();}
    }
    

  }
void MatchE4(){
    if(frequency>320&&frequency<340){
      if(abs(frequency-329.6)<3){Serial.print("In Tune:");Serial.println(frequency);}
      else if(frequency<329.6){Serial.print("Tune UP:");Serial.println(frequency);clockwise();}
      else if(frequency>329.6){Serial.print("Tune DOWN:");Serial.println(frequency);anticlockwise();}
    }
  }           
void learnNote(char Note,float TargetFreq){
     if((frequency>(TargetFreq-10))&&(frequency<(TargetFreq+10))){
      if(abs(frequency-TargetFreq)<5){Serial.println("Correct!Well Done!");}
      else if(frequency<TargetFreq){Serial.println("Wrong! Slide Finger down!");}
      else if(frequency>TargetFreq){Serial.println("Wrong! Slide Finger up!");}
    }        
  }
  
void loop(){
      
          checkClipping(); 
    if (checkMaxAmp>ampThreshold){
      frequency = 38462/float(period);//calculate frequency timer rate/period
          
        }      
     if(Serial.available()>0){
      input = Serial.read();
      Serial.println(input);
      }
      if(frequency>70&&frequency<350){
            switch (input){
              case '1':
                      MatchE2();
                      break;
              case '2':
                      MatchA();
                      break;
              case '3':
                      MatchD();
                      break;
              case '4':
                      MatchG();
                      break;
              case '5':
                      MatchB();
                      break;
              case '6':
                      MatchE4();
                      break;
              case 'c':
                      learnNote('c',130.8);
                      break;
              case 'd':
                      learnNote('d',146.8);
                      break;                                                         
              case 'e':
                      learnNote('e',164.8);
                      break;
              case 'f':
                      learnNote('f',174.6);
                      break;
              case 'g':
                      learnNote('g',196);
                      break;
              case 'a':
                      learnNote('a',220);
                      break;
              case 'b':
                      learnNote('b',246.9);
                      break;                                
              }
        }
      
        
      
    delay(100);
  
}




