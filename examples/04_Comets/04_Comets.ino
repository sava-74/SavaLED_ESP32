#include <SavaLED_ESP32.h>

#define LED_PIN    14
#define NUM_LEDS   100
#define BRIGHTNESS 150

SavaLED_ESP32 strip;

// Палитра для случайных цветов
const uint32_t palette[] = {
  RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, VIOLET, MAGENTA, WHITE
};
const int PALETTE_SIZE = sizeof(palette) / sizeof(palette[0]);

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  strip.begin(NUM_LEDS, LED_PIN);
  strip.setBrightness(BRIGHTNESS);
}

void loop() {
  // Просто вызываем один метод. Вся магия внутри.
  // Аргументы: кол-во комет, длина хвоста, палитра, размер палитры, цвет фона
  strip.runCometsEffect(5, 20, palette, PALETTE_SIZE, strip.Color(0, 0, 80),500);
}