#ifndef __COMMANDS_H__
#define __COMMANDS_H__

/***********************************************
*
* @File: commands.h
* @Purpose: Declarations for command parsing and
*           execution functions for the Maester terminal.
* @Author: Anas TAGUI |  Abderrahmen MESKINE
* @Date: 2026-03-06
*
***********************************************/

// Project Includes
#include "globals.h"
#include "io_utils.h"
#include "config.h"
#include "inventory.h"

// Prototypes
int processCommand(const char *psInput, MaesterConfig *pstConfig,
                   Inventory *pstInventory, int *pnRunning);

#endif
