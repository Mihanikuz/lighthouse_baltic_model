/*
 * Light.ino - Управление светодиодами на ESP32-WROOM-32
 * Реализует два эффекта:
 * 1. Плавное перетекание между 12 светодиодами (аппаратный ШИМ)
 * 2. Эффект мерцания для 3 светодиодов (аналоговый вывод)
 */

#include <Arduino.h>

// Основные настройки светодиодов
#define NUM_LEDS 12
#define CANDLES_LEDS 3

// Пины, к которым подключены светодиоды
int ledPins[NUM_LEDS] = {27, 26, 25, 33, 32, 23, 22, 21, 19, 18, 17, 16};
int candlePins[CANDLES_LEDS] = {2, 4, 5}; // Пины для эффекта мерцания

// Частота ШИМ
#define PWM_FREQ 5000

// Разрешение ШИМ (8 бит = 0-255)
#define PWM_RESOLUTION 8

// Каналы ШИМ (по одному на каждый светодиод)
int pwmChannels[NUM_LEDS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

// Задержка между шагами (в миллисекундах)
#define DELAY 2

// Параметры для эффекта мерцания
#define MIN_BRIGHTNESS 100
#define MAX_BRIGHTNESS 150
#define DELAY_TIME 300

// Переменные для управления временем
unsigned long previousMillisFlow = 0;   // Последнее время обновления эффекта перетекания
unsigned long previousMillisCandle = 0; // Последнее время обновления эффекта мерцания

// Переменные для управления состоянием эффектов
int currentLed = 0;       // Индекс текущего светодиода для эффекта перетекания
int flowBrightness = 0;   // Текущая яркость (0-255) для эффекта перетекания

void setup() {
  // Настройка каналов ШИМ для плавного перетекания
  for (int i = 0; i < NUM_LEDS; i++) {
    ledcSetup(pwmChannels[i], PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(ledPins[i], pwmChannels[i]);
  }

  // Настройка пинов для эффекта мерцания
  for (int i = 0; i < CANDLES_LEDS; i++) {
    pinMode(candlePins[i], OUTPUT);
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Основной цикл эффекта плавного перетекания:
  // 1. Плавно увеличиваем яркость текущего светодиода
  // 2. Плавно уменьшаем яркость предыдущего светодиода
  // 3. Переходим к следующему светодиоду по кругу
  if (currentMillis - previousMillisFlow >= DELAY) {
    previousMillisFlow = currentMillis;

    // Плавное увеличение яркости текущего светодиода
    if (flowBrightness <= 255) {
      ledcWrite(pwmChannels[currentLed], flowBrightness);
      flowBrightness++;
    }

    // Плавное уменьшение яркости предыдущего светодиода
    if (currentLed > 0 && flowBrightness > 0) {
      ledcWrite(pwmChannels[currentLed - 1], 255 - flowBrightness);
    } else if (currentLed == 0 && flowBrightness > 0) {
      // Для первого светодиода предыдущим будет последний
      ledcWrite(pwmChannels[NUM_LEDS - 1], 255 - flowBrightness);
    }

    // Если яркость текущего светодиода достигла максимума, переходим к следующему
    if (flowBrightness > 255) {
      flowBrightness = 0;
      currentLed = (currentLed + 1) % NUM_LEDS;
    }
  }

  // Эффект мерцания (имитация пламени свечи):
  // - Для каждого из 3 светодиодов генерируется случайная яркость
  // - Диапазон яркости ограничен MIN_BRIGHTNESS..MAX_BRIGHTNESS
  if (currentMillis - previousMillisCandle >= DELAY_TIME) {
    previousMillisCandle = currentMillis;

    for (int i = 0; i < CANDLES_LEDS; i++) {
      // Генерация случайной яркости для каждого светодиода
      int brightness = random(MIN_BRIGHTNESS, MAX_BRIGHTNESS);
      analogWrite(candlePins[i], brightness);
    }
  }
}