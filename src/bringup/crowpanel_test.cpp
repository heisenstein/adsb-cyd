// Bring-up sketch for the CrowPanel Advance 5.0": checks PSRAM, display, backlight, touch.
// Built only by the crowpanel5 environment (see platformio.ini).
#include <Arduino.h>
#include "../display/lgfx_crowpanel5.h"

static LGFX lcd;

static void draw_test_screen() {
    const int w = lcd.width(), h = lcd.height();
    lcd.fillScreen(TFT_BLACK);

    // Colour bars: wrong channel order or missing bits show up immediately
    const uint16_t bars[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_WHITE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA};
    int bw = w / 7;
    for (int i = 0; i < 7; i++) lcd.fillRect(i * bw, 0, bw, 60, bars[i]);

    // Border at the very edge: reveals shifted or clipped images
    lcd.drawRect(0, 0, w, h, TFT_WHITE);
    lcd.drawRect(1, 1, w - 2, h - 2, TFT_WHITE);

    // Quarter-screen touch hot-zone preview
    lcd.drawFastVLine(w / 3, 60, h - 60, TFT_DARKGREY);
    lcd.drawFastVLine(w * 2 / 3, 60, h - 60, TFT_DARKGREY);

    lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    lcd.setTextDatum(top_left);
    lcd.setFont(&fonts::Font4);
    lcd.setCursor(12, 76);
    lcd.printf("CrowPanel 5.0 bring-up  %dx%d", w, h);
    lcd.setCursor(12, 108);
    lcd.printf("PSRAM free: %u KB", (unsigned)(ESP.getFreePsram() / 1024));
    lcd.setCursor(12, 140);
    lcd.print("Touch the screen");
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println("\nCrowPanel Advance 5.0 bring-up");
    Serial.printf("PSRAM: %u KB total, %u KB free\n",
                  (unsigned)(ESP.getPsramSize() / 1024), (unsigned)(ESP.getFreePsram() / 1024));

    lcd.init();
    lcd.setRotation(0);
    crowpanel_set_backlight(100);
    draw_test_screen();
}

void loop() {
    static int px = -1, py = -1;
    static uint32_t last_print = 0;

    lgfx::touch_point_t tp;
    if (lcd.getTouch(&tp)) {
        // Erase the previous marker, draw the new one, show coordinates
        if (px >= 0) lcd.fillCircle(px, py, 14, TFT_BLACK);
        px = tp.x;
        py = tp.y;
        lcd.fillCircle(px, py, 12, TFT_ORANGE);
        lcd.setTextColor(TFT_WHITE, TFT_BLACK);
        lcd.setFont(&fonts::Font4);
        lcd.setCursor(12, 180);
        lcd.printf("x=%3d y=%3d   ", px, py);
        if (millis() - last_print > 200) {
            last_print = millis();
            Serial.printf("touch x=%d y=%d\n", px, py);
        }
    }
    delay(10);
}
