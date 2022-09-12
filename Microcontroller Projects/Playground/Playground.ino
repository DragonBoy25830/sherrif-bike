int play_pin = 6;
int pause_pin = 7;
int led = 8;

void setup()
{
  Serial.begin(9600);
  pinMode(play_pin, INPUT_PULLUP);
  pinMode(pause_pin, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  attachInterrupt(play_pin, play, CHANGE);
  attachInterrupt(pause_pin, pause, CHANGE);
}

void loop()
{
}

void play()
{
  digitalWrite(led, HIGH);
  Serial.println("play");
}

void pause()
{
  digitalWrite(led, LOW);
  Serial.println("pause");
}