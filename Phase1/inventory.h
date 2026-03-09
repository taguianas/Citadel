#ifndef __INVENTORY_H__
#define __INVENTORY_H__

/***********************************************
*
* @File: inventory.h
* @Purpose: Declarations for inventory management
*           functions (reading/writing stock.db).
* @Author: Anas TAGUI |  Abderrahmen MESKINE
* @Date: 2026-03-06
*
***********************************************/

// Project Includes
#include "globals.h"
#include "io_utils.h"

// Prototypes
int loadInventory(const char *psFilePath, Inventory *pstInventory);
void displayInventory(const Inventory *pstInventory);
void freeInventory(Inventory *pstInventory);

#endif
