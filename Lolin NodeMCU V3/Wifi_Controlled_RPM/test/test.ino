int counter = 0;
unsigned long previousMillis = 0;
const long interval = 1000;

void setup()
{
  Serial.begin(115200); // initialize serial communication at 9600 BPS
}
void loop()
{

  int sensorValue = analogRead(A0); // read the input on analog pin 0

  float voltage = sensorValue * (5.0 / 1023.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V)

  unsigned long currentMillis = millis();

  if (voltage > 2)
    counter++;

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    Serial.println(counter);
    counter = 0;
  }
  // THEORETICAL MAXIMUM READING IS 8900
}

/*
int counter = 0;
unsigned long previousMillis = 0;
const long interval = 1000;
double threshold = 0.5;

void setup() {
  Serial.begin(115200);   // initialize serial communication at 9600 BPS
}
void loop()
{

  int sensorValue = analogRead(A0);   // read the input on analog pin 0

  float voltage = sensorValue * (5.0 / 1023.0);   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V)
  float previous_voltage = 0;

  unsigned long currentMillis = millis();

  if (abs(voltage - previous_voltage) > threshold)
  {
    previous_voltage = voltage;
    counter++;
  }

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    Serial.println(counter);
    counter = 0;
  }

}
 */
