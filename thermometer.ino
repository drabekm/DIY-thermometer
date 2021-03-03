#include <math.h>

const int THERMOMETER_PIN = A1;
const int TEMPERATURE_BUFFER_SIZE = 50; //Voltage from the thermometer tends to jump sometimes
                                    //so this should average it and even it out

const int firstDisplayPins[4] = {9,10,11,12};
const int seccondDisplayPins[4] = {5,6,7,8};

float temperatureArray[TEMPERATURE_BUFFER_SIZE];
byte temperatureArrayIndex = 0;

void setup() {
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 59286;            // preload timer 65536-16MHz/256/10Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
  
  Serial.begin(9600);
  pinMode(THERMOMETER_PIN, INPUT);
  pinMode(2, OUTPUT);


  setUpDisplayPins(firstDisplayPins);
  setUpDisplayPins(seccondDisplayPins);
}

ISR(TIMER1_OVF_vect)        // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  TCNT1 = 59286;            // preload timer
  float thermometerVolts = getVoltage();
  float temperatureCentigrade = getCentigradeTemperature(thermometerVolts);
  insertNewTemperatureToArray(temperatureCentigrade);
  Serial.print("C average: ");
  
  float test = getAverageTemperature();
  Serial.println(test);
  /*Serial.println( (int(test) % 100) / 10);
  
  Serial.println(int(test) % 10);
  
  Serial.println(int((test * 10)) % 10);
  
  Serial.println("================");*/
  displayNumber(firstDisplayPins, (int(test) % 100) / 10);
  displayNumber(seccondDisplayPins, int(test) % 10);
}

void loop() {

  digitalWrite(2, !digitalRead(2));
  delay(500);

}

void setUpDisplayPins(int pins[4])
{
  for(int i = 0; i < 4; i++)
  {
    pinMode(pins[i], OUTPUT);
  }
}

void displayNumber(int pins[4], int number)
{
  for(int i = 0; i < 4; i++)
  {
     digitalWrite(pins[i], number %2);
     number /=2;
  }
}

float getVoltage()
{
  return analogRead(THERMOMETER_PIN) * 5.0 / 1024.0;
}

float getCentigradeTemperature(float voltage)
{
  return (voltage - 0.5) * 100;
}

void insertNewTemperatureToArray(float temperature)
{
  temperatureArray[temperatureArrayIndex] = temperature;
  temperatureArrayIndex += 1;
  temperatureArrayIndex = temperatureArrayIndex % TEMPERATURE_BUFFER_SIZE;
  Serial.println(temperatureArrayIndex);
}

float getAverageTemperature()
{
  float temperatureSum = 0;
  for(int i = 0; i < TEMPERATURE_BUFFER_SIZE; i++)
  {
    temperatureSum += temperatureArray[i];
  }

  return temperatureSum / TEMPERATURE_BUFFER_SIZE;
}
