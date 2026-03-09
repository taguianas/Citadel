#ifndef __IO_UTILS_H__
#define __IO_UTILS_H__

/***********************************************
*
* @File: io_utils.h
* @Purpose: Declarations for I/O utility functions
*           that replace printf/scanf with read/write.
* @Author: Anas TAGUI |  Abderrahmen MESKINE
* @Date: 2026-03-06
*
***********************************************/

// Project Includes
#include "globals.h"

// Prototypes
void printMessage(const char *psMessage);
void printInt(int nValue);
void printFloat(float fValue);
char *readLine(int nFd);
int stringToInt(const char *psStr);
void toUpperCase(char *psStr);
int compareIgnoreCase(const char *psA, const char *psB);
char *trimWhitespace(char *psStr);
int countWords(const char *psStr);
char *getWord(const char *psStr, int nIndex, char *psBuffer, int nBufferSize);
void stripAmpersand(char *psStr);

#endif
