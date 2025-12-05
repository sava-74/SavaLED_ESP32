/**
 * @file 09_CycleAllColors.ino
 * @brief Пример демонстрации всей палитры предопределенных цветов.
 * 
 * Этот скетч циклически перебирает все 24 встроенных в библиотеку цвета.
 * Каждый цвет отображается на всей ленте в течение 3 секунд.
 * 
 * Демонстрируемые функции:
 * - fillColor(COLOR, brightness): Основная функция для заливки ленты цветом
 *   с ручной установкой яркости.
 * - Предопределенные цветовые константы (RED, GREEN, BLUE, GOLD и т.д.).
 * 
 * АРХИТЕКТУРА:
 * Скетч строго следует неблокирующей парадигме:
 * 1. Таймер на основе millis() отвечает за смену цвета каждые 3 секунды.
 *    Это не останавливает выполнение loop().
 * 2. Функция strip.show() вызывается только тогда, когда лента готова
 *    принять новые данные (проверка strip.canShow()), что обеспечивает
 *    максимальную производительность и отзывчивость.
 */

#include <SavaLED_ESP32.h>

// --- Конфигурация ---
#define LED_PIN    14
#define NUM_LEDS   100
// Устанавливаем яркость для эффекта в функции fillColor, 
// а глобальную ставим на максимум, чтобы она не мешала.
#define BRIGHTNESS 255

SavaLED_ESP32 strip;

// --- 1. Создаем массивы для управления цветами ---

// Массив, содержащий все 24 предопределенных цвета
const uint32_t palette[] = {
    RED, CRIMSON, PINK, ORANGE, GOLD, YELLOW, LIME, GREEN, OLIVE, TEAL,
    CYAN, SKYBLUE, BLUE, NAVY, INDIGO, VIOLET, PURPLE, MAGENTA, MAROON,
    BROWN, WHITE, SILVER, GRAY, BLACK
};

// Соответствующий массив с английскими названиями (порядок ВАЖЕН!)
const char* palette_names_en[] = {
    "Red", "Crimson", "Pink", "Orange", "Gold", "Yellow", "Lime", "Green",
    "Olive", "Teal", "Cyan", "Sky Blue", "Blue", "Navy", "Indigo", "Violet",
    "Purple", "Magenta", "Maroon", "Brown", "White", "Silver", "Gray", "Black"
};

// Соответствующий массив с русскими названиями (порядок ВАЖЕН!)
const char* palette_names_ru[] = {
    "Красный", "Малиновый", "Розовый", "Оранжевый", "Золотой", "Желтый", "Лайм",
    "Зеленый", "Оливковый", "Бирюзовый", "Голубой", "Небесно-голубой", "Синий",
    "Темно-синий", "Индиго", "Фиалковый", "Лиловый", "Пурпурный", "Бордовый",
    "Коричневый", "Белый", "Серебряный", "Серый", "Черный"
};

// Рассчитываем общее количество цветов в палитре
const int palette_size = sizeof(palette) / sizeof(palette[0]);

// --- 2. Переменные для управления анимацией ---
int currentColorIndex = -1; // Начинаем с -1, чтобы при первом запуске стал 0
unsigned long lastColorChangeTime = 0;
const long COLOR_INTERVAL_MS = 3000; // Интервал смены цвета: 3 секунды

void setup() {
  Serial.begin(115200);
  Serial.println("\nПример 09: Циклическая смена всех цветов палитры");
  Serial.printf("Всего цветов в палитре: %d\n", palette_size);

  if (!strip.begin(NUM_LEDS, LED_PIN)) {
    Serial.println("Ошибка инициализации!");
    while (true);
  }
  
  // Устанавливаем глобальную яркость на максимум.
  // Яркостью эффекта будем управлять вторым аргументом в fillColor().
  strip.setBrightness(BRIGHTNESS);
}

void loop() {
  // --- Логика смены цвета (выполняется раз в 3 секунды) ---
  if (millis() - lastColorChangeTime >= COLOR_INTERVAL_MS) {
    lastColorChangeTime = millis(); // Сбрасываем таймер

    // Переходим к следующему цвету в массиве
    currentColorIndex++;
    if (currentColorIndex >= palette_size) {
      currentColorIndex = 0; // Если дошли до конца, начинаем сначала
    }

    // Выводим информацию в порт
    Serial.printf("Цвет: %s (%s)\n", palette_names_ru[currentColorIndex], palette_names_en[currentColorIndex]);
    
    // Заливаем буфер новым цветом. Используем fillColor, как и было запрошено.
    // Яркость ставим на 100% (255).
    strip.fillColor(palette[currentColorIndex], 50);
  }

  // --- Логика отображения (выполняется как можно чаще) ---
  // Постоянно отправляем содержимое буфера на ленту, как только она готова.
  // Это гарантирует, что новый цвет появится на ленте мгновенно после его установки.
  if (strip.canShow()) {
    strip.show();
  }
}