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
static const int DATA_PIN = 32;  // USB D-;
static const int CLOCK_PIN = 33; // USB D+;
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
  vga.init(vga.MODE320x240.custom(256, 192), RED_PIN, GREEN_PIN, BLUE_PIN, HSYNC_PIN, VSYNC_PIN);
  Serial.printf("HEAP after vga  %d \n", ESP.getFreeHeap());
  vga.clear(0);
  kbd.begin();
  kbd.enaIRQ();
  SPI.begin(SCK, MISO, MOSI, CS);
  if (!SD.begin(CS, SPI, 80000000))
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
 
  Serial.println(F("Inizializing FS..."));
  mount_spiffs();

  // Get all information of SPIFFS

  unsigned int totalBytes = SPIFFS.totalBytes();
  unsigned int usedBytes = SPIFFS.usedBytes();

  Serial.println("===== File system info =====");

  Serial.print("Total space:      ");
  Serial.print(totalBytes);
  Serial.println(" byte");

  Serial.print("Total space used: ");
  Serial.print(usedBytes);
  Serial.println(" byte");

  Serial.println();

  // Open dir folder
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

void do_keyboard()
{
  uint32_t scancode = 0;
  bool bAan = false;
  uint8_t scancode_LOW;
  uint8_t scancode_MED;
  uint8_t keyt;
  uint8_t scancode_HIGH = kbd.read();
  delay(5);
  if (kbd.available())
  {
    scancode_LOW = kbd.read();
    scancode = scancode_HIGH << 8 | scancode_LOW;
    // Serial.print("Keyb OF F: ");    // hier kan ook de E0 komen. F0 zet toets uit, E0 zet de key aan.
    // Serial.println(scancode, HEX); // als de low een F0 is, dan nikx doen... is extended code
    if (scancode_HIGH == 0xf0)
    {
      bAan = false;        // de toets mag uit
      keyt = scancode_LOW; // hier staat de goede waarde.
      // Serial.printf("key: %x, aan: %d\r\n", keyt, bAan);
    }
    if (scancode_HIGH == 0xe0 && scancode_LOW != 0xf0)
    {
      bAan = true;
      keyt = scancode_LOW; // hier staat de goede waarde.
      // Serial.printf("key: %x, aan: %d\r\n", keyt, bAan);
    }
  }
  else
  {
    scancode = scancode_HIGH;
    // Serial.print("Keyb ON: "); // hier kan de key gewoon gestuurd worden
    bAan = true;
    keyt = scancode;
    // Serial.printf("key: %x, aan: %d\r\n", keyt, bAan);
  }
  if (kbd.available())
  {
    scancode_MED = kbd.read(); // hier wordt de OFF overruled. De Scancode med zetten we uit...
    scancode = scancode_HIGH << 16 | scancode_LOW << 8 | scancode_MED;
    // Serial.print("Keyb OFF2: ");
    if (scancode_HIGH == 0xe0 && scancode_LOW == 0xf0)
    {
      bAan = false;
      keyt = scancode_MED; // hier staat de goede waarde.
      // Serial.printf("key: %x, aan: %d\r\n", keyt, bAan);
    }
  }
  // Serial.println(scancode, HEX);
  if (keyt < 128)
  {
    key[keyt] = bAan;
    switch (keyt)
    {
    case 0x07:
      atom_reset(0);
    case 0x11:
      alt = bAan;
      break;
    case 0x12:
    case 0x59:
      shift = bAan;
      break;
    case 0x14:
      ctrl = bAan;
      break;
    case 0x6b:
      key[0x74] = bAan;
      shift = bAan;
      break;
    case 0x75:
      key[0x72] = bAan;
      shift = bAan;
      break;
    }
  }
}

void loop()
{
  // static byte last_ts = 0;
  unsigned long ts1, ts2;

  ts1 = millis();
  atom_run();
  ts2 = millis();
  while (kbd.available())
  {
    do_keyboard();
  }

  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;
  vTaskDelay(0); // important to avoid task watchdog timeouts - change this to slow down emu
}

void fastBox(int x, int y, int l, int b, int kleur)
{
  for (int i = 0; i < l; i++)
  {
    for (int j = 0; j < b; j++)
    {
      vga.dotFast(i + y, j + x, kleur);
    }
  }
}

void SetTxt(int x, int y, int ch)
{
  if (ch & 0x40)
  {
    int kleur;

    if (ch & 0x80)
    {
      kleur = RED;
    }
    else
    {
      kleur = YELLOW;
    }
    if (ch & 0x01)
    {
      fastBox(x + 8, y + 4, 4, 4, kleur);
    }
    else
    {
      fastBox(x + 8, y + 4, 4, 4, BLACK);
    }
    if (ch & 0x02)
      fastBox(x + 8, y + 0, 4, 4, kleur);
    else
    {
      fastBox(x + 8, y + 0, 4, 4, BLACK);
    }
    if (ch & 0x04)
      fastBox(x + 4, y + 4, 4, 4, kleur);
    else
    {
      fastBox(x + 4, y + 4, 4, 4, BLACK);
    }
    if (ch & 0x08)
      fastBox(x + 4, y + 0, 4, 4, kleur);
    else
    {
      fastBox(x + 4, y + 0, 4, 4, BLACK);
    }
    if (ch & 0x10)
      fastBox(x + 0, y + 4, 4, 4, kleur);
    else
    {
      fastBox(x + 0, y + 4, 4, 4, BLACK);
    }
    if (ch & 0x20)
      fastBox(x + 0, y + 0, 4, 4, kleur);
    else
    {
      fastBox(x + 0, y + 0, 4, 4, BLACK);
    }
  }
  else
  {
    const unsigned char *pix = &fontdata[(ch & 0x3f) * 12];
    for (int px = 0; px < 12; px++)
    {
      for (int py = 0; py < 8; py++)
      {
        if (ch & 0x80)
        { // hoogste bit is gezet
          if (*(pix) & (1 << (7 - py)))
            vga.dotFast(py + y, px + x, BLACK);
          else
            vga.dotFast(py + y, px + x, GREEN);
        }
        else
        {
          if (*(pix) & (1 << (7 - py)))
            vga.dotFast(py + y, px + x, GREEN);
          else
            vga.dotFast(py + y, px + x, BLACK);
        }
      }
      pix++;
    }
  }
}

void dotFast(int x, int y, int kleur)
{
  int tab[] = {GREEN, YELLOW, BLUE, RED, BLACK, WHITE};
  if (kleur < 6)
  {
    vga.dotFast(x, y, tab[kleur]);
  }
}

void drawl(int line, int och, int ch, int pos)
{
  vga.drawli(line, och, ch, pos);
}

