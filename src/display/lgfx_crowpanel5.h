#pragma once
// LovyanGFX config for the Elecrow CrowPanel Advance 5.0" (SKU DIS02050A):
// ESP32-S3-WROOM-1-N16R8, 800x480 parallel-RGB panel (ST7262), GT911 capacitive touch.
//
// Pins and timings come from Elecrow's repo / forum posts, not yet verified on
// hardware. If the picture is shifted, noisy or tearing, tune LCD_PCLK_HZ and the
// porch values first.

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>

#define LCD_PCLK_HZ 16000000  // 12-21 MHz; lower is steadier with WiFi + PSRAM traffic

// Backlight is not a GPIO: an on-board MCU at I2C 0x30 sets it (shares the touch I2C bus).
#define BL_I2C_ADDR 0x30
#define CROWPANEL_HW_V12 1  // board silkscreen revision: 1 = V1.2+, 0 = V1.1

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_RGB _panel_inst;
    lgfx::Bus_RGB _bus_inst;
    lgfx::Touch_GT911 _touch_inst;

public:
    LGFX() {
        {
            auto cfg = _panel_inst.config();
            cfg.memory_width = 800;
            cfg.memory_height = 480;
            cfg.panel_width = 800;
            cfg.panel_height = 480;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            _panel_inst.config(cfg);
        }
        {
            auto cfg = _panel_inst.config_detail();
            cfg.use_psram = 1;  // framebuffer lives in PSRAM
            _panel_inst.config_detail(cfg);
        }
        {
            auto cfg = _bus_inst.config();
            cfg.panel = &_panel_inst;
            // d0-d4 = B0-B4, d5-d10 = G0-G5, d11-d15 = R0-R4
            cfg.pin_d0 = GPIO_NUM_21;
            cfg.pin_d1 = GPIO_NUM_47;
            cfg.pin_d2 = GPIO_NUM_48;
            cfg.pin_d3 = GPIO_NUM_45;
            cfg.pin_d4 = GPIO_NUM_38;
            cfg.pin_d5 = GPIO_NUM_9;
            cfg.pin_d6 = GPIO_NUM_10;
            cfg.pin_d7 = GPIO_NUM_11;
            cfg.pin_d8 = GPIO_NUM_12;
            cfg.pin_d9 = GPIO_NUM_13;
            cfg.pin_d10 = GPIO_NUM_14;
            cfg.pin_d11 = GPIO_NUM_7;
            cfg.pin_d12 = GPIO_NUM_17;
            cfg.pin_d13 = GPIO_NUM_18;
            cfg.pin_d14 = GPIO_NUM_3;
            cfg.pin_d15 = GPIO_NUM_46;

            cfg.pin_henable = GPIO_NUM_42;
            cfg.pin_vsync = GPIO_NUM_41;
            cfg.pin_hsync = GPIO_NUM_40;
            cfg.pin_pclk = GPIO_NUM_39;
            cfg.freq_write = LCD_PCLK_HZ;

            cfg.hsync_polarity = 0;
            cfg.hsync_front_porch = 8;
            cfg.hsync_pulse_width = 4;
            cfg.hsync_back_porch = 8;
            cfg.vsync_polarity = 0;
            cfg.vsync_front_porch = 8;
            cfg.vsync_pulse_width = 4;
            cfg.vsync_back_porch = 8;
            // Same polarity as LovyanGFX's bundled Elecrow 5" (same ST7262 panel type)
            cfg.pclk_active_neg = 1;
            cfg.pclk_idle_high = 0;
            cfg.de_idle_high = 0;
            _bus_inst.config(cfg);
        }
        _panel_inst.setBus(&_bus_inst);
        {
            auto cfg = _touch_inst.config();
            cfg.x_min = 0;
            cfg.x_max = 799;
            cfg.y_min = 0;
            cfg.y_max = 479;
            cfg.pin_int = -1;
            cfg.pin_rst = -1;  // reset is handled by the on-board MCU
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;
            cfg.i2c_port = 0;
            cfg.i2c_addr = 0x5D;
            cfg.pin_sda = GPIO_NUM_15;
            cfg.pin_scl = GPIO_NUM_16;
            cfg.freq = 400000;
            _touch_inst.config(cfg);
            _panel_inst.setTouch(&_touch_inst);
        }
        setPanel(&_panel_inst);
    }
};

// 0 = off .. 100 = full. Call after lcd.init() so the I2C bus is already set up.
inline void crowpanel_set_backlight(int percent) {
    percent = constrain(percent, 0, 100);
    uint8_t v;
#if CROWPANEL_HW_V12
    // V1.2+: 0 = brightest, 244 = dimmest, 245 = off
    v = (percent == 0) ? 245 : (uint8_t)(244 - percent * 244 / 100);
#else
    // V1.1: 0x05 = off ... 0x10 = max (send 0x10 first to wake the panel)
    v = (percent == 0) ? 0x05 : (uint8_t)(0x06 + percent * 10 / 100);
#endif
    lgfx::i2c::transactionWrite(0, BL_I2C_ADDR, &v, 1, 400000);
}
