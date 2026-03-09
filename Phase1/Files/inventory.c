/***********************************************
*
* @File: inventory.c
* @Purpose: Implementation of inventory management
*           functions for reading binary stock.db files
*           and displaying product listings.
* @Author: Anas TAGUI |  Abderrahmen MESKINE
* @Date: 2026-03-06
*
***********************************************/

#include "inventory.h"

/********************************************************************
*
* @Name: loadInventory
* @Def: Reads the binary stock.db file and loads all products
*       into the Inventory structure. Each record consists of
*       name[100] (char), amount (int), weight (float).
* @Arg: In: psFilePath = path to the stock.db binary file
*       Out: pstInventory = pointer to Inventory struct to populate
* @Ret: Returns 0 on success, -1 on error.
*
********************************************************************/
int loadInventory(const char *psFilePath, Inventory *pstInventory) {

    int nFd = 0;
    int nBytesRead = 0;
    int nCapacity = 16;
    int nCount = 0;
    Product stTemp;
    Product *pNewArray = NULL;

    if (NULL == psFilePath || NULL == pstInventory) {
        return -1;
    }

    pstInventory->pProducts = NULL;
    pstInventory->nNumProducts = 0;

    nFd = open(psFilePath, O_RDONLY);
    if (nFd < 0) {
        printMessage("ERROR: Could not open inventory file.\n");
        return -1;
    }

    // Allocate initial array
    pstInventory->pProducts = (Product *)malloc(sizeof(Product) * nCapacity);
    if (NULL == pstInventory->pProducts) {
        close(nFd);
        return -1;
    }

    // Read records: each is name[100] + int + float
    nCount = 0;
    while (1) {
        memset(&stTemp, 0, sizeof(Product));

        // Read name (100 bytes)
        nBytesRead = read(nFd, stTemp.sName, MAX_PRODUCT_NAME);
        if (nBytesRead <= 0) {
            break;
        }
        if (nBytesRead < MAX_PRODUCT_NAME) {
            break;
        }

        // Read amount (int)
        nBytesRead = read(nFd, &stTemp.nAmount, sizeof(int));
        if (nBytesRead < (int)sizeof(int)) {
            break;
        }

        // Read weight (float)
        nBytesRead = read(nFd, &stTemp.fWeight, sizeof(float));
        if (nBytesRead < (int)sizeof(float)) {
            break;
        }

        // Grow array if needed
        if (nCount >= nCapacity) {
            nCapacity = nCapacity * 2;
            pNewArray = (Product *)realloc(pstInventory->pProducts, sizeof(Product) * nCapacity);
            if (NULL == pNewArray) {
                close(nFd);
                return -1;
            }
            pstInventory->pProducts = pNewArray;
        }

        memcpy(&pstInventory->pProducts[nCount], &stTemp, sizeof(Product));
        nCount++;
    }

    pstInventory->nNumProducts = nCount;
    close(nFd);

    return 0;
}

/********************************************************************
*
* @Name: displayInventory
* @Def: Displays the current inventory in a formatted table showing
*       item name, value (amount as gold), and weight (as stone).
* @Arg: In: pstInventory = pointer to Inventory struct to display
* @Ret: None.
*
********************************************************************/
void displayInventory(const Inventory *pstInventory) {

    int i = 0;

    if (NULL == pstInventory || NULL == pstInventory->pProducts) {
        printMessage("No inventory loaded.\n");
        return;
    }

    printMessage("--- Trade Ledger ---\n");
    printMessage("Item                           | Value (Gold) | Weight (Stone)\n");
    printMessage("-------------------------------------------------------\n");

    for (i = 0; i < pstInventory->nNumProducts; i++) {
        // Print product name (pad to ~31 chars)
        printMessage(pstInventory->pProducts[i].sName);

        // Calculate padding
        int nNameLen = strlen(pstInventory->pProducts[i].sName);
        int nPadding = 31 - nNameLen;
        int j = 0;
        if (nPadding < 1) {
            nPadding = 1;
        }
        for (j = 0; j < nPadding; j++) {
            printMessage(" ");
        }

        printMessage("| ");
        printInt(pstInventory->pProducts[i].nAmount);

        // Calculate padding for amount column
        int nAmountLen = 0;
        int nTempVal = pstInventory->pProducts[i].nAmount;
        if (0 == nTempVal) {
            nAmountLen = 1;
        } else {
            while (nTempVal > 0) {
                nAmountLen++;
                nTempVal = nTempVal / 10;
            }
        }
        int nValPad = 13 - nAmountLen;
        for (j = 0; j < nValPad; j++) {
            printMessage(" ");
        }

        printMessage("| ");
        printFloat(pstInventory->pProducts[i].fWeight);
        printMessage("\n");
    }

    printMessage("-------------------------------------------------------\n");
    printMessage("Total Entries: ");
    printInt(pstInventory->nNumProducts);
    printMessage("\n");
}

/********************************************************************
*
* @Name: freeInventory
* @Def: Frees dynamically allocated memory in an Inventory struct.
* @Arg: In/Out: pstInventory = pointer to Inventory struct to free
* @Ret: None.
*
********************************************************************/
void freeInventory(Inventory *pstInventory) {

    if (NULL == pstInventory) {
        return;
    }

    if (NULL != pstInventory->pProducts) {
        free(pstInventory->pProducts);
        pstInventory->pProducts = NULL;
    }
    pstInventory->nNumProducts = 0;
}
