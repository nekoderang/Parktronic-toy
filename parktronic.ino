#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Ultrasonic.h>    // подключаем библиотеку Ultrasonic

int map_led[9] = {
  B00000000,
  B10000000,
  B11000000,
  B11100000,
  B11110000,
  B11111000,
  B11111100,
  B11111110,
  B11111111,
};
int dist = 0;
unsigned long tmr;
int latchPin = 8; //Пин подключен к ST_CP входу 74HC595
int clockPin = 12;  //Пин подключен к SH_CP входу 74HC595
int dataPin = 11; //Пин подключен к DS входу 74HC595
int piezoPin = 3; //Пин подключения пьезодинамика
Ultrasonic ultrasonic(6,5);
Thread soundThread = Thread();

void setup() {
  
  Serial.begin (9600);

  //Тип пинов для сдвигового регистра
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  //масштабирование по частоте пищалки с дальнего до ближнего расстояний
  int math_tmr = map(dist, 50, 4, 850, 250);
  //инициализация параллельного потока на пищалку
  soundThread.onRun(tone_sound);     // назначаем потоку задачу
  soundThread.setInterval(math_tmr);

  //Считывание расстояния дальномером c определенной задержкой
  if (millis()-tmr > 20)
     {
      tmr = millis();
      dist = ultrasonic.distanceRead(CM);
     }
 
  Serial.print(dist);     // выводим расстояние в сантиметрах
  Serial.println(" cm");

  //включение потока пищалки с максимальной дистанции
  if (dist <= 50) {
    if (soundThread.shouldRun())
        soundThread.run(); // запускаем поток
  }
  
  //масштабирование расстояния для светодиодной шкалы
  int math_led = map(dist, 4, 50, 8, 0);
  if (dist >= 51) math_led = 0;   //фильтр по нижней границе шкалы
  
  //Вывод значений
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, map_led[math_led]); // Инвертируем сигнал при помощи MSBFIRST, грузим с первого бита
  digitalWrite(latchPin, HIGH);

}

void tone_sound() {    // Управление левым мотором
      tone(piezoPin, 1000, 200);
     }
