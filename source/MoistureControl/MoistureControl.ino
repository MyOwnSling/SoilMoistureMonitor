/*
  MoistureCotrol
  Reads the input from a 5V soil moisture sensor and determines whether or not the
  soil is dry enough to water. Actuates the attached pump based on the input from 
  the soil moisture sensor.
 */

#define DRY 4.25 // Dry threshold at which watering will occur
#define WET 2.25 // Wet threshold at which watering will stop
#define OFF 0 // Pump off constant
#define ON 1 // Pump on constant
#define POLLING_PERIOD 2500 // Period between sensor polls (ms) when not watering
#define WATERING_PERIOD 500 // Period between sensor polls (ms) when watering

int led = 13; // LED digital pin
int pumpRelay = 8; // Water pump relay digital pin
int soilMoistureAnalogPin = 0; // soil moisture sensor analog pin
//int state = POLLING; // Set initial state
int period = POLLING_PERIOD; // Time to wait between sensor polls
int count = 0; // Number of polls done while watering
boolean watering = false; // Flag that indicates whether or not the pump is on

// Polls the soil moisture sensor and returns the voltage
float getSensorVoltage()
{
  Serial.println("Soil moisture sensor voltage:");
  float moistureVoltage = analogRead(soilMoistureAnalogPin) * (5.0 / 1023.0);
  Serial.print("\t");
  Serial.println(moistureVoltage); // Print to serial for debug purposes
  return moistureVoltage;
}

// Checks to see if the water reservoir used for pumping is empty or not
boolean reservoirEmpty()
{
  // Not implemented yet
  // This will eventually check another sensor (level sensor or similar) to
  //   see if there is water in the reservoir
  return false;
}

// Turns the pump relay on or off depending on the input
void actuatePump(int state)
{
  if (state == OFF)
  {
    // Turn the pump relay off
    
    // This test prevents duplicate messages
    if (watering)
    {
      Serial.println("Done watering.");
    }
    digitalWrite(pumpRelay, LOW); // Turn the LED off since we are no longer pumping
    digitalWrite(led, LOW);
    watering = false;
  }
  else if (state == ON)
  {
    // Turn the pump relay on
    if(!reservoirEmpty())
    {
      Serial.println("Watering...");
      digitalWrite(pumpRelay, HIGH); // Turn the LED on to indicate that we are watering
      digitalWrite(led, HIGH);
      watering = true;
    }
    else
    {
      Serial.println("Cannot water, reservoir empty");
    }
  }
}

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600); // initialize serial communication at 9600 bits per second:
  pinMode(led, OUTPUT); // set the LED pin
  pinMode(pumpRelay, OUTPUT); // Set the pin used to actuate the water pump relay
}  

// the loop routine runs over and over again forever:
void loop()
{
  // Spit out some text formatting for easy reading
  Serial.println("");
  Serial.println("********************");
  
  // Poll the moisture sensor and spit out the data on watering time (current or previous)
  float moisture = getSensorVoltage();
  Serial.print("Watering/watered for ");
  Serial.print((float)((float)count * (float)WATERING_PERIOD / 1000.0));
  Serial.print(" seconds");
  Serial.println("");
  
  // Run a check to see if the reservoir is empty
  if(reservoirEmpty())
  {
    Serial.println("WARNING: reservoir is empty");
  }
  
  // If the sensor voltage goes above the DRY treshold, then start watering
  if (moisture >= DRY)
  {
    if(reservoirEmpty())
    {
      Serial.println("Water reservoir is empty, pump will not activate");
    }
    else
    {
      period = WATERING_PERIOD;
      if (!watering)
      {
        count = 0;
      }
      actuatePump(ON);
    }
  }
  // If the sensor voltage goes below the WET threshold, then stop watering
  else if (moisture <= WET)
  {
    period = POLLING_PERIOD;
    actuatePump(OFF);
  }
  
  if (watering)
  {
    // Currently watering, so we will increment the counter
    count++;
  }
  
  // Don't do anything for a bit
  delay(period);
}
