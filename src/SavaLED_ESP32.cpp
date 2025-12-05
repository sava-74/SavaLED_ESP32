#include "SavaLED_ESP32.h"
#include "esp_log.h"

// Константы таймингов, используются только здесь
#define WS2812_T0H_NS 400
#define WS2812_T0L_NS 850
#define WS2812_T1H_NS 800
#define WS2812_T1L_NS 450

// --- Таблица гамма-коррекции ---
const uint8_t SavaLED_ESP32::_gamma_table[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 5,
  5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8,
  9, 10, 10, 10, 10, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14,
  15, 15, 15, 16, 17, 17, 17, 17, 18, 18, 19, 20, 20, 20, 20, 21,
  22, 22, 23, 23, 23, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 30,
  30, 31, 31, 32, 33, 33, 34, 35, 35, 36, 37, 37, 38, 38, 39, 40,
  41, 41, 42, 43, 44, 45, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53,
  54, 54, 55, 56, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
  68, 69, 69, 70, 71, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 84,
  85, 86, 87, 89, 90, 91, 93, 94, 95, 96, 97, 98, 100, 101, 102, 103,
  105, 106, 108, 109, 110, 111, 113, 115, 117, 118, 119, 121, 122, 123, 124, 127,
  128, 130, 131, 133, 134, 136, 137, 139, 140, 143, 145, 146, 147, 148, 151, 153,
  154, 156, 158, 159, 162, 163, 165, 167, 169, 171, 173, 174, 176, 179, 180, 182,
  185, 186, 188, 190, 192, 194, 196, 199, 201, 202, 205, 207, 209, 211, 214, 216,
  218, 220, 223, 225, 226, 230, 231, 235, 236, 240, 241, 245, 246, 250, 251, 255
};

// --- Таблица "Радужное Колесо" (3x256, R-G-B компоненты) ---
const uint8_t SavaLED_ESP32::_rainbow_wheel[3][256] = {
{
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 253, 247, 241, 235, 229, 223, 217, 211, 205, 199, 193, 187, 181, 175, 169, 163, 157,
  151, 145, 139, 133, 128, 122, 116, 110, 104, 98, 92, 86, 80, 74, 68, 62, 56, 50, 44, 38,
  32, 26, 20, 14, 8, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 8, 14, 20, 26, 32, 38, 44, 50,
  56, 62, 68, 74, 80, 86, 92, 98, 104, 110, 116, 122, 128, 133, 139, 145, 151, 157, 163, 169,
  175, 181, 187, 193, 199, 205, 211, 217, 223, 229, 235, 241, 247, 253, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
},
{
  0, 6, 12, 18, 24, 30, 36, 42, 48, 54, 60, 66, 72, 78, 84, 90, 96, 102, 108, 114,
  120, 126, 131, 137, 143, 149, 155, 161, 167, 173, 179, 185, 191, 197, 203, 209, 215, 221, 227, 233,
  239, 245, 251, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 249, 243, 237, 231, 225, 219, 213, 207, 201, 195, 189,
  183, 177, 171, 165, 159, 153, 147, 141, 135, 129, 124, 118, 112, 106, 100, 94, 88, 82, 76, 70,
  64, 58, 52, 46, 40, 34, 28, 22, 16, 10, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
},
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 4, 10, 16, 22, 28, 34, 40, 46, 52, 58, 64, 70, 76, 82,
  88, 94, 100, 106, 112, 118, 124, 129, 135, 141, 147, 153, 159, 165, 171, 177, 183, 189, 195, 201,
  207, 213, 219, 225, 231, 237, 243, 249, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 251, 245, 239, 233, 227, 221,
  215, 209, 203, 197, 191, 185, 179, 173, 167, 161, 155, 149, 143, 137, 131, 126, 120, 114, 108, 102,
  96, 90, 84, 78, 72, 66, 60, 54, 48, 42, 36, 30, 24, 18, 12, 6
}
};


IRAM_ATTR bool SavaLED_ESP32::_rmt_tx_done_callback(rmt_channel_handle_t tx_chan, const rmt_tx_done_event_data_t *edata, void *user_ctx) {
    SemaphoreHandle_t sem = (SemaphoreHandle_t)user_ctx;
    BaseType_t task_woken = pdFALSE;
    xSemaphoreGiveFromISR(sem, &task_woken);
    return task_woken == pdTRUE;
}

SavaLED_ESP32::SavaLED_ESP32() : 
    _numLeds(0), 
    _pin(-1), 
    _brightness(255), 
    _isReady(false), 
    _pixels(nullptr),
    _tx_buffer(nullptr),
    _ledChannel(nullptr),
    _ledEncoder(nullptr),
    _tx_done_sem(nullptr),
	_gamma_enabled(true)
{
}

SavaLED_ESP32::~SavaLED_ESP32() {
    _cleanup();
}

void SavaLED_ESP32::_cleanup() {
    if (_tx_done_sem && xSemaphoreTake(_tx_done_sem, pdMS_TO_TICKS(100)) == pdFAIL) {}
    if (_pixels) { delete[] _pixels; _pixels = nullptr; }
    if (_tx_buffer) { delete[] _tx_buffer; _tx_buffer = nullptr; }
    if (_ledEncoder) { rmt_del_encoder(_ledEncoder); _ledEncoder = nullptr; }
    if (_ledChannel) {
        rmt_disable(_ledChannel);
        rmt_del_channel(_ledChannel);
        _ledChannel = nullptr;
    }
    if (_tx_done_sem) { vSemaphoreDelete(_tx_done_sem); _tx_done_sem = nullptr; }
    _isReady = false;
}

bool SavaLED_ESP32::begin(uint16_t numLeds, int pin) {
    if (_isReady) _cleanup();

    _numLeds = numLeds;
    _pin = pin;

    size_t buffer_size = _numLeds * 3;
    _pixels = new (std::nothrow) uint8_t[buffer_size];
    if (!_pixels) return false;
    
    _tx_buffer = new (std::nothrow) uint8_t[buffer_size];
    if (!_tx_buffer) { _cleanup(); return false; }

    memset(_pixels, 0, buffer_size);

    _tx_done_sem = xSemaphoreCreateBinary();
    if (!_tx_done_sem) { _cleanup(); return false; }
    xSemaphoreGive(_tx_done_sem);

    rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = (gpio_num_t)_pin,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .mem_block_symbols = 64, 
        .trans_queue_depth = 4,
        .flags = {.invert_out = false, .with_dma = false}
    };
    
    esp_err_t err = rmt_new_tx_channel(&tx_chan_config, &_ledChannel);
    if (err != ESP_OK) { _cleanup(); return false; }
    
     rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = {
            .duration0 = (uint32_t)(WS2812_T0H_NS * 1ULL * RMT_LED_STRIP_RESOLUTION_HZ / 1000000000ULL),
            .level0 = 1,
            .duration1 = (uint32_t)(WS2812_T0L_NS * 1ULL * RMT_LED_STRIP_RESOLUTION_HZ / 1000000000ULL),
            .level1 = 0,
        },
        .bit1 = {
            .duration0 = (uint32_t)(WS2812_T1H_NS * 1ULL * RMT_LED_STRIP_RESOLUTION_HZ / 1000000000ULL),
            .level0 = 1,
            .duration1 = (uint32_t)(WS2812_T1L_NS * 1ULL * RMT_LED_STRIP_RESOLUTION_HZ / 1000000000ULL),
            .level1 = 0,
        },
        .flags = {.msb_first = 1}
    };

    err = rmt_new_bytes_encoder(&bytes_encoder_config, &_ledEncoder);
    if (err != ESP_OK) { _cleanup(); return false; }

    rmt_tx_event_callbacks_t cbs = { .on_trans_done = _rmt_tx_done_callback };
    err = rmt_tx_register_event_callbacks(_ledChannel, &cbs, _tx_done_sem);
    if (err != ESP_OK) { _cleanup(); return false; }
    
    err = rmt_enable(_ledChannel);
    if (err != ESP_OK) { _cleanup(); return false; }

    _txConfig = {.loop_count = 0};
    _isReady = true;
    return true;
}

void SavaLED_ESP32::show() {
    if (!_isReady || !canShow()) return;
    
    // Это происходит в начале отправки, подготавливая библиотеку к следующему кадру.
    _current_effect_slot = 0;

    xSemaphoreTake(_tx_done_sem, 0);

    size_t buffer_size = _numLeds * 3;
    
    if (_brightness < 255) {
        for (uint32_t i = 0; i < buffer_size; i++) {
            _tx_buffer[i] = ((uint16_t)_pixels[i] * _brightness) >> 8;
        }
    } else {
        memcpy(_tx_buffer, _pixels, buffer_size);
    }
    
    if (_gamma_enabled) {
        for (uint32_t i = 0; i < buffer_size; i++) {
            _tx_buffer[i] = _gamma_table[_tx_buffer[i]];
        }
    }
    
    if (rmt_transmit(_ledChannel, _ledEncoder, _tx_buffer, buffer_size, &_txConfig) != ESP_OK) {
        xSemaphoreGive(_tx_done_sem);
    }
}

bool SavaLED_ESP32::canShow() const {
    if (!_isReady || !_tx_done_sem) return false;
    return uxSemaphoreGetCount(_tx_done_sem) > 0;
}

void SavaLED_ESP32::setBrightness(uint8_t brightness) {
    _brightness = brightness;
}

uint16_t SavaLED_ESP32::getNumLeds() const {
    return _numLeds;
}

void SavaLED_ESP32::setPixel(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
    if (!_pixels || n >= _numLeds) return;
    uint16_t idx = n * 3;
    _pixels[idx]     = g;
    _pixels[idx + 1] = r;
    _pixels[idx + 2] = b;
}

void SavaLED_ESP32::setPixel(uint16_t n, uint32_t color) {
    setPixel(n, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

void SavaLED_ESP32::setPixelColor(uint16_t n, uint32_t color, uint8_t brightness) {
    if (!_pixels || n >= _numLeds) return;
    uint8_t r = ((uint16_t)((color >> 16) & 0xFF) * brightness) >> 8;
    uint8_t g = ((uint16_t)((color >> 8) & 0xFF) * brightness) >> 8;
    uint8_t b = ((uint16_t)(color & 0xFF) * brightness) >> 8;
    setPixel(n, r, g, b);
}

void SavaLED_ESP32::clear() {
    //fill(0, 0, 0);
    if (!_pixels) return;
    memset(_pixels, 0, _numLeds * 3);
}

void SavaLED_ESP32::fill(uint8_t r, uint8_t g, uint8_t b) {
    if (!_pixels) return;
    for (uint16_t i = 0; i < _numLeds; i++) {
        setPixel(i, r, g, b);
    }
}

void SavaLED_ESP32::fill(uint32_t color) {
    fill((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

void SavaLED_ESP32::fillColor(uint32_t color, uint8_t brightness) {
    uint8_t r = ((uint16_t)((color >> 16) & 0xFF) * brightness) >> 8;
    uint8_t g = ((uint16_t)((color >> 8) & 0xFF) * brightness) >> 8;
    uint8_t b = ((uint16_t)(color & 0xFF) * brightness) >> 8;
    fill(r, g, b);
}

uint32_t SavaLED_ESP32::Color(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

/*void SavaLED_ESP32::setPixelHSV(uint16_t n, uint8_t h, uint8_t s, uint8_t v) {
    if (!_pixels || n >= _numLeds) return;
    uint8_t r, g, b;
    if (s == 0) { r = g = b = v; } 
    else {
        uint8_t region = h / 43;
        uint8_t remainder = (h - (region * 43)) * 6;
        uint8_t p = (v * (255 - s)) >> 8;
        uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
        uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
        switch (region) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            default: r = v; g = p; b = q; break;
        }
    }
    setPixel(n, r, g, b);
}

void SavaLED_ESP32::fillHSV(uint8_t h, uint8_t s, uint8_t v) {
    uint8_t r, g, b;
    if (s == 0) { r = g = b = v; } 
    else {
        uint8_t region = h / 43;
        uint8_t remainder = (h - (region * 43)) * 6;
        uint8_t p = (v * (255 - s)) >> 8;
        uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
        uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
        switch (region) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            default: r = v; g = p; b = q; break;
        }
    }
    fill(r, g, b);
}*/
void SavaLED_ESP32::setPixelHSV(uint16_t n, uint8_t h, uint8_t s, uint8_t v) {
    if (!_pixels || n >= _numLeds) return;

    // 1. Берем готовые R, G, B компоненты из таблицы.
    uint8_t r = _rainbow_wheel[0][h];
    uint8_t g = _rainbow_wheel[1][h];
    uint8_t b = _rainbow_wheel[2][h];

    // 2. Применяем яркость (Value), если нужно.
    if (v != 255) {
        r = ((uint16_t)r * v) >> 8;
        g = ((uint16_t)g * v) >> 8;
        b = ((uint16_t)b * v) >> 8;
    }

    // 3. Применяем насыщенность (Saturation), если нужно.
    if (s != 255) {
        uint8_t V = v; // Яркость, которую нужно подмешивать
        uint8_t S = s;
        r = ((r * S) + (V * (255 - S))) >> 8;
        g = ((g * S) + (V * (255 - S))) >> 8;
        b = ((b * S) + (V * (255 - S))) >> 8;
    }
    
    // 4. Устанавливаем итоговый цвет.
    setPixel(n, r, g, b);
}

void SavaLED_ESP32::fillHSV(uint8_t h, uint8_t s, uint8_t v) {
    // Та же логика, что и выше, но для одного цвета,
    // который затем используется для заливки.
    uint8_t r = _rainbow_wheel[0][h];
    uint8_t g = _rainbow_wheel[1][h];
    uint8_t b = _rainbow_wheel[2][h];

    if (v != 255) {
        r = ((uint16_t)r * v) >> 8;
        g = ((uint16_t)g * v) >> 8;
        b = ((uint16_t)b * v) >> 8;
    }
    if (s != 255) {
        uint8_t V = v;
        uint8_t S = s;
        r = ((r * S) + (V * (255 - S))) >> 8;
        g = ((g * S) + (V * (255 - S))) >> 8;
        b = ((b * S) + (V * (255 - S))) >> 8;
    }

    fill(r, g, b);
}

void SavaLED_ESP32::setGammaCorrection(bool enabled) {
    _gamma_enabled = enabled;
}

// --- НЕБЛОКИРУЮЩИЕ ЭФФЕКТЫ ---

void SavaLED_ESP32::rainbowCycle(uint16_t speed, uint8_t brightness) {
    // Эта простая версия просто вызывает расширенную,
    // применяя эффект ко всей ленте с движением вперед.
    // Это обеспечивает полную обратную совместимость.
    rainbowCycle(0, _numLeds, false, speed, brightness);
}

void SavaLED_ESP32::rainbowCycle(uint16_t start_pixel, uint16_t num_pixels, bool reversed, uint16_t speed, uint8_t brightness) {
    // Проверяем, не вышли ли мы за пределы выделенных слотов
    if (_current_effect_slot >= 5) return;

    // Получаем ссылку на состояние для ТЕКУЩЕГО вызова
    EffectState& state = _effect_slots[_current_effect_slot];

    if (speed == 0) return;

    uint32_t frame_delay = map(speed, 1, 255, 50, 1);
    if (millis() - state.last_update < frame_delay) {
        // Если для этого слота время еще не пришло, мы все равно должны нарисовать его старое состояние!
        // Иначе он будет "моргать". Поэтому `return` убираем.
    } else {
        state.last_update = millis();
        state.counter++; // Продвигаем анимацию только для этого слота
    }

    // Рисуем кадр, используя `state.counter` из нашего слота
    for (uint16_t i = 0; i < num_pixels; i++) {
        int pixel_index;
        if (reversed) {
            pixel_index = start_pixel - i;
        } else {
            pixel_index = start_pixel + i;
        }

        if (pixel_index < 0 || pixel_index >= _numLeds) {
            continue; 
        }

        uint8_t hue = ((i * 256) / num_pixels) - state.counter;
        setPixelHSV(pixel_index, hue, 255, brightness);
    }
    
    // Переключаемся на следующий слот для следующего вызова эффекта В ЭТОМ ЖЕ КАДРЕ
    _current_effect_slot++;
}

void SavaLED_ESP32::rainbowStatic(uint16_t start_pixel, uint16_t num_pixels, bool reversed, uint8_t brightness, uint8_t start_hue) {
    // Эта функция просто вызывает основную, передавая ей "конечный тон" по умолчанию (255).
    // Это создаст полный спектр радуги.
    rainbowStatic(start_pixel, num_pixels, reversed, brightness, start_hue, 255);
}

void SavaLED_ESP32::rainbowStatic(uint16_t start_pixel, uint16_t num_pixels, bool reversed, uint8_t brightness, uint8_t start_hue, uint8_t end_hue) {
    if (num_pixels == 0) return;

    for (uint16_t i = 0; i < num_pixels; i++) {
        int pixel_index;
        if (reversed) {
            pixel_index = start_pixel - i;
        } else {
            pixel_index = start_pixel + i;
        }

        if (pixel_index < 0 || pixel_index >= _numLeds) {
            continue; 
        }

        // Линейная интерполяция Тона (Hue) от start_hue до end_hue
        uint8_t hue = start_hue + ((int16_t)end_hue - start_hue) * i / (num_pixels > 1 ? num_pixels - 1 : 1);
        
        // Рисуем пиксель, используя рассчитанный тон и заданную яркость
        setPixelHSV(pixel_index, hue, 255, brightness);
    }
}


void SavaLED_ESP32::breathingRainbow(uint16_t speed, uint8_t brightness) {
    static uint32_t last_update = 0;
    static uint8_t counter = 0;
    if (speed == 0) return;
    uint32_t frame_delay = map(speed, 1, 255, 50, 1);
    if (millis() - last_update < frame_delay) return;
    last_update = millis();
    counter++;
    fillHSV(counter, 255, brightness);
    //if (canShow()) show();
}
// --- РЕАЛИЗАЦИЯ ЭФФЕКТА "КОМЕТЫ" (ИНКАПСУЛИРОВАННАЯ ВЕРСИЯ) ---

// --- Публичный метод ---
void SavaLED_ESP32::runCometsEffect(uint8_t num_comets, uint8_t tail_length, const uint32_t palette[], int palette_size, uint32_t background_color, uint16_t spawn_interval_ms) {
    if (num_comets > SAVA_MAX_COMETS) num_comets = SAVA_MAX_COMETS;

    static unsigned long last_frame_time = 0;
    const int FRAME_INTERVAL_MS = 1000 / 60; // 60 FPS

    // 1. Логика (спаун и обновление позиций) - выполняется всегда
    if (millis() - _comets_last_spawn_attempt >= spawn_interval_ms) {
        _comets_last_spawn_attempt = millis();
        _spawnComet(num_comets, tail_length, palette, palette_size);
    }
    _updateComets(tail_length);

    // 2. Отрисовка - выполняется с фиксированным FPS
    if (millis() - last_frame_time >= FRAME_INTERVAL_MS) {
        last_frame_time = millis();
        _drawComets(background_color, tail_length);
        //if (canShow()) {
          //  show();
        //}
    }
}

// --- Приватные методы (твоя логика, перенесенная внутрь класса) ---
void SavaLED_ESP32::_spawnComet(uint8_t num_comets, uint8_t tail_length, const uint32_t palette[], int palette_size) {
    // Ищем "спящую" комету только среди активного пула
    for (int i = 0; i < num_comets; i++) {
        if (_comets[i].state == CometState::INACTIVE) {
            SavaComet& c = _comets[i];
            c.state = CometState::APPEARING;
            c.color = palette[random(palette_size)];
            c.position = random(_numLeds);
            
            const int edgeZoneSize = _numLeds / 4;
            if (c.position < edgeZoneSize) c.direction = 1;
            else if (c.position >= _numLeds - edgeZoneSize) c.direction = -1;
            else c.direction = (random(2) == 0) ? 1 : -1;

            c.brightness = 0;
            c.speed_ms = random(20, 71);
            c.tail_length = tail_length; // Используем переданный параметр
            c.current_tail_len = 0;
            c.spawn_time = millis();
            c.last_move_time = millis();
            return;
        }
    }
}

void SavaLED_ESP32::_updateComets(uint8_t tail_length) {
    unsigned long currentTime = millis();
    for (int i = 0; i < SAVA_MAX_COMETS; i++) {
        if (_comets[i].state == CometState::INACTIVE) continue;

        SavaComet& c = _comets[i];
        const unsigned long APPEAR_DURATION = 1800;

        if (c.state == CometState::APPEARING) {
            unsigned long timePassed = currentTime - c.spawn_time;
            if (timePassed >= APPEAR_DURATION) {
                c.state = CometState::MOVING;
                c.brightness = 255;
                c.last_move_time = currentTime;
            } else {
                c.brightness = map(timePassed, 0, APPEAR_DURATION, 0, 255);
            }
        } else if (c.state == CometState::MOVING) {
            if (currentTime - c.last_move_time >= c.speed_ms) {
                c.last_move_time += c.speed_ms;
                c.position += c.direction;

                if (c.current_tail_len < tail_length) {
                    c.current_tail_len++;
                }
                if ((c.direction == 1 && c.position >= _numLeds + tail_length) ||
                    (c.direction == -1 && c.position < 0 - tail_length)) {
                    c.state = CometState::INACTIVE;
                }
            }
        }
    }
}

void SavaLED_ESP32::_drawComets(uint32_t background_color, uint8_t tail_length) {
    fill(background_color); // Рисуем фон

    for (int i = 0; i < SAVA_MAX_COMETS; i++) {
        if (_comets[i].state != CometState::INACTIVE) {
            SavaComet& c = _comets[i];
            
            // Рисуем голову
            if (c.position >= 0 && c.position < _numLeds) {
                setPixelColor(c.position, c.color, c.brightness);
            }
            
            // Рисуем хвост
            if (c.state == CometState::MOVING) {
                for (int j = 0; j < c.current_tail_len; j++) {
                    int tailPos = c.position - (c.direction * (j + 1));
                    if (tailPos >= 0 && tailPos < _numLeds) {
                        // Яркость хвоста убывает. Вместо TAIL_BRIGHTNESS[j] используем простую математику
                        uint8_t tailBrightness = 255 * (c.current_tail_len - j) / c.current_tail_len;
                        // += для смешивания цветов, если пиксели пересекаются
                        // Для этого нужно будет сначала прочитать цвет пикселя, а потом добавить.
                        // Для простоты пока просто рисуем поверх (setPixelColor)
                        setPixelColor(tailPos, c.color, tailBrightness);
                    }
                }
            }
        }
    }
}

