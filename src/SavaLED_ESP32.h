#ifndef SAVA_LED_ESP32_H
#define SAVA_LED_ESP32_H

#include <Arduino.h>
#include "driver/rmt_tx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
// Определяем общепринятые константы для таймингов WS2812.
#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz разрешение, 1 тик = 100ns
// --- Максимальное кол-во комет, которое поддерживает библиотека ---
#define SAVA_MAX_COMETS 10
// --- Предопределенные цветовые константы (формат 0xRRGGBB) ---
const uint32_t RED      = 0xFF0000;
const uint32_t LIME     = 0x00FF00; // Ярко-зеленый
const uint32_t BLUE     = 0x0000FF;

// Вторичные цвета
const uint32_t YELLOW   = 0xFFFF00;
const uint32_t CYAN     = 0x00FFFF;
const uint32_t MAGENTA  = 0xFF00FF;

// Третичные и популярные цвета
const uint32_t ORANGE   = 0xFF7F00;
const uint32_t CRIMSON  = 0xDC143C; // Малиновый
const uint32_t PINK     = 0xFF69B4; // Ярко-розовый
const uint32_t GOLD     = 0xFFD700;
const uint32_t GREEN    = 0x008000; // Темно-зеленый
const uint32_t OLIVE    = 0x808000;
const uint32_t TEAL     = 0x008080; // Бирюзовый
const uint32_t SKYBLUE  = 0x87CEEB; // Небесно-голубой
const uint32_t NAVY     = 0x000080; // Темно-синий
const uint32_t INDIGO   = 0x4B0082;
const uint32_t VIOLET   = 0xEE82EE; // Фиалковый
const uint32_t PURPLE   = 0x800080;
const uint32_t MAROON   = 0x800000; // Бордовый
const uint32_t BROWN    = 0xA52A2A;

// Оттенки серого и специальные
const uint32_t WHITE    = 0xFFFFFF;
const uint32_t SILVER   = 0xC0C0C0;
const uint32_t GRAY     = 0x808080;
const uint32_t BLACK    = 0x000000; // Он же "Выключено"
// Состояние кометы
enum class CometState { INACTIVE, APPEARING, MOVING };
// Структура для хранения данных одной кометы
struct SavaComet {
    CometState    state = CometState::INACTIVE;
    uint32_t      color = 0;
    int           position = 0;
    int           direction = 1;
    uint8_t       brightness = 0;
    long          speed_ms = 50;
	int           tail_length = 10;
    int           current_tail_len = 0;
    unsigned long spawn_time = 0;
    unsigned long last_move_time = 0;
};

/**
 * @class SavaLED_ESP32
 * @brief Низкоуровневая, неблокирующая библиотека для управления адресными светодиодами WS2812/SK6812
 *        на ESP32 с использованием периферии RMT.
 */
class SavaLED_ESP32 {
public:
    SavaLED_ESP32();
    ~SavaLED_ESP32();

    // --- Основные функции ---
    bool begin(uint16_t numLeds, int pin);    // Инициализация
    void show();                              // Отправка данных на ленту
    bool canShow() const;                       // Проверка готовности к следующему кадру    
    void setBrightness(uint8_t brightness);
    uint16_t getNumLeds() const;
    
    // --- Функции для работы с пикселями (RGB) ---
    void setPixel(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    void setPixel(uint16_t n, uint32_t color);
    void setPixelColor(uint16_t n, uint32_t color, uint8_t brightness);
    void clear();
    void fill(uint8_t r, uint8_t g, uint8_t b);
    void fill(uint32_t color);
    void fillColor(uint32_t color, uint8_t brightness);

    // --- Функции-помощники и работа с цветом (HSV) ---
    uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
    void setPixelHSV(uint16_t n, uint8_t h, uint8_t s, uint8_t v);
    void fillHSV(uint8_t h, uint8_t s, uint8_t v);

    // --- Гамма-коррекция ---
    void setGammaCorrection(bool enabled);

    // --- Неблокирующие эффекты ---
    /**
    * @brief Рисует статичный радужный градиент на всю длину ленты.
    * @param speed скорость анимации (0-255).
    * @param brightness Яркость эффекта (0-255) по умолчанию 255.
    */
    void rainbowCycle(uint16_t speed, uint8_t brightness = 255);
    /**
    * @brief Рисует статичный радужный градиент на сегменте ленты.
    * @param start_pixel Индекс начального пикселя.
    * @param num_pixels Количество пикселей в градиенте.
    * @param reversed Направление градиента. false - вперед, true - назад.
    * @param speed скорость анимации (0-255).
    * @param brightness Яркость эффекта (0-255) по умолчанию 255.
    */
    void rainbowCycle(uint16_t start_pixel, uint16_t num_pixels, bool reversed, uint16_t speed, uint8_t brightness = 255);
    void breathingRainbow(uint16_t speed, uint8_t brightness = 255);

    /**
    * @brief [Версия 1: Радуга] Рисует статичный радужный градиент на сегменте ленты.
    * @param start_pixel Индекс начального пикселя.
    * @param num_pixels Количество пикселей в градиенте.
    * @param reversed Направление градиента. false - вперед, true - назад.
    * @param brightness Яркость градиента (0-255).
    * @param start_hue Начальный тон (0-255), с которого начнется радуга (0 - красный).
    */
    void rainbowStatic(uint16_t start_pixel, uint16_t num_pixels, bool reversed = false, uint8_t brightness = 255, uint8_t start_hue = 0);

    /**
    * @brief [Версия 2: Градиент] Рисует статичный градиент между двумя любыми цветами.
    * @param start_pixel Индекс начального пикселя.
    * @param num_pixels Количество пикселей в градиенте.
    * @param reversed Направление градиента. false - вперед, true - назад.
    * @param brightness Яркость градиента (0-255).
    * @param start_color Начальный цвет градиента (например, RED).
    * @param end_color Конечный цвет градиента (например, BLUE).
    */
    void rainbowStatic(uint16_t start_pixel, uint16_t num_pixels, bool reversed, uint8_t brightness, uint8_t start_color, uint8_t end_color);


    // --- НОВАЯ ФУНКЦИЯ ДЛЯ ЭФФЕКТА КОМЕТ ---
    /**
     * @brief Запускает и обновляет неблокирующий эффект "Кометы".
     *        Вызывайте эту функцию в каждой итерации loop().
     * @param num_comets Количество комет (до SAVA_MAX_COMETS).
     * @param tail_length Фиксированная длина хвоста для всех комет.
     * @param palette Массив цветов для случайного выбора.
     * @param palette_size Размер массива palette.
     * @param background_color Цвет фона (например, BLACK, или strip.Color(r,g,b)).
	 * @param spawn_interval_ms Интервал в миллисекундах, с которым библиотека будет ПЫТАТЬСЯ создать новую комету.
     */
    void runCometsEffect(uint8_t num_comets, uint8_t tail_length, const uint32_t palette[], int palette_size, uint32_t background_color = BLACK, uint16_t spawn_interval_ms = 1500);

private:
    uint16_t _numLeds;
    int _pin;
    uint8_t _brightness;
    bool _isReady;
    
    uint8_t* _pixels;
    uint8_t* _tx_buffer;

    rmt_channel_handle_t _ledChannel;
    rmt_encoder_handle_t _ledEncoder;
    rmt_transmit_config_t _txConfig;

    static IRAM_ATTR bool _rmt_tx_done_callback(rmt_channel_handle_t tx_chan, const rmt_tx_done_event_data_t *edata, void *user_ctx);
    SemaphoreHandle_t _tx_done_sem;
    
    void _cleanup();
    
    // --- Таблица-ускоритель для HSV -> RGB конвертации ---
    static const uint8_t _rainbow_wheel[3][256];
    
    // --- Члены для Гамма-коррекции ---
    bool _gamma_enabled;
    static const uint8_t _gamma_table[256];

	// --- ВНУТРЕННИЙ МЕНЕДЖЕР ЭФФЕКТА "КОМЕТЫ" ---
    SavaComet _comets[SAVA_MAX_COMETS];
    unsigned long _comets_last_spawn_attempt = 0;

    // Приватные методы, которые реализуют логику
    void _spawnComet(uint8_t num_comets, uint8_t tail_length, const uint32_t palette[], int palette_size);
    void _updateComets(uint8_t tail_length);
    void _drawComets(uint32_t background_color, uint8_t tail_length);
	
    // --- Система слотов для управления состоянием встроенных эффектов ---
    struct EffectState {
        uint32_t last_update = 0;
        uint8_t  counter = 0;
    };
    EffectState _effect_slots[5]; // Поддерживаем до 5 независимых эффектов в одном кадре
    int _current_effect_slot = 0;
	
};

#endif // SAVA_LED_ESP32_H