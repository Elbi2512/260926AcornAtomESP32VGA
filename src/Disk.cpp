#include <Arduino.h>
#include "def\hardware.h"
/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-microsd-card-arduino/
  
  This sketch was mofidied from: Examples > SD(esp32) > SD_Test
*/

#include <FS.h>
#include <SPIFFS.h>
#include "PS2keyboard.h"
//#include "SD.h"
//#include "SPI.h"
extern uint8_t irq_num;

void errorHalt(String errormsg);
//void IRAM_ATTR kb_interruptHandler(void);

extern PS2Keyboard kbd;

#define SCK 14
#define MISO 2
#define MOSI 12
#define CS 13

/*String getFileEntriesFromDir(String path)
{
    KB_INT_STOP;
    Serial.printf("Getting entries from: '%s'\n", path.c_str());
    String filelist;
    File root = SPIFFS.open(path.c_str());
    if (!root || !root.isDirectory())
    {
        errorHalt((String)ERR_DIR_OPEN + "\n" + root);
    }
    File file = root.openNextFile();
    if (!file)
        Serial.println("No entries found!");
    while (file)
    {
        Serial.printf("Found %s: %s...%ub...", (file.isDirectory() ? "DIR" : "FILE"), file.name(), file.size());
        String filename = file.name();
        byte start = filename.indexOf("/", path.length()) + 1;
        byte end = filename.indexOf("/", start);
        filename = filename.substring(start, end);
        Serial.printf("%s...", filename.c_str());
        if (filename.startsWith("."))
        {
            Serial.println("HIDDEN");
        }
        else if (cfg_arch == "48K" & file.size() > SIZE48K)
        {
            Serial.println("128K SKIP");
        }
        else
        {
            if (filelist.indexOf(filename) < 0)
            {
                Serial.println("ADDING");
                filelist += filename + "\n";
            }
            else
            {
                Serial.println("EXISTS");
            }
        }
        file = root.openNextFile();
    }
    KB_INT_START;
    return filelist;
} 

unsigned short countFileEntriesFromDir(String path)
{
    String entries = getFileEntriesFromDir(path);
    unsigned short count = 0;
    for (unsigned short i = 0; i < entries.length(); i++)
    {
        if (entries.charAt(i) == ASCII_NL)
        {
            count++;
        }
    }
    return count;
}

String getAllFilesFrom(const String path)
{
    KB_INT_STOP;
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    String listing;

    while (file)
    {
        file = root.openNextFile();
        String filename = file.name();
        if (filename.startsWith(path) && !filename.startsWith(path + "/."))
        {
            listing.concat(filename.substring(path.length() + 1));
            listing.concat("\n");
        }
    }
    vTaskDelay(2);
    KB_INT_START;
    return listing;
} */

void listAllFiles()
{
    kbd.disIRQ();
    File root = SPIFFS.open("/");
    Serial.println("fs opened");
    File file = root.openNextFile();
    Serial.println("fs openednextfile");

    while (file)
    {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
    vTaskDelay(2);
    kbd.enaIRQ();
}
/*
File open_read_file(String filename)
{
    File f;
    filename.replace("\n", " ");
    filename.trim();
    if (cfg_slog_on)
        Serial.printf("%s '%s'\n", MSG_LOADING, filename.c_str());
    if (!SPIFFS.exists(filename.c_str()))
    {
        KB_INT_START;
        errorHalt((String)ERR_READ_FILE + "\n" + filename);
    }
    f = SPIFFS.open(filename.c_str(), FILE_READ);
    vTaskDelay(2);

    return f;
}
*/
/* void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
*/
void createDir(fs::FS &fs, const char *path)
{
    Serial.println(__func__);
    Serial.printf("Creating Dir: %s\n", path);
    kbd.disIRQ();
    if (fs.mkdir(path))
    {
        Serial.println("Dir created");
    }
    else
    {
        Serial.println("mkdir failed");
    }
    kbd.enaIRQ();
}

void removeDir(fs::FS &fs, const char *path)
{
    Serial.println(__func__);
    Serial.printf("Removing Dir: %s\n", path);
    kbd.disIRQ();
    if (fs.rmdir(path))
    {
        Serial.println("Dir removed");
    }
    else
    {
        Serial.println("rmdir failed");
    }
    kbd.enaIRQ();
}

void readFile(fs::FS &fs, const char *path)
{
    Serial.println(__func__);
    Serial.printf("Reading file: %s\n", path);
    kbd.disIRQ();
    File file = fs.open(path);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available())
    {
        Serial.write(file.read());
    }
    file.close();
    kbd.enaIRQ();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.println(__func__);

    Serial.printf("Writing file: %s\n", path);
    kbd.disIRQ();
    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("File written");
    }
    else
    {
        Serial.println("Write failed");
    }
    file.close();
    kbd.enaIRQ();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.println(__func__);
    Serial.printf("Appending to file: %s\n", path);
    kbd.disIRQ();
    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("Message appended");
    }
    else
    {
        Serial.println("Append failed");
    }
    file.close();
    kbd.enaIRQ();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
    Serial.println(__func__);
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    kbd.disIRQ();
    if (fs.rename(path1, path2))
    {
        Serial.println("File renamed");
    }
    else
    {
        Serial.println("Rename failed");
    }
    kbd.enaIRQ();
}

void deleteFile(fs::FS &fs, const char *path)
{
    Serial.println(__func__);
    Serial.printf("Deleting file: %s\n", path);
    kbd.disIRQ();
    if (fs.remove(path))
    {
        Serial.println("File deleted");
    }
    else
    {
        Serial.println("Delete failed");
    }
    kbd.enaIRQ();
}

void testFileIO(fs::FS &fs, const char *path)
{
    Serial.println(__func__);
    kbd.disIRQ();
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file)
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len)
        {
            size_t toRead = len;
            if (toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    }
    else
    {
        Serial.println("Failed to open file for reading");
    }

    file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++)
    {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
    kbd.enaIRQ();
}

/*
#include "Emulator/Keyboard/PS2Kbd.h"
#include "Emulator/Memory.h"
#include "def/ascii.h"
#include "def/files.h"
#include "def/msg.h"
#include "def/types.h"
#include <FS.h>
#include <SPIFFS.h>

void errorHalt(String errormsg);
//void IRAM_ATTR kb_interruptHandler(void);

// Globals

String cfg_arch = "128K";
String cfg_ram_file = NO_RAM_FILE;
String cfg_rom_set = "SINCLAIR";
String cfg_sna_file_list;
boolean cfg_slog_on = true;
boolean cfg_wconn = false;
String cfg_wssid = "none";
String cfg_wpass = "none";

void IRAM_ATTR mount_spiffs()
{
    if (!SPIFFS.begin())
        errorHalt(ERR_MOUNT_FAIL);

    vTaskDelay(2);
}

String getAllFilesFrom(const String path)
{
    //KB_INT_STOP;
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    String listing;

    while (file)
    {
        file = root.openNextFile();
        String filename = file.name();
        if (filename.startsWith(path) && !filename.startsWith(path + "/."))
        {
            listing.concat(filename.substring(path.length() + 1));
            listing.concat("\n");
        }
    }
    vTaskDelay(2);
    return listing;
}

void listAllFiles()
{
    File root = SPIFFS.open("/");
    Serial.println("fs opened");
    File file = root.openNextFile();
    Serial.println("fs openednextfile");

    while (file)
    {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
    vTaskDelay(2);
}

File open_read_file(String filename)
{
    File f;
    filename.replace("\n", " ");
    filename.trim();
    if (cfg_slog_on)
        Serial.printf("%s '%s'\n", MSG_LOADING, filename.c_str());
    if (!SPIFFS.exists(filename.c_str()))
    {
        errorHalt((String)ERR_READ_FILE + "\n" + filename);
    }
    f = SPIFFS.open(filename.c_str(), FILE_READ);
    vTaskDelay(2);

    return f;
}
String getFileEntriesFromDir(String path)
{
    //KB_INT_STOP;
    //KB_INT_STOP;
    Serial.printf("Getting entries from: '%s'\n", path.c_str());
    String filelist;
    File root = SPIFFS.open(path.c_str());
    if (!root || !root.isDirectory())
    {
        errorHalt((String)ERR_DIR_OPEN + "\n" + root);
    }
    File file = root.openNextFile();
    if (!file)
        Serial.println("No entries found!");
    while (file)
    {
        Serial.printf("Found %s: %s...%ub...", (file.isDirectory() ? "DIR" : "FILE"), file.name(), file.size());
        String filename = file.name();
        byte start = filename.indexOf("/", path.length()) + 1;
        byte end = filename.indexOf("/", start);
        filename = filename.substring(start, end);
        Serial.printf("%s...", filename.c_str());
        if (filename.startsWith("."))
        {
            Serial.println("HIDDEN");
        }
        else if (cfg_arch == "48K" & file.size() > SIZE48K)
        {
            Serial.println("128K SKIP");
        }
        else
        {
            if (filelist.indexOf(filename) < 0)
            {
                Serial.println("ADDING");
                filelist += filename + "\n";
            }
            else
            {
                Serial.println("EXISTS");
            }
        }
        file = root.openNextFile();
    }
    //KB_INT_START;
    return filelist;
}

unsigned short countFileEntriesFromDir(String path)
{
    String entries = getFileEntriesFromDir(path);
    unsigned short count = 0;
    for (unsigned short i = 0; i < entries.length(); i++)
    {
        if (entries.charAt(i) == ASCII_NL)
        {
            count++;
        }
    }
    return count;
}
*/

/*
// hier oude zooi...

void load_rom(String arch, uint8_t *adres)
{
    String path = "/roms";
    //   +arch + ".rom";
    Serial.printf("Loading ROMSET '%s'\n", path.c_str());
    Serial.printf("Rombase: %X\r\n", adres);

    byte n_roms = countFileEntriesFromDir(path);
    if (n_roms < 1)
    {
        errorHalt("No ROMs found at " + path + "\nARCH: '" + arch);
    }
    Serial.printf("Processing %u ROMs\n", n_roms);
    for (byte f = 0; f < 1; f++)
    {
        File rom_f = open_read_file(path + "/" + (String)arch + ".rom");
        Serial.printf("Loading ROM '%s', size: %d\n", rom_f.name(), rom_f.size());
        for (int i = 0; i < rom_f.size(); i++)
        {
            adres[i] = rom_f.read();
            //Serial.printf("%X", adres[i]);
        }
        rom_f.close();
    }
}

void errorHalt(String errormsg)
{

    Serial.print(errormsg);

    while (1)
    {
        // do_keyboard();
        delay(5);
    }
}
*/