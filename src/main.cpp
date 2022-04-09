

#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#define TERMISTORNOMINAL 100000
#define TEMPERATURENOMINAL 25
#define BETA 3950
#define R1 9830
#define pinA D3
#define pinB D4
#define pinC D5
#define gornaGrzalka D6
#define dolnaGrzalka D7
#define DEBOUNCE_TIME 5
#define DEBOUNCE_TIME2 1000
#define napiecie 3.3 //napiecie na dzielniku napiecia
#define rozdzielczosc 1024  // ilosc stanow na przetworniku analogowym 
volatile uint32_t DebounceTimer = 0;
volatile uint32_t DebounceTimer2 = 0;
volatile uint32_t timer = 0;
volatile uint32_t timertemp = 0;
volatile int32_t setTime = 1200000;
volatile int32_t button = 0;
volatile int32_t tryb = 0;
int histereza = 2;
int setTemp = 150;
bool pinAstateCurrent = 0;
bool pinAStateLast = 1;
bool pinCstateCurrent = 0;
bool pinCStateLast = 1;
bool enableTimer = 0;
bool grzeje = 1;
bool gora = 1;
bool dol = 1;
float R2;
float aproksymacja_rezystancji;
float aproksymacja_napiecia;
double srednia;
int e = 0;
bool trybZmiana=0;

LiquidCrystal_I2C lcd(0x27,16,2); 

long int zmien(long int liczba, int ilosc)
{
//      Serial.print("test_zmien_1 ");
  pinAstateCurrent = digitalRead(pinC);
  if ((pinAStateLast == LOW) && (pinAstateCurrent == HIGH))
  {

    if (digitalRead(pinB) == HIGH)
    {
      liczba = liczba - ilosc;
    }
    else
    {

      liczba = liczba + ilosc;
    }
  }
  pinAStateLast = pinAstateCurrent;
  return (liczba);
}


ICACHE_RAM_ATTR void enkoder()
{
    Serial.print("test_enkoder_1 ");
  if (millis() - DEBOUNCE_TIME >= DebounceTimer)
  {
    DebounceTimer = millis();
    if (button == 0)
    {
      setTemp = zmien(setTemp, 5);
    }
    if (button == 1)
    {
      tryb = zmien(tryb,1);
      if (tryb > 2) tryb = 0;
      if (tryb < 0) tryb = 2;
      trybZmiana=1;
    }
    if (button == 2)
    {
      setTime = zmien(setTime, 60000);  
    }
  }
}
ICACHE_RAM_ATTR void przycisk()
{
  if (millis() - DEBOUNCE_TIME2 >= DebounceTimer2)
  {
    DebounceTimer2 = millis();
    button ++;
    if (button > 2) button=0;
    if (button == 2) enableTimer=1;
    timer = millis();
 // Serial.print("\n");
 // Serial.print(button);  
  }
}
void czas()
{

  timertemp = millis();
  lcd.setCursor(2, 1);
  setTime = setTime - (timertemp - timer);
  timer = timertemp;
  if (setTime < 0)
  {
    grzeje = 0;
  }
}
void display_reset()
{
 // Serial.print("test_01 ");
  lcd.init();   
  lcd.backlight();
  lcd.print("T");
  //lcd.setCursor(0, 1);
  //lcd.print("R");
  //lcd.setCursor(9, 1);
  lcd.setCursor(9, 0);
//  Serial.print("test_02 ");
  lcd.print("N");
}
void wyswietl(float steinhart)
{
  lcd.setCursor(2, 0);
  lcd.print("       ");
  lcd.setCursor(2, 0);
  lcd.print(steinhart);
  lcd.setCursor(2, 1);
  // lcd.setCursor(2, 1);
  // lcd.print("l/d");
  if (enableTimer == 1)
  {
    lcd.setCursor(10, 1);
    long int temp = setTime / 1000;
    lcd.print(temp / 60);
    lcd.print(":");
    lcd.print(temp % 60);
    lcd.print("  ");
  }
  lcd.setCursor(11, 0);
  lcd.print(setTemp);
  lcd.print("  ");
  lcd.setCursor(1, 1);
  lcd.print(grzeje);
  lcd.setCursor(3, 1);
  if (tryb==0) lcd.print("G  D");
  if (tryb==1) lcd.print("G   ");
  if (tryb==2) lcd.print("   D");

}
void grzanie()
{
  if (grzeje==0)
  {
    digitalWrite(gornaGrzalka,LOW);
    digitalWrite(dolnaGrzalka,LOW);   
  }
  else
  {
  if (tryb==0) 
  {    
    digitalWrite(gornaGrzalka,HIGH);
    digitalWrite(dolnaGrzalka,HIGH);   
  }
  if (tryb==1) 
  {    
    digitalWrite(gornaGrzalka,HIGH);
    digitalWrite(dolnaGrzalka,LOW);   
  }
  if (tryb==2) 
  {    
    digitalWrite(gornaGrzalka,LOW);
    digitalWrite(dolnaGrzalka,HIGH);   
  }
}
}
void setup()
{
  Serial.begin(115200);
  display_reset();
  //Serial.print("test_11 ");
  pinMode(13, INPUT);
  pinMode(14, INPUT);
 // Serial.print("test_12 ");
  pinMode(pinB, INPUT_PULLUP);
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinC, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinB), enkoder, CHANGE);
 // Serial.print("test_13 ");
  attachInterrupt(digitalPinToInterrupt(pinA), przycisk, RISING);
  pinMode(gornaGrzalka, OUTPUT);
  pinMode(dolnaGrzalka, OUTPUT);
}
void loop()
{
  if (e == 1000)
  {
    display_reset();
    e = 0;
  }
  e++;
  delay(50);
  double temp = 0;
  int samples = 10;
  // digitalWrite(10,HIGH);
  // delay(100);
  // digitalWrite(10,LOW);
  // delay(100);
  // digitalWrite(11,HIGH);
  // delay(100);
  // digitalWrite(11,LOW);
  // delay(1000);
  for (int i = 0; i < samples; i++)
  {
    temp = temp + analogRead(A0);
    delay(5);
  }
  srednia = 0. + temp / samples;
  aproksymacja_napiecia = napiecie - srednia * napiecie / rozdzielczosc;
  srednia = rozdzielczosc-1 - srednia;
  aproksymacja_rezystancji = (rozdzielczosc-1) / srednia - 1;
  aproksymacja_rezystancji = R1 / aproksymacja_rezystancji;
  float steinhart;
  steinhart = aproksymacja_rezystancji / TERMISTORNOMINAL; // (R/Ro)
  steinhart = log(steinhart);                              // ln(R/Ro)
  steinhart /= BETA;                                       // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);        // + (1/To)
  steinhart = 1.0 / steinhart;                             // Invert
  steinhart -= 273.15;
  wyswietl(steinhart);
  if (steinhart >= setTemp + histereza) grzeje = 0;
  if (steinhart <= setTemp - histereza) grzeje = 1;
  if (enableTimer == true)
  {
    czas();
  }
grzanie();
  // Serial.print("\ntemperatura\n"); // convert absolute temp to C
  // Serial.print("\n\n");

  // if (setTemp + 5 < steinhart || setTemp - 5 > steinhart)
  // {
  //   digitalWrite(A4, LOW); //Cykliczna zmiana stanu wyjścia
  //   tone(A4, 3000);
  //   delay(100);
  //   noTone(A4);
  // }
}
