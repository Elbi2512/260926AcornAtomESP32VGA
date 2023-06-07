/*
	ff_emu.c
	
	Functions to emulate the FatFilesystem routines as used by AtoMMC.
	
	Note this is by no means a complete emulation of FATFS, but 
	replicates / emulates enough of the functionality to allow emulation
	of the AtoMMC interface firmware.
	
	I had to split the emulation over two files because of a clash of 
	structure names used by fatfs calls and the underlying os.
	
	2012-06-12, Phill Harvey-Smith.
*/

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h> // Required for OSX and lseek()
#include <FS.h>
#include <SPIFFS.h>

#include "atommc/integer.h"
#include "ff.h"
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include "atommc/ff_emudir.h"
#include "atom.h"
#include "PS2Keyboard.h"

// This sequence of defines is needed as MinGW specifically needs binary
// files to be opened with O_BINARY, which does not exist on other platforms.
#ifndef O_BINARY
#ifdef _O_BINARY
#define O_BINARY _O_BINARY
#else
#define O_BINARY 0
#endif
#endif

#define SHORT_NAME_LEN 12

extern PS2Keyboard kbd;
DIR dj;
EMUDIR emu;
File root;
char MMCPath[PATHSIZE + 1];
char BaseMMCPath[PATHSIZE + 1];
File *openfil;
char openname[SHORT_NAME_LEN + 1];
extern char globalData[256];
void HexDumpHead(char *, int);
// FRESULT f_open(File *, char *, BYTE);
#ifdef DEBUGFF
void HexDump(const void *Buff,
             int Length);

void HexDumpHead(const void *Buff,
                 int Length);

#endif

static BYTE file_exists(char name[])
{
    kbd.disIRQ();
    if (SPIFFS.exists(name) == true)
    {
        //Serial.printf("%s: File found %d\n", __func__, __LINE__);
        return FR_OK;
    }
    else
    {
        //Serial.printf("%s: File not found %d\n", __func__, __LINE__);
        return FR_NO_PATH;
    }
}

static void update_FIL(File *file,
                       int fp,
                       int updatefp)
{
    struct stat statbuf;
    /*
    if ((fp) || (0 == updatefp))
    {
        if (updatefp)
            fil->fs = (FATFS *)fp;
        else
            fp = (int)fil->fs;

        if (0 == fstat(fp, &statbuf))
        {
            fil->fsize = statbuf.st_size;
        }
        fil->fptr = lseek(fp, 0, SEEK_CUR);
    }
    */
}

static FRESULT get_result(int err_no)
{
    //debuglog("get_result errno=%d [%04X]\n",err_no,err_no);
    switch (err_no)
    {
    case ENOENT:
        return FR_NO_PATH;
    case EACCES:
        return FR_DENIED;
    case EBUSY:
        return FR_DENIED;
    case EFAULT:
    case EIO:
        return FR_DISK_ERR;

    default:
        return FR_OK;
    }
}
FRESULT f_chdrive(
    BYTE drv /* Drive number */
)
{
    return FR_OK;
}
/*
FRESULT f_mount(
    BYTE vol, // Logical drive number to be mounted/unmounted 
    FATFS *fs // Pointer to new file system object (NULL for unmount)
)
{
    return FR_OK;
}
*/

FRESULT f_chdir(
    char *path /* Pointer to the directory path */
)
{
    char newpath[PATHSIZE + 1];
    char fullpath[PATHSIZE + 1];
    FRESULT result = FR_NO_PATH;

    // add new directory to current
    snprintf(newpath, PATHSIZE, "%s/%s", MMCPath, path);

    // Resolve the newpath
    if (NULL != saferealpath(newpath, fullpath))
    {
        // Check that the new path is BELOW the base mmcpath
        if (0 == strncmp(BaseMMCPath, fullpath, strlen(BaseMMCPath)))
        {
            // And that it exists
            if (0 == access(fullpath, FR_OK))
            {
                strcpy(MMCPath, fullpath);
                result = FR_OK;
            }
        }
    }

    // //Serial.printf("f_chdir(path)\nnewpath=%s\nfullpath=%s\nbasepath=%s\n",path,newpath,fullpath,BaseMMCPath);

    return result;
}

FRESULT f_open(File *fp, char *path, char mode)
{
    //Serial.println(__func__);
    kbd.disIRQ();
    BYTE exists;
    char open_mode = 0;
    int newfile;
    FRESULT status;
    char tekst[20];

    // Get real path of file and check to see if it exists
    //  //Serial.printf("Fopenpath: %s/%s\n", (const char *)globalData, path);
    mode &= (FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW);
    //Serial.printf("Mode: %x\n", mode);

    sprintf(tekst, "/%s", (const char *)globalData);
    //Serial.println(tekst);
    //Serial.printf("mode: %x\r\n", mode);
    //Serial.println(__LINE__);
    //res = SPIFFS.open("/test.txt", FILE_WRITE);
    //Serial.println(__LINE__);
    exists = file_exists(tekst);
    //Serial.println(__LINE__);
    if (FR_OK == exists)
    {
        if (mode & FA_CREATE_NEW)
        {
            //Serial.println(__LINE__);
            kbd.enaIRQ();
            return FR_EXIST;
        }

        if (mode & FA_CREATE_ALWAYS)
        {
            //Serial.println(__LINE__);
            open_mode = O_CREAT;
        }
        if (mode & (FA_READ | FA_WRITE))
        {
            //Serial.println(__LINE__);
            if (mode & FA_WRITE)
                open_mode |= O_RDWR;
            else
                open_mode |= O_RDONLY;
            //Serial.println(__LINE__);
        }
        //Serial.println(__LINE__);
    }
    else
    {
        //Serial.println(__LINE__);
        if (mode & (FA_OPEN_ALWAYS | FA_CREATE_NEW | FA_CREATE_ALWAYS))
        {
            //Serial.println("Poging om file aan te maken");
            //Serial.println(mode, HEX);
            open_mode = O_CREAT | O_RDWR;
        }
        else
        {
            //Serial.println("File not found");
            kbd.enaIRQ();
            return FR_NO_FILE;
        }
    }
    //Serial.printf("%s: %d\n", __func__, __LINE__);
    //Serial.printf("File openmode: %x\n", open_mode);

    if (open_mode == 0)
    {
        //Serial.printf("Open ffile: %s, met modus: %s\n", tekst, "r");
        *fp = SPIFFS.open(tekst, "r");
    }
    if (open_mode == 2)
    {
        //Serial.printf("Open ffile: %s, met modus: %s\n", tekst, "w");
        *fp = SPIFFS.open(tekst, "w");
    }
    /*
    exists = file_exists(path);

    //debuglog("f_open(%s,%02X):exists=%d\n",open_path,mode,exists);

    mode &= (FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW);

    if (FR_OK == exists)
    {
        if (mode & FA_CREATE_NEW)
            return FR_EXIST;

        if (mode & FA_CREATE_ALWAYS)
            open_mode = O_CREAT;

        if (mode & (FA_READ | FA_WRITE))
        {
            if (mode & FA_WRITE)
                open_mode |= O_RDWR;
            else
                open_mode |= O_RDONLY;
        }
    }
    else
    {
        if (mode & (FA_OPEN_ALWAYS | FA_CREATE_NEW | FA_CREATE_ALWAYS))
            open_mode = O_CREAT | O_RDWR;
        else
            return FR_NO_FILE;
    }

    newfile = open(open_path, open_mode | O_BINARY, S_IRWXU);
    update_FIL(fp, newfile, 1);

    //debuglog("Openmode:%04X\n",open_mode);

    if (newfile > 0)
    {
        openfil = fp;
        status = FR_OK;
    }
    else
        status = FR_INVALID_NAME;
        */
    //kbd.enaIRQ();
    return status;
}

FRESULT f_read(
    File *fp,   /* Pointer to the file object */
    char *buff, /* Pointer to data buffer */
    size_t btr, /* Number of bytes to read */
    size_t *br  /* Pointer to number of bytes read */
)
{
    //Serial.println(__func__);
    kbd.disIRQ();
    FRESULT status = (FRESULT)0;
    DWORD ptrpos;
    int bytesread;
    int error;

    ptrpos = fp->position();

    bytesread = fp->readBytes(buff, btr);
    *br = bytesread;

    //Serial.printf("f_read(%d) offset=%d[%04X],result=%d\n", btr, ptrpos, ptrpos, *br);
    HexDumpHead(buff, btr);

    update_FIL(fp, 0, 0);

    if (bytesread < 0)
    {
        error = errno;
        status = (FRESULT)error;
    }
    status = FR_OK;
    kbd.enaIRQ();
    return status;
}

FRESULT f_write(
    File *fp,      /* Pointer to the file object */
    uint8_t *buff, /* Pointer to the data to be written */
    size_t btw,    /* Number of bytes to write */
    size_t *bw     /* Pointer to number of bytes written */
)
{
    DWORD ptrpos;
    int written;
    int error;
    int len = btw;
    FRESULT status;
    //Serial.println(__func__);
    kbd.disIRQ();

    ptrpos = fp->position();

    // SP9 START

    written = fp->write(buff, btw);
    *bw = written;

    //debuglog("f_write(%d) offset=%d[%04X],result=%d\n",btw,ptrpos,ptrpos,written);
    //	HexDumpHead(buff,btw);

    if (written < 0)
    {
        error = errno;
        //debuglog("errno: %s [%d]\n",strerror(error),error);
        status = (FRESULT)error; /* Return correct error for RAF */
    }

    update_FIL(fp, 0, 0);
    status = FR_OK;
    kbd.enaIRQ();
    return status;
}

// SP9 END

FRESULT f_close(
    File *fp /* Pointer to the file object to be closed */
)
{
    //Serial.println(__func__);
    kbd.disIRQ();
    // int result = 0;

    if (0 != (int)fp)
        fp->close();

    //debuglog("f_close():result=%d\n",result);

    fp = NULL;
    openfil = NULL;
    kbd.enaIRQ();
    return FR_OK;
}

FRESULT f_unlink(char *path) /* Pointer to the file or directory path */
{
    char del_path[PATHSIZE + 1];
    int open_mode = 0;

    /* CHANGED FOR SP4 */

    int result = 0;
    FRESULT status;

    int newfile;

    // Get real path of file and check to see if it exists
    snprintf(del_path, PATHSIZE, "%s/%s", MMCPath, path);

    //debuglog("f_unlink(%s)\n",del_path);
    //Serial.println(__func__);
    kbd.disIRQ();

    result = unlink(del_path);

    if (result == 0)
        status = FR_OK;
    else

        status = get_result(errno);

    /* END SP4*/
    kbd.enaIRQ();
    return status;
}

FRESULT f_opendir(
    DIR *dj,   /* Pointer to directory object to create */
    char *path /* Pointer to the directory path */
)
{
    FRESULT status;
    //Serial.println(__func__);
    kbd.disIRQ();

    root = SPIFFS.open("/");
    //Serial.println("fs opened");
    File file = root.openNextFile();
    //Serial.println("fs openednextfile");

    if (file)
    {
        //Serial.print("FILE: ");
        //Serial.println(file.name());
        status = FR_OK;
    }
    else
    {
        status = FR_NO_PATH;
    }
    kbd.enaIRQ();
    return status;
}

FRESULT f_readdir(
    DIR *dj,     /* Pointer to the open directory object */
    FILINFO *fno /* Pointer to file information to return */
)
{
    File file;
    FRESULT status;
    //Serial.println(__func__);
    //kbd.disIRQ();
    //Serial.print("Finename: ");
    //Serial.println(fno->fname);
    /*  // If a file found copy it's details, else set size to 0 and filename to ''
    if (findnext(&emu))
    {
        fno->fsize = emu.fsize;
        fno->fattrib = emu.fattrib;
        strncpy(fno->fname, emu.filename, FNAMELEN);
    }
    else
    {
        fno->fsize = 0;
        fno->fname[0] = 0;
    }

    return FR_OK;
    */

    // If a file found copy it's details, else set size to 0 and filename to ''
    /*
    file = root.openNextFile();
    if (file)
    {
        fno->fsize = file.size();
        // emu.fsize;
        // Bestaat deze in fs?? fno->fattrib = file-> emu.fattrib;
        strncpy(fno->fname, file.name(), FNAMELEN);
    }
    else
    {
        fno->fsize = 0;
        fno->fname[0] = 0;
    }
    kbd.enaIRQ();
    */
    return FR_OK;
}

FRESULT f_lseek(
    File *fp, /* Pointer to the file object */
    DWORD ofs /* File pointer from top of file */
)
{
    //Serial.println(__func__);
    kbd.disIRQ();

    fp->seek(ofs);
    update_FIL(fp, 0, 0);
    kbd.enaIRQ();
    return FR_OK;
}

static void get_fileinfo(             /* No return code */
                         DIR *dj,     /* Pointer to the directory object */
                         FILINFO *fno /* Pointer to the file information to be filled */
)
{
}

void get_fileinfo_special(FILINFO *fno)
{
    //   get_fileinfo(&dj, fno);

    //Serial.printf("get_fileinfo_special()\n");
    /*
    if (NULL != openfil)
    {

        fno->fsize = openfil->fsize;
        //		fno->fptr	= openfil->fptr;
        fno->fdate = 0;
        fno->ftime = 0;
        fno->fattrib = get_fat_attribs((int)openfil->fs);
         //Serial.printf("size=%d, attr=%d\n", fno->fsize, fno->fattrib);
    }
    */
}

// #ifdef DEBUGFF
void HexDump(char *Buff,
             int Length)
{
    char LineBuff[80];
    char *LineBuffPos;
    int LineOffset;
    int CharOffset;
    char *BuffPtr;

    BuffPtr = Buff;

    for (LineOffset = 0; LineOffset < Length; LineOffset += 16, BuffPtr += 16)
    {
        LineBuffPos = LineBuff;
        LineBuffPos += sprintf(LineBuffPos, "%4.4X ", LineOffset);

        for (CharOffset = 0; CharOffset < 16; CharOffset++)
        {
            if ((LineOffset + CharOffset) < Length)
                LineBuffPos += sprintf(LineBuffPos, "%02X ", BuffPtr[CharOffset]);
            else
                LineBuffPos += sprintf(LineBuffPos, "   ");
        }

        for (CharOffset = 0; CharOffset < 16; CharOffset++)
        {
            if ((LineOffset + CharOffset) < Length)
            {
                if (isprint(BuffPtr[CharOffset]))
                    LineBuffPos += sprintf(LineBuffPos, "%c", BuffPtr[CharOffset]);
                else
                    LineBuffPos += sprintf(LineBuffPos, " ");
            }
            else
                LineBuffPos += sprintf(LineBuffPos, ".");
        }
        Serial.printf("%s\n", LineBuff);
    }
    Serial.printf("\n\n");
}
//#endif

void HexDumpHead(char *Buff,
                 int Length)
{
    Serial.printf("Addr 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F ASCII\n");
    Serial.printf("----------------------------------------------------------\n");

    HexDump(Buff, Length);
};
