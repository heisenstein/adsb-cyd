#pragma once

// WiFi credentials
#define WIFI_SSID "squirrel_net"
#define WIFI_PASS "LFGM2026!"

// Home location
// Newark Airport (EWR/KEWR) coordinates
#define HOME_LAT 40.6925
#define HOME_LON -74.1687

// ADS-B settings — reduced for CYD (no PSRAM, 320KB DRAM)
#define ADSB_RADIUS_NM 25
#define ADSB_POLL_INTERVAL_MS 15000
#define MAX_AIRCRAFT 50
#define TRAIL_LENGTH 15

// CYD display
#define LCD_H_RES 320
#define LCD_V_RES 240

// CYD touch calibration (landscape rotation 1)
#define TOUCH_X_MIN 2600
#define TOUCH_X_MAX 1100
#define TOUCH_Y_MIN 3950
#define TOUCH_Y_MAX 280
