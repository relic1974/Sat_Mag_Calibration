#include <SAT_AppStorage.h>
#include <EEPROM.h>
#include <nanosat_message.h>
#include <I2C_Conv.h>
#include <I2C_add.h>
#include "SAT_Mag.h"
#include <Wire.h>
#include <stdio.h>
#include <string.h>
#include <OnboardCommLayer.h>

OnboardCommLayer ocl;
SAT_AppStorage storage;
SAT_Mag mag;

#define MAX_BUFFER_SIZE 128

char offsLineBuffer[MAX_BUFFER_SIZE];
 
int x, y, z;
int xmin, xmax, ymax, ymin, zmin, zmax; 
int samples = 0;
int i;
int charCount = 0;

float offx = 0.0;
float offy = 0.0;
float offz = 0.0;

void setup() 
{
  Wire.begin();
  Serial.begin(9600);  
  mag.configMag(); 
  
  Serial.println("Taking 360 samples, please wait...\n");  
  takeSample();
}
 
void loop()  {}


void takeSample(void)
{
  for(samples = 0; samples <= 360; samples ++)
  {
    x = mag.readx();               
    y = mag.ready();
    z = mag.readz();
    
    if(samples == 0)                        // set initial values for all minimums and maximums
    {      
      xmin = x;
      xmax = x;     
      ymin = y;
      ymax = y;      
      zmin = z;
      zmax = z; 
    }
    
    if (xmax < x)  {xmax = x;}                      // first 90 mins in orbit calibrate
    if (xmin > x)  {xmin = x;}
    if (ymax < y)  {ymax = y;}
    if (ymin > y)  {ymin = y;}
    if (zmax < z)  {zmax = z;}
    if (zmin > z)  {zmin = z;}
    
    Serial.print("samples taken = ");
    Serial.println(samples);
    
    delay(15000);                                     // set to 15000
  }
  
    Serial.print("\n\nmin x: ");
    Serial.print(xmin);
    Serial.print(" max x: ");
    Serial.println(xmax); 
  
    Serial.print("min y: ");
    Serial.print(ymin);
    Serial.print(" max y: ");
    Serial.println(ymax); 
  
    Serial.print("min z: ");
    Serial.print(zmin);
    Serial.print(" max z: ");
    Serial.println(zmax); 
    
    offx = float(xmin + xmax) / 2;                 // generate offset values
    offy = float(ymin + ymax) / 2;
    offz = float(zmin + zmax) / 2;
    
    Serial.println("\n\nSuggested new offset values: ");
    
    Serial.print("xoff = ");
    Serial.print(offx);  Serial.print("\t");
    Serial.print("yoff = ");
    Serial.print(offy);  Serial.print("\t");
    Serial.print("zoff = ");
    Serial.println(offz);
    
    char offsbuffer[30];
    sprintf(offsbuffer, "%d,%d,%d\n", (int)offx, (int)offy, (int)offz);
    Serial.println("\nContents of offsbuffer...\n");
    Serial.println(offsbuffer);
    
    int offswritten = snprintf(offsLineBuffer, MAX_BUFFER_SIZE - 1, "%f,%f,%f\n", offx, offy, offz);    
    offsLineBuffer[offswritten] = '\0';
    charCount += offswritten;
    
    Serial.print("\nint offswritten = ");
    Serial.print(offswritten);
    
    storage.send(offsLineBuffer);         // store calibration data on 1st line
}
