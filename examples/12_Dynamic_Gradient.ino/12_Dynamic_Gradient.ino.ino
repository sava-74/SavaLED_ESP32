/**
 * @file 12_Dynamic_Gradient.ino
 * @brief Продвинутый пример: Динамическое управление градиентом.
 * 
 * Этот скетч демонстрирует, как можно управлять параметрами "статических"
 * рисующих функций в реальном времени для создания сложных анимаций.
 * 
 * Демонстрируемые функции:
 * - rainbowStatic(start, num, reversed, brightness, start_hue): Основная функция.
 * 
 * Эффект:
 * 1. В центре ленты (на пикселе 50) появляется точка.
 * 2. Каждые 5 секунд эта точка "расширяется" на 15 пикселей в обе стороны,
 *    создавая два симметричных радужных градиента, расходящихся от центра.
 * 3. Когда градиенты достигают краев ленты, анимация начинается заново.
 * 
 * АРХИТЕКТУРА:
 * - Вся логика анимации (изменение количества пикселей) и отрисовка
 *   происходят внутри `if (strip.canShow())`, что обеспечивает
 *   плавность и синхронизацию с реальной частотой обновления ленты.
 * - Отдельный `millis()`-таймер отвечает за изменение размера градиента
 *   каждые 5 секунд.
 */
#include <SavaLED_ESP32.h>

// --- Конфигурация ---
#define LED_PIN    14
#define NUM_LEDS   100
#define BRIGHTNESS 255

// --- Настройки анимации ---
const int CENTER_PIXEL = NUM_LEDS/2; // Центральный пиксель, от которого растет эффект
const int EXPAND_STEP = 1;  // На сколько пикселей расширяется градиент за шаг
const long EXPAND_INTERVAL_MS = 30; // Интервал расширения: 5 секунд

SavaLED_ESP32 strip;

// --- Переменные состояния анимации ---
int current_gradient_size = 1; // Текущее количество пикселей в каждом градиенте
unsigned long lastExpandTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\nПример 12: Динамический градиент");

  if (!strip.begin(NUM_LEDS, LED_PIN)) {
    while (true);
  }
  strip.setBrightness(BRIGHTNESS);
  lastExpandTime = millis(); // Инициализируем таймер
}

void loop() {
  uint32_t loopStartTime = micros(); // Засекаем время в начале - дебаги
  // Проверяем, не пора ли изменить размер градиента
  if (millis() - lastExpandTime >= EXPAND_INTERVAL_MS) {
    lastExpandTime = millis(); // Сбрасываем таймер
    
    // Увеличиваем размер градиента
    current_gradient_size += EXPAND_STEP;
    
    // Если градиент вышел за половину ленты, начинаем заново
    if (current_gradient_size > (NUM_LEDS/2)) {
      current_gradient_size = 0;
    }
    //Serial.printf("Новый размер градиента: %d пикселей\n", current_gradient_size);
  }

  // Вся отрисовка происходит только тогда, когда лента готова
  if (strip.canShow()) {
    
    // 1. Начинаем каждый кадр с чистого листа (черный фон)
    //    Это важно, чтобы стирать "призрачные" пиксели от предыдущего,
    //    более широкого градиента.
    strip.clear();
    
    // 2. Рисуем градиент, идущий ВПРАВО от центра
    strip.rainbowStatic(
      CENTER_PIXEL,           // Начальный пиксель
      current_gradient_size,  // Текущее количество пикселей
      false,                  // Направление: вперед (reversed = false)
      255,                    // Яркость
      170,                    // Начальный тон: Синий
      0                       // Конечный тон: красный
    );

    // 3. Рисуем градиент, идущий ВЛЕВО от центра
    strip.rainbowStatic(
      CENTER_PIXEL,           // Начальный пиксель 
      current_gradient_size,  // Текущее количество пикселей
      true,                   // Направление: назад (reversed = true)
      255,                    // Яркость
      170,                    // Начальный тон: Синий
      0                       // Конечный тон: красный
    );
    
    // 4. Отправляем готовый кадр на ленту
    strip.show();
  }

  uint32_t loopExecutionTime = micros() - loopStartTime;
  Serial.print("Loop time: ");
  Serial.println(loopExecutionTime);

}