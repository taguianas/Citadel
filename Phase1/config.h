#ifndef __CONFIG_H__
#define __CONFIG_H__

/***********************************************
*
* @File: config.h
* @Purpose: Declarations for configuration file
*           parsing functions (maester.dat).
* @Author: Anas TAGUI |  Abderrahmen MESKINE
* @Date: 2026-03-06
*
***********************************************/

// Project Includes
#include "globals.h"
#include "io_utils.h"

// Prototypes
int parseConfig(const char *psFilePath, MaesterConfig *pstConfig);
void freeConfig(MaesterConfig *pstConfig);

#endif
