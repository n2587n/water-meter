//        fresh water level monitor with two wire sender
//        33 ohms when full and 243 ohms whem enpty
//        made by Mike forland 6/6/2020

#include <Wire.h>  // Comes with Arduino IDE
#include <stdlib.h>

#

//Get the library from - https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
#include <LiquidCrystal_I2C.h>

//This following line sets the pins for the i2c backpack on your LCD
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // 0x7 is this lcd screens address


// #define CALIBRATE_MODE //comment out to disable xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#define analogPin 0    // select the input pin for the potentiometer

#define MAX 990 //max analog input
#define MIN 823 //min analog input


#define MIN_DX (.0001)
#define SAMPLE_FREQ .5 //samples per second
#define SAMPLE_AVG_FREQ 1 //freq for sampling
#define SAMPLING_WINDOW (SAMPLE_AVG_FREQ * 540) //sampling window seconds
#define TOTAL_SAMPLES (int)(SAMPLE_FREQ * SAMPLING_WINDOW) //Total number of samples to average

double samples[TOTAL_SAMPLES]; //array of samples for averaging a window of values
int sample_index = 0; //index into the array where the next sample will be stored
double lastRunningAvgMeasurement = 0; //
char buffer[10];

double getTankPercentage(){
  return ((double)analogRead(analogPin) - MIN) / ((float)MAX - (float)MIN);
}

void setup()
{
  double initialMeasurement = getTankPercentage();
  for(int i = 0; i<TOTAL_SAMPLES;i++){
    samples[i] = initialMeasurement;
  }
  
  lcd.begin(16, 2); lcd.clear();
  lcd.setCursor(0, 0);  lcd.print ("   BOOTING UP   ");
  delay(1000);
  lcd.setCursor(0, 1);  lcd.print (" water meter v4 ");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);  lcd.print("FRESHWATER LEVEL");
  lcd.setCursor(0, 0);
  delay(50);
  lcd.clear();

  lcd.setCursor(4, 1);
  lcd.print("  % IN TANK");
}

//int samples[
long int counter = 0;
double dx = 0;
float timeTillFull = 0;
float timeTillEmpty = 0;

#ifdef CALIBRATE_MODE
int measuredValue = 0;
float maxValue = 0;
float minValue = 99999;
#endif

void loop()
{

  double percentage = getTankPercentage();
  if (sample_index == TOTAL_SAMPLES) {
    sample_index = 0;
  }
  samples[sample_index++] = percentage;
  double runningAverage = 0;
  for (int iterator = 0; iterator < TOTAL_SAMPLES; iterator++) {
    runningAverage += samples[iterator];
  }
  runningAverage /= TOTAL_SAMPLES;

  timeTillFull = ((1 - runningAverage) / dx);
  timeTillEmpty = ((-1 * runningAverage) / dx);

#ifndef CALIBRATE_MODE
  //lcd.clear();           // rem this after calbration


  //lcd.setCursor(0, 0);
lcd.setCursor(0, 0);

  if (dx > MIN_DX) {
    
   // lcd.setCursor(0, 0);
    lcd.print("            ");
    //delay(1);
    lcd.setCursor(0, 0);
    //delay(1);

    if(counter < (SAMPLE_FREQ * TOTAL_SAMPLES)){
       lcd.print("calculating fill");
    }else{
    lcd.print("Fill time ");
    lcd.print((int)timeTillFull);
    //delay(1);
    lcd.print(":");
    lcd.print((int)((timeTillFull - (int)timeTillFull)*60));
    }
  
  } else if (dx < -MIN_DX) {
   // lcd.setCursor(0, 0);
    lcd.print("               ");
    lcd.setCursor(0, 0);

     if(counter < (SAMPLE_FREQ * TOTAL_SAMPLES)){
       lcd.print("calculating drain");
    }else{
    lcd.print("Drain time:");
    lcd.print((int)timeTillEmpty);
    lcd.print(":");
    lcd.print((int)((timeTillEmpty - (int)timeTillEmpty)*60));
    }
  } else {
    lcd.print("  Level stable   ");
  }

  lcd.setCursor(0, 1);  
  lcd.print("      ");
  delay(1);
  lcd.setCursor(0, 1);
  lcd.print(runningAverage * 100);

#endif

#ifdef CALIBRATE_MODE
  lcd.clear();           // rem this after calbration
  measuredValue = analogRead(analogPin);
  if (measuredValue < minValue) {
    minValue = measuredValue;
  }
  if (measuredValue > maxValue) {
    maxValue = measuredValue;
  }

  lcd.setCursor(0, 1);   // rem this after calbration
  lcd.print("Min: ");     // rem this after calbration
  lcd.print(minValue);

  lcd.setCursor(0, 0);
  lcd.print("Max: ");
  lcd.print(maxValue);
  /*
     lcd.setCursor(0, 2);   // rem this after calbration
     lcd.print("dx: ");     // rem this after calbration
     lcd.setCursor(4, 2);  // rem this after calbration
     lcd.print(dx);         // rem this after calbration
  */
#endif

  delay((1 / (float)SAMPLE_FREQ) * 1000);
  counter++;
  if ((counter % 1) == 0) {
    dx = (runningAverage - lastRunningAvgMeasurement) * SAMPLE_FREQ;


    lastRunningAvgMeasurement = runningAverage;
  }
  //lcd.clear();
}
