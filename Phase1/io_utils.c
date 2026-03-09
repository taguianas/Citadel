/***********************************************
*
* @File: io_utils.c
* @Purpose: Implementation of I/O utility functions
*           using only read() and write() system calls.
* @Author: Anas TAGUI |  Abderrahmen MESKINE
* @Date: 2026-03-06
*
***********************************************/

#include "io_utils.h"

/********************************************************************
*
* @Name: printMessage
* @Def: Writes a string message to standard output using write().
* @Arg: In: psMessage = null-terminated string to print
* @Ret: None.
*
********************************************************************/
void printMessage(const char *psMessage) {

    int nLen = 0;

    if (NULL == psMessage) {
        return;
    }

    nLen = strlen(psMessage);
    write(STDOUT_FILENO, psMessage, nLen);
}

/********************************************************************
*
* @Name: printInt
* @Def: Converts an integer to string and writes it to stdout.
* @Arg: In: nValue = integer value to print
* @Ret: None.
*
********************************************************************/
void printInt(int nValue) {

    char sBuffer[32];
    int nIndex = 0;
    int nIsNegative = 0;
    int nTemp = 0;
    int nStart = 0;
    int nEnd = 0;
    char cSwap = 0;

    if (0 == nValue) {
        write(STDOUT_FILENO, "0", 1);
        return;
    }

    if (nValue < 0) {
        nIsNegative = 1;
        nValue = -nValue;
    }

    // Build the number string in reverse
    while (nValue > 0) {
        nTemp = nValue % 10;
        sBuffer[nIndex] = '0' + nTemp;
        nIndex++;
        nValue = nValue / 10;
    }

    if (nIsNegative) {
        sBuffer[nIndex] = '-';
        nIndex++;
    }

    // Reverse the string
    nStart = 0;
    nEnd = nIndex - 1;
    while (nStart < nEnd) {
        cSwap = sBuffer[nStart];
        sBuffer[nStart] = sBuffer[nEnd];
        sBuffer[nEnd] = cSwap;
        nStart++;
        nEnd--;
    }

    sBuffer[nIndex] = '\0';
    write(STDOUT_FILENO, sBuffer, nIndex);
}

/********************************************************************
*
* @Name: printFloat
* @Def: Converts a float to string with one decimal and writes to stdout.
* @Arg: In: fValue = float value to print
* @Ret: None.
*
********************************************************************/
void printFloat(float fValue) {

    int nIntPart = 0;
    int nDecPart = 0;
    float fTemp = 0.0;

    if (fValue < 0) {
        write(STDOUT_FILENO, "-", 1);
        fValue = -fValue;
    }

    nIntPart = (int)fValue;
    fTemp = (fValue - nIntPart) * 10.0f + 0.5f;
    nDecPart = (int)fTemp;

    if (nDecPart >= 10) {
        nIntPart++;
        nDecPart = 0;
    }

    printInt(nIntPart);
    write(STDOUT_FILENO, ".", 1);
    printInt(nDecPart);
}

/********************************************************************
*
* @Name: readLine
* @Def: Reads a line from a file descriptor character by character
*       until newline or EOF. Returns dynamically allocated string.
* @Arg: In: nFd = file descriptor to read from
* @Ret: Dynamically allocated string with the line content,
*       or NULL if EOF or error.
*
********************************************************************/
char *readLine(int nFd) {

    char cChar = 0;
    int nBytesRead = 0;
    int nCapacity = 128;
    int nLength = 0;
    char *psLine = NULL;
    char *psTemp = NULL;

    psLine = (char *)malloc(nCapacity);
    if (NULL == psLine) {
        return NULL;
    }

    while (1) {
        nBytesRead = read(nFd, &cChar, 1);

        if (nBytesRead <= 0) {
            // EOF or error
            if (0 == nLength) {
                free(psLine);
                return NULL;
            }
            break;
        }

        if ('\n' == cChar) {
            break;
        }

        // Grow buffer if needed
        if (nLength >= nCapacity - 1) {
            nCapacity = nCapacity * 2;
            psTemp = (char *)realloc(psLine, nCapacity);
            if (NULL == psTemp) {
                free(psLine);
                return NULL;
            }
            psLine = psTemp;
        }

        psLine[nLength] = cChar;
        nLength++;
    }

    psLine[nLength] = '\0';
    return psLine;
}

/********************************************************************
*
* @Name: stringToInt
* @Def: Converts a string to an integer.
* @Arg: In: psStr = null-terminated string containing a number
* @Ret: The integer value represented by the string.
*
********************************************************************/
int stringToInt(const char *psStr) {

    int nResult = 0;
    int nSign = 1;
    int i = 0;

    if (NULL == psStr) {
        return 0;
    }

    // Skip whitespace
    while (' ' == psStr[i] || '\t' == psStr[i]) {
        i++;
    }

    if ('-' == psStr[i]) {
        nSign = -1;
        i++;
    } else if ('+' == psStr[i]) {
        i++;
    }

    while (psStr[i] >= '0' && psStr[i] <= '9') {
        nResult = nResult * 10 + (psStr[i] - '0');
        i++;
    }

    return nResult * nSign;
}

/********************************************************************
*
* @Name: compareIgnoreCase
* @Def: Compares two strings ignoring case differences.
* @Arg: In: psA = first string
*       In: psB = second string
* @Ret: 0 if equal ignoring case, non-zero otherwise.
*
********************************************************************/
int compareIgnoreCase(const char *psA, const char *psB) {

    int i = 0;

    if (NULL == psA || NULL == psB) {
        return -1;
    }

    while (psA[i] != '\0' && psB[i] != '\0') {
        if (tolower((unsigned char)psA[i]) != tolower((unsigned char)psB[i])) {
            return tolower((unsigned char)psA[i]) - tolower((unsigned char)psB[i]);
        }
        i++;
    }

    return tolower((unsigned char)psA[i]) - tolower((unsigned char)psB[i]);
}

/********************************************************************
*
* @Name: trimWhitespace
* @Def: Removes leading and trailing whitespace from a string in place.
* @Arg: In/Out: psStr = string to trim
* @Ret: Pointer to the trimmed string (same pointer, shifted).
*
********************************************************************/
char *trimWhitespace(char *psStr) {

    char *psEnd = NULL;

    if (NULL == psStr) {
        return NULL;
    }

    // Trim leading whitespace
    while (' ' == *psStr || '\t' == *psStr || '\r' == *psStr) {
        psStr++;
    }

    if ('\0' == *psStr) {
        return psStr;
    }

    // Trim trailing whitespace
    psEnd = psStr + strlen(psStr) - 1;
    while (psEnd > psStr && (' ' == *psEnd || '\t' == *psEnd || '\r' == *psEnd)) {
        psEnd--;
    }

    *(psEnd + 1) = '\0';

    return psStr;
}

/********************************************************************
*
* @Name: countWords
* @Def: Counts the number of whitespace-separated words in a string.
* @Arg: In: psStr = null-terminated string
* @Ret: Number of words in the string.
*
********************************************************************/
int countWords(const char *psStr) {

    int nCount = 0;
    int nInWord = 0;
    int i = 0;

    if (NULL == psStr) {
        return 0;
    }

    for (i = 0; psStr[i] != '\0'; i++) {
        if (' ' == psStr[i] || '\t' == psStr[i]) {
            nInWord = 0;
        } else {
            if (0 == nInWord) {
                nCount++;
                nInWord = 1;
            }
        }
    }

    return nCount;
}

/********************************************************************
*
* @Name: getWord
* @Def: Extracts the nth word (0-indexed) from a string.
* @Arg: In: psStr = source string
*       In: nIndex = word index (0-based)
*       Out: psBuffer = buffer to store the extracted word
*       In: nBufferSize = size of the output buffer
* @Ret: Pointer to psBuffer if successful, NULL otherwise.
*
********************************************************************/
char *getWord(const char *psStr, int nIndex, char *psBuffer, int nBufferSize) {

    int nCurrentWord = 0;
    int nInWord = 0;
    int nWordStart = 0;
    int nWordLen = 0;
    int i = 0;

    if (NULL == psStr || NULL == psBuffer || nBufferSize <= 0) {
        return NULL;
    }

    for (i = 0; psStr[i] != '\0'; i++) {
        if (' ' == psStr[i] || '\t' == psStr[i]) {
            if (nInWord) {
                if (nCurrentWord == nIndex) {
                    // Found our word
                    nWordLen = i - nWordStart;
                    if (nWordLen >= nBufferSize) {
                        nWordLen = nBufferSize - 1;
                    }
                    memcpy(psBuffer, psStr + nWordStart, nWordLen);
                    psBuffer[nWordLen] = '\0';
                    return psBuffer;
                }
                nCurrentWord++;
                nInWord = 0;
            }
        } else {
            if (0 == nInWord) {
                nWordStart = i;
                nInWord = 1;
            }
        }
    }

    // Handle last word
    if (nInWord && nCurrentWord == nIndex) {
        nWordLen = i - nWordStart;
        if (nWordLen >= nBufferSize) {
            nWordLen = nBufferSize - 1;
        }
        memcpy(psBuffer, psStr + nWordStart, nWordLen);
        psBuffer[nWordLen] = '\0';
        return psBuffer;
    }

    return NULL;
}

/********************************************************************
*
* @Name: stripAmpersand
* @Def: Removes all '&' characters from a string in place.
* @Arg: In/Out: psStr = string to modify
* @Ret: None.
*
********************************************************************/
void stripAmpersand(char *psStr) {

    int nRead = 0;
    int nWrite = 0;

    if (NULL == psStr) {
        return;
    }

    for (nRead = 0; psStr[nRead] != '\0'; nRead++) {
        if ('&' != psStr[nRead]) {
            psStr[nWrite] = psStr[nRead];
            nWrite++;
        }
    }
    psStr[nWrite] = '\0';
}
