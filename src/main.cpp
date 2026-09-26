#include <Arduino.h>
// #include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "SPIFFS.h"

#include "def/msg.h"
#include "def\hardware.h"

#include <ESP32Lib.h>

#include "def/hardware.h"
#include <Arduino.h>
#include <string.h>
#include "atommc/diskio.h"
#include "atommc/ff.h"
#include "atommc/atmmc2.h"
#include "atommc/atmmc2def.h"
#include "atommc/wildcard.h"
#include "atommc/atmmc2io.h"
#include "atom.h"
#include "roms.h"
#include "FS.h"

#include <esp_bt.h>
#include "driver/timer.h"
#include "soc/timer_group_struct.h"
#include "atom.h"
#include "PS2Keyboard.h"

extern int fileOpen(char *);
extern int filenum;
// extern uint8_t *rom;
File dir;
int numTabs = 1;

// void listFilesInDir(File, int);

// Pins for PS/2 Interface (some USB keyboards WORK)
static const int DATA_PIN = 32;
static const int CLOCK_PIN = 33;
extern unsigned int shift, ctrl, alt;
PS2Keyboard kbd(DATA_PIN, CLOCK_PIN);

uint8_t fontdata[] =
    {
        0x00,
        0x00,
        0x00,
        0x1c,
        0x22,
        0x02,
        0x1a,
        0x2a,
        0x2a,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x14,
        0x22,
        0x22,
        0x3e,
        0x22,
        0x22,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3c,
        0x12,
        0x12,
        0x1c,
        0x12,
        0x12,
        0x3c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1c,
        0x22,
        0x20,
        0x20,
        0x20,
        0x22,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3c,
        0x12,
        0x12,
        0x12,
        0x12,
        0x12,
        0x3c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3e,
        0x20,
        0x20,
        0x3c,
        0x20,
        0x20,
        0x3e,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3e,
        0x20,
        0x20,
        0x3c,
        0x20,
        0x20,
        0x20,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1e,
        0x20,
        0x20,
        0x26,
        0x22,
        0x22,
        0x1e,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x22,
        0x22,
        0x22,
        0x3e,
        0x22,
        0x22,
        0x22,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1c,
        0x08,
        0x08,
        0x08,
        0x08,
        0x08,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x02,
        0x02,
        0x02,
        0x02,
        0x22,
        0x22,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x22,
        0x24,
        0x28,
        0x30,
        0x28,
        0x24,
        0x22,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x20,
        0x20,
        0x20,
        0x20,
        0x20,
        0x20,
        0x3e,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x22,
        0x36,
        0x2a,
        0x2a,
        0x22,
        0x22,
        0x22,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x22,
        0x32,
        0x2a,
        0x26,
        0x22,
        0x22,
        0x22,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3e,
        0x22,
        0x22,
        0x22,
        0x22,
        0x22,
        0x3e,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3c,
        0x22,
        0x22,
        0x3c,
        0x20,
        0x20,
        0x20,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1c,
        0x22,
        0x22,
        0x22,
        0x2a,
        0x24,
        0x1a,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3c,
        0x22,
        0x22,
        0x3c,
        0x28,
        0x24,
        0x22,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1c,
        0x22,
        0x10,
        0x08,
        0x04,
        0x22,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3e,
        0x08,
        0x08,
        0x08,
        0x08,
        0x08,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x22,
        0x22,
        0x22,
        0x22,
        0x22,
        0x22,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x22,
        0x22,
        0x22,
        0x14,
        0x14,
        0x08,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x22,
        0x22,
        0x22,
        0x2a,
        0x2a,
        0x36,
        0x22,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x22,
        0x22,
        0x14,
        0x08,
        0x14,
        0x22,
        0x22,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x22,
        0x22,
        0x14,
        0x08,
        0x08,
        0x08,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3e,
        0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x3e,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x38,
        0x20,
        0x20,
        0x20,
        0x20,
        0x20,
        0x38,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x20,
        0x20,
        0x10,
        0x08,
        0x04,
        0x02,
        0x02,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x0e,
        0x02,
        0x02,
        0x02,
        0x02,
        0x02,
        0x0e,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x1c,
        0x2a,
        0x08,
        0x08,
        0x08,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x10,
        0x3e,
        0x10,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x08,
        0x08,
        0x08,
        0x08,
        0x00,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x14,
        0x14,
        0x14,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x14,
        0x14,
        0x36,
        0x00,
        0x36,
        0x14,
        0x14,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x1e,
        0x20,
        0x1c,
        0x02,
        0x3c,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x32,
        0x32,
        0x04,
        0x08,
        0x10,
        0x26,
        0x26,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x10,
        0x28,
        0x28,
        0x10,
        0x2a,
        0x24,
        0x1a,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x18,
        0x18,
        0x18,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x10,
        0x20,
        0x20,
        0x20,
        0x10,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x04,
        0x02,
        0x02,
        0x02,
        0x04,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x1c,
        0x3e,
        0x1c,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x08,
        0x3e,
        0x08,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x30,
        0x30,
        0x10,
        0x20,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3e,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x30,
        0x30,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x02,
        0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x20,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x18,
        0x24,
        0x24,
        0x24,
        0x24,
        0x24,
        0x18,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x18,
        0x08,
        0x08,
        0x08,
        0x08,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1c,
        0x22,
        0x02,
        0x1c,
        0x20,
        0x20,
        0x3e,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1c,
        0x22,
        0x02,
        0x0c,
        0x02,
        0x22,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x04,
        0x0c,
        0x14,
        0x3e,
        0x04,
        0x04,
        0x04,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3e,
        0x20,
        0x3c,
        0x02,
        0x02,
        0x22,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1c,
        0x20,
        0x20,
        0x3c,
        0x22,
        0x22,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3e,
        0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x20,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1c,
        0x22,
        0x22,
        0x1c,
        0x22,
        0x22,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1c,
        0x22,
        0x22,
        0x1e,
        0x02,
        0x02,
        0x1c,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x18,
        0x18,
        0x00,
        0x18,
        0x18,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x18,
        0x18,
        0x00,
        0x18,
        0x18,
        0x08,
        0x10,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x04,
        0x08,
        0x10,
        0x20,
        0x10,
        0x08,
        0x04,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3e,
        0x00,
        0x3e,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x10,
        0x08,
        0x04,
        0x02,
        0x04,
        0x08,
        0x10,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x18,
        0x24,
        0x04,
        0x08,
        0x08,
        0x00,
        0x08,
        0x00,
        0x00,
};

extern void initmem();
extern void loadroms();
extern void reset6502();
// extern void initvideo();
extern unsigned int shift;
// void mount_spiffs();

extern void init8255();
int debugon = true;
extern void resetvia();
//
extern void atom_reset(int);
extern void atom_run();

uint8_t irq_num;

byte key[128];

// SETUP *************************************
VGA3BitI vga;

void IRAM_ATTR mount_spiffs()
{
  if (!SPIFFS.begin())
  {
    // errorHalt(ERR_MOUNT_FAIL);
  }
  vTaskDelay(2);
}

void swap_flash(word *a, word *b)
{
  word temp = *a;
  *a = *b;
  *b = temp;
}

// void list()
// {
//   Serial.println(__func__);
//   Serial.print("List Dir Dirwaarde: ");
//   Serial.println(dir);
//   // listFilesInDir(dir, 1);
//   Serial.println(__LINE__);
// }

void setup()
{
  // Turn off peripherals to gain memory (?do they release properly)
  //  esp_bt_controller_deinit();
  //  esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);
  Serial.begin(115200);

  Serial.printf("HEAP BEGIN %d\n", ESP.getFreeHeap());
  vga.setFrameBufferCount(1);
  // vga.init(vga.MODE320x240, RED_PIN, GREEN_PIN, BLUE_PIN, HSYNC_PIN, VSYNC_PIN);
  // vga.init(vga.MODE320x240.custom(256, 192), RED_PIN, GREEN_PIN, BLUE_PIN, HSYNC_PIN, VSYNC_PIN);
  // 1. Maak de aangepaste 256x192 Mode aan op basis van de 320x240 timing auto myMode = vga.MODE320x240.custom(256, 192);
  auto myMode = vga.MODE320x240.custom(264, 192);

  const int shiftX = 4; // Probeer stapjes van 4 of 8

  myMode.hBack += shiftX;
  myMode.hFront -= shiftX;

  vga.init(myMode, RED_PIN, GREEN_PIN, BLUE_PIN, HSYNC_PIN, VSYNC_PIN);
  Serial.printf("HEAP after vga  %d \n", ESP.getFreeHeap());
  vga.clear(0);
  kbd.begin();
  kbd.enaIRQ();
  SPI.begin(SCK, MISO, MOSI, CS);
  if (!SD.begin(CS, SPI, 20000000))
  {
    Serial.println("SD card mount failed");
  }
  else
  {
    Serial.println("SD card mounted");
  }
  Serial.printf("%s bank %u: %ub\n", MSG_FREE_HEAP_AFTER, 0, ESP.getFreeHeap());

  Serial.printf("%s %u\n", MSG_EXEC_ON_CORE, xPortGetCoreID());
  Serial.printf("%s 6502 RESET: %ub\n", MSG_FREE_HEAP_AFTER, ESP.getFreeHeap());

  Serial.printf("HEAP after vga  %d \n", ESP.getFreeHeap());
  dir = SD.open("/");

  initmem();
  Serial.printf("HEAP after initmem  %d \n", ESP.getFreeHeap());
  loadroms();
  Serial.printf("HEAP after loadroms  %d \n", ESP.getFreeHeap());
  reset6502();
  Serial.printf("HEAP after reset6502  %d \n", ESP.getFreeHeap());
  init8255();
  debugon = false;
  resetvia();
  //
  atom_reset(0);
  Serial.print("Setup: MAIN Executing on core ");
  Serial.println(xPortGetCoreID());

  for (int c = 0; c < 128; c++)
  {
    keylookup[c] = c;
    key[c] = 0;
  }
  Serial.println("End of setup");
}

#if 0
void do_keyboard()
{
  uint32_t scancode = 0;
  bool bAan = false;
  uint8_t scancode_LOW;
  uint8_t scancode_MED;
  void do_keyboard()
  {
    static bool keyReleased = false;
    static bool rawKeyDown[128] = {};
    uint8_t scancode = kbd.read();
    if (scancode == 0xe0)
      void do_keyboard()
      {
        static bool keyReleased = false;
        static bool rawKeyDown[128] = {};
        uint8_t scancode = kbd.read();
        if (scancode == 0xe0)
          return;
        if (scancode == 0xf0)
        {
          keyReleased = true;
          return;
        }

        bool keyDown = !keyReleased;
        keyReleased = false;
        if (scancode >= 128)
          return;

        bool wasDown = rawKeyDown[scancode];
        rawKeyDown[scancode] = keyDown;
        bool physicalShift = rawKeyDown[0x12] || rawKeyDown[0x59];
        bool syntheticShift = false;
        bool suppressShift = false;
        if (scancode == 0x07 && keyDown && !wasDown)
          atom_reset(0);

        memset(key, 0, sizeof(key[0]) * 128);
        for (int sourceCode = 0; sourceCode < 128; sourceCode++)
        {
          if (!rawKeyDown[sourceCode])
            continue;

          int matrixCode = sourceCode;
          if (sourceCode == KEY_TAB)
            matrixCode = ATOM_MATRIX_COPY_ID;
          else if (physicalShift && sourceCode == KEY_2)
          {
            matrixCode = 0x55;
            suppressShift = true;
          }
          else if (physicalShift && sourceCode == KEY_8)
            matrixCode = KEY_MONKEYTALE;
          else if (physicalShift && sourceCode == KEY_9)
            matrixCode = KEY_9;
          else if (physicalShift && sourceCode == KEY_0)
            matrixCode = KEY_0;
          else if (physicalShift && sourceCode == KEY_7)
            matrixCode = KEY_6;
          else if (sourceCode == KEY_SEMICOLON && physicalShift)
          {
            matrixCode = KEY_MONKEYTALE;
            suppressShift = true;
          }
          else if (sourceCode == KEY_MONKEYTALE)
          {
            matrixCode = physicalShift ? KEY_2 : KEY_7;
            syntheticShift = !physicalShift;
          }
          else if (sourceCode == 0x55)
          {
            if (physicalShift)
              matrixCode = KEY_SEMICOLON;
            else
            {
              matrixCode = KEY_MINUS;
              syntheticShift = true;
            }
          }
          key[matrixCode] = true;
        }

        key[KEY_UP] = rawKeyDown[0x75] || rawKeyDown[0x72];
        key[KEY_RIGHT] = rawKeyDown[0x74] || rawKeyDown[0x6b];
        shift = (!suppressShift && physicalShift) || syntheticShift || rawKeyDown[0x72] || rawKeyDown[0x6b];
        ctrl = rawKeyDown[0x14];
        alt = rawKeyDown[0x11];
      }
  atom_run();
  ts2 = millis();
  while (kbd.available())
    do_keyboard();
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;
  vTaskDelay(0);
}

#endif

void do_keyboard()
{
  static bool keyReleased = false;
  static bool rawKeyDown[128] = {};
  uint8_t scancode = kbd.read();
  if (scancode == 0xe0)
    return;
  if (scancode == 0xf0)
  {
    keyReleased = true;
    return;
  }

  bool keyDown = !keyReleased;
  keyReleased = false;
  if (scancode >= 128)
    return;

  bool wasDown = rawKeyDown[scancode];
  rawKeyDown[scancode] = keyDown;
  bool physicalShift = rawKeyDown[0x12] || rawKeyDown[0x59];
  bool syntheticShift = false;
  bool suppressShift = false;
  if (scancode == 0x07 && keyDown && !wasDown)
    atom_reset(0);

  memset(key, 0, sizeof(key[0]) * 128);
  for (int sourceCode = 0; sourceCode < 128; sourceCode++)
  {
    if (!rawKeyDown[sourceCode])
      continue;
    int matrixCode = sourceCode;
    if (sourceCode == KEY_TAB)
      matrixCode = ATOM_MATRIX_COPY_ID;
    else if (physicalShift && sourceCode == KEY_2)
    {
      matrixCode = 0x55;
      suppressShift = true;
    }
    else if (physicalShift && sourceCode == KEY_8)
      matrixCode = KEY_MONKEYTALE;
    else if (physicalShift && sourceCode == KEY_9)
      matrixCode = KEY_9;
    else if (physicalShift && sourceCode == KEY_0)
      matrixCode = KEY_0;
    else if (physicalShift && sourceCode == KEY_7)
      matrixCode = KEY_6;
    else if (sourceCode == KEY_SEMICOLON && physicalShift)
    {
      matrixCode = KEY_MONKEYTALE;
      suppressShift = true;
    }
    else if (sourceCode == KEY_MONKEYTALE)
    {
      matrixCode = physicalShift ? KEY_2 : KEY_7;
      syntheticShift = !physicalShift;
    }
    else if (sourceCode == 0x55)
    {
      if (physicalShift)
        matrixCode = KEY_SEMICOLON;
      else
      {
        matrixCode = KEY_MINUS;
        syntheticShift = true;
      }
    }
    key[matrixCode] = true;
  }

  key[KEY_UP] = rawKeyDown[0x75] || rawKeyDown[0x72];
  key[KEY_RIGHT] = rawKeyDown[0x74] || rawKeyDown[0x6b];
  shift = (!suppressShift && physicalShift) || syntheticShift || rawKeyDown[0x72] || rawKeyDown[0x6b];
  ctrl = rawKeyDown[0x14];
  alt = rawKeyDown[0x11];
}

void loop()
{
  unsigned long ts1, ts2;
  ts1 = millis();
  atom_run();
  ts2 = millis();
  while (kbd.available())
    do_keyboard();
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;
  vTaskDelay(0);
}

void fastBox(int x, int y, int l, int b, int kleur)
{
  if (l <= 0 || b <= 0)
    return;

  int left = x < 0 ? 0 : x;
  int top = y < 0 ? 0 : y;
  int right = x + b > vga.xres ? vga.xres : x + b;
  int bottom = y + l > vga.yres ? vga.yres : y + l;
  int clippedWidth = right - left;
  if (clippedWidth <= 0 || bottom <= top)
    return;

  if ((left & 1) == 0 && (clippedWidth & 1) == 0)
  {
    uint8_t packedColor = (kleur & 0x0f) * 0x11;
    int firstByte = left >> 1;
    for (int rowIndex = top; rowIndex < bottom; rowIndex++)
    {
      uint8_t *row = vga.backBuffer[rowIndex];
      for (int byteIndex = 0; byteIndex < clippedWidth / 2; byteIndex++)
        row[firstByte + byteIndex] = packedColor;
    }
    return;
  }

  for (int row = top; row < bottom; row++)
    for (int column = left; column < right; column++)
      vga.dotFast(row, column, kleur);
}

void SetTxt(int x, int y, int ch)
{
  if (x < 0 || y < 0 || x + 8 > vga.xres || y + 12 > vga.yres)
    return;

  if (ch & 0x40)
  {
    int color = (ch & 0x80) ? RED : YELLOW;
    for (int bit = 0; bit < 6; bit++)
    {
      int blockX = (bit & 1) ? 0 : 4;
      int blockY = 8 - (bit / 2) * 4;
      fastBox(x + blockX, y + blockY, 4, 4, (ch & (1 << bit)) ? color : BLACK);
    }
  }
  else
  {
    const unsigned char *pix = &fontdata[(ch & 0x3f) * 12];
    uint8_t foreground = (ch & 0x80) ? BLACK : GREEN;
    uint8_t background = (ch & 0x80) ? GREEN : BLACK;
    if ((x & 1) == 0)
    {
      for (int py = 0; py < 12; py++)
      {
        uint8_t *row = vga.backBuffer[y + py];
        for (int px = 0; px < 8; px += 2)
        {
          uint8_t leftColor = (pix[py] & (0x80 >> px)) ? foreground : background;
          uint8_t rightColor = (pix[py] & (0x80 >> (px + 1))) ? foreground : background;
          row[(x + px) >> 1] = leftColor | (rightColor << 4);
        }
      }
    }
    else
    {
      for (int py = 0; py < 12; py++)
      {
        for (int px = 0; px < 8; px++)
        {
          bool pixel = (pix[py] & (0x80 >> px)) != 0;
          vga.dotFast(x + px, y + py, pixel ? foreground : background);
        }
      }
    }
  }
}

void dotFast(int x, int y, int kleur)
{
  int tab[] = {GREEN, YELLOW, BLUE, RED, BLACK, WHITE};
  if (kleur < 6)
    vga.dotFast(x, y, tab[kleur]);
}

void drawl(int line, int och, int ch, int pos)
{
  vga.drawli(line, och, ch, pos);
}
