
/*
  Title: Bike Computer Source Code
  
  Name: Carl Sagrado
  Student no: X00084403

  
  Current Version 3.5
  
  Previous Versions
  
          - v1.0 - LCD display testing
          - v2.1 - Switch testing
          - v2.2 - display Switch value to LCD
          - v3.0 - Test simulated wheel
          - v3.1 - calculate time difference
          - v3.2 - display time difference to LCD
          - v4.0 - Add timer funcitionality
          - v4.1 - trigger timer using left/right switch
          - v4.2 - start/pause (LEFT Button) and stop/reset (RIGHT Button)
          - v5.0 - Add brightness control 
          - v5.1 - send PWM signal to transistor for Backlight
          - v6.0 - add contrast level display
          - 
 
  Description:
            - To apply all the codes and to fully test bike computer to full working order in time for the DEMO 
            
  
  Hardware Required:
  * MSP430G2 Launchpad
  * DC Power Supply
  * Function Generator
  * Bike Computer Prototype
*/




volatile long currentTime;  //store microprocessor time
volatile long previousTime=0;  //calculated time dependent to to attach interrupt function
int circumference = 2;   //set circumference of the wheel to 2m

boolean right = false, left = false, up = true, down = false, btnSelect = false;
volatile long currentSpeed; //speed  
volatile long Timemillis; //time in millis
volatile long TimeSeconds; //time in seconds
volatile long  elapseMillis = 0;
volatile long  elapse = 0;//elapse time
int t = 0;

double voltage = 0;
int backlight = 0;      // variable for backlight adjustment
int brightValue = 0;    //variable used for display and for the backlight
int contrastValue = 0;  // variable to store the value coming from the potentiometer
int intensity = 0;    //contrast intensity mapped from contrast value


#include <LiquidCrystal.h>  //caling the LCD library

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(P2_0, P2_1,P2_2,P2_3, P2_4, P2_5);

byte pBar[8] = {      //brightness and constrast custom progress bar
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};



void setup() {

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  lcd.createChar(0, pBar);  //custom char
  pinMode(P1_2, OUTPUT);  //set pin 1.2 to be an output pin (backlight)
  pinMode(P1_6, INPUT_PULLUP);  //set pin to input pull up
  attachInterrupt(P1_6, wheel, RISING);  //using attach interrupt function at the rising edge of the signal
}

void loop() {

  int sensorValue = analogRead(A4);  //Reads analog value from P1_4 (reads condition of the SWITCHES when pressed)
  voltage = sensorValue*0.00322265;
  contrastValue = analogRead(A1);  //Reads analog value from P1_1 (From POT to LCD that adjusts CONTRAST)
  analogWrite(P1_2,backlight); //Brightness
  // buttons();          //listen to the button clicks
  currentTime = millis();
  MenuSelection();  //calling the menu selection function
  if (btnSelect==false){  //MENU 1 *DEFAULT DISPLAY"*
    lcd.clear();  //clear the screen
    lcd.setCursor(0,0);  //default/home cursor
    lcd.print("Speed: ");  //display title
    lcd.print(currentSpeed);  //display current speed in the LCD
    lcd.print("m/s");  //add unit to speed
    lcd.setCursor(0,1);  //set the cursor to next row
    lcd.print("Elapse: ");  //display title
    lcd.print(elapse);  //display the elapse time
    lcd.print("s");  //unit in seconds
    Timer();    //Function call for the Timer 134
    delay(239); //delay to clear and display information to the LCD (This delay is related to the timer
    //Taking into consideration the delay given to the system (timer also takes the toll of the delay) therefore, by rationalising the delay time, it would output desired ratio of the seconds of the timer 

  } 
  else {            //MENU 2 *Display adjust menu option (Brightness or Contrast)

    if (up==true){ 

      Brightness();    //call the brightness function
      if (voltage>=2.60 && voltage <=2.65){ //detects when btn right is pressed
        brightValue  = btnAdd(brightValue);   //increment brightness value
      } 
      else if (voltage>=2.79 && voltage <=2.85){ //detects when btn left is pressed
        brightValue = btnSubtract(brightValue);  //decrement brightness value
      }
    }
    else{
      Contrast(); //call the contrast function

    }

    if (voltage>=2.91 && voltage <=2.94){  //detects when btn down is pressed
      up=false; //set 'up' to false to change menu option (go to contrast)
    } 
    else if (voltage>=2.99 && voltage <=3.10){  //detects when btn up is pressed
      up=true;   //set 'up' to true to change menu option (go to brightness)
    } 

  }
  TimeSeconds = Timemillis*0.001;  //convert millisecond to second (1/1000)
  currentSpeed = circumference/TimeSeconds;  //calculate velocity by distance over time

}


boolean buttonStatus (boolean x){  //invert boolean value of the button when called

  if (x==true){  //check if value is true
    x = false;  //if true, change value to false
  } 
  else {        //check if value is true
    x = true;  //if true, change value to false
  }
  //  delay(250); 
  return x;  //return a boolean
}

int btnAdd (int x){  // button increment
  if (x>=10){    //if the value exceed 10 or equal to 10
    x = 10;      //by default, value is 10
  } 
  else {
    x++;       //otherwise, increment the value
  }
  return x;   //return an integer value not greater than 10

}

int btnSubtract (int x){  //button decrement
  if (x>1){  //if the value is greater than 1
    x--; //proceed to subtract by 1
  } 
  else {
    x = 1;  //lowest value is 1
  }
  return x;   //returnan integer value
}

int buttonCounter (int x){
  if (x>10){
    x = 0;
  } 
  else {
    x++; 
  }
  return x; 
}

void MenuSelection(){
  if (voltage>=1.95 && voltage <=2.00){   //detects when btn select is pressed
    btnSelect = buttonStatus(btnSelect);  //invert button select value
  }
}


void Brightness(){


  lcd.clear();   //clear the display
  lcd.setCursor(0,0); //set cursor to row 0, column 0
  lcd.print(" LCD Brightness");  //display title (move text to middle using space
  lcd.setCursor(0,1);   //set the cursor to row 1, column 0
  backlight=map(brightValue, 0, 10,0, 255); //set the cursor row 1, column dependent to i (either incremented or decremented) 
  int pBari=map(brightValue, 0, 10, 4, 13); //take the mapped value and map the value to be used for progress bar
  //prints the progress bar
  for (int i=3; i<pBari; i++)
  {
    lcd.setCursor(i, 1);   //set the cursor row 1, column dependent to i (either incremented or decremented)   
    lcd.write(byte(0));  //display the custom char in the display (position dependent to the setCursor fnction 
  }

  delay(200);   


}

void Contrast(){

  lcd.clear();          //clear the display
  lcd.setCursor(0,0);    //set cursor to row 0, column 0
  lcd.print("  LCD Contrast");  //display title (move text to middle using space
  lcd.setCursor(0,1);          //set the cursor to row 1, column 0
  intensity=map(contrastValue, 0, 255, 0, 10); //map the value coming from Analog (CONTRASt)
  int smileyi=map(intensity, 0, 10, 4, 13);  //take the mapped value and map the value to be used for progress bar

  //prints the progress bar
  for (int i=3; i<smileyi; i++)
  {
    lcd.setCursor(i, 1); //set the cursor row 1, column dependent to i (either incremented or decremented)   
    lcd.write(byte(0));  //display the custom char in the display (position dependent to the setCursor fnction 
  }
  delay(500);   
}

void Timer(){

  if (voltage>=2.60 && voltage <=2.65){  //RIGHT
    right = buttonStatus(right);  //invert boolean value
    delay(100);
  }
  else if (voltage>=2.79 && voltage <=2.85){  //LEFT
    left = buttonStatus(left);  //invert boolean value
    delay(100);
  }


  elapseMillis = elapseMillis + t;
  elapse = (elapseMillis/4);

  if (left ==true){ //if left is pressed, start the time
    t=1;  //add to elapse millis each loop 
    right = false;
  }
  else {
    t = 0;
  }

  if (right ==true){  //if right is pressed, timer reset
    elapse = 0; //
    elapseMillis = 0;  //
    t = 0;  
    left = false;
  }
}
void wheel(){
  Timemillis = currentTime-previousTime;  //take time difference as "Time" 
  previousTime = currentTime; //then assign the current time to previous time


}





