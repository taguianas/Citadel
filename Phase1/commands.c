/***********************************************
*
* @File: commands.c
* @Purpose: Implementation of command parsing and
*           execution for the Maester terminal.
*           Handles LIST REALMS, LIST PRODUCTS,
*           START TRADE, EXIT, and stub commands.
* @Author: Anas TAGUI |  Abderrahmen MESKINE
* @Date: 2026-03-06
*
***********************************************/

#include "commands.h"

// Forward declarations for internal helper functions
static void cmdListRealms(MaesterConfig *pstConfig);
static void cmdListProducts(const char *psInput, MaesterConfig *pstConfig,
                            Inventory *pstInventory);
static void cmdStartTrade(const char *psInput, MaesterConfig *pstConfig,
                          Inventory *pstInventory);
static void cmdExit(MaesterConfig *pstConfig, Inventory *pstInventory,
                    int *pnRunning);
static void runTradeMenu(const char *psTargetRealm, MaesterConfig *pstConfig);
static int writeTradeFile(const TradeList *pstTradeList, const char *psFolderPath);

/********************************************************************
*
* @Name: processCommand
* @Def: Parses user input and dispatches to the appropriate command
*       handler. Commands are case-insensitive. Returns 0 on success,
*       -1 if the command is unknown or malformed.
* @Arg: In: psInput = raw user input string
*       In: pstConfig = pointer to loaded MaesterConfig
*       In: pstInventory = pointer to loaded Inventory
*       In/Out: pnRunning = flag to signal program exit
* @Ret: Returns 0 on valid command, -1 on unknown command.
*
********************************************************************/
int processCommand(const char *psInput, MaesterConfig *pstConfig,
                   Inventory *pstInventory, int *pnRunning) {

    char sWord1[MAX_BUFFER];
    char sWord2[MAX_BUFFER];
    char sWord3[MAX_BUFFER];
    int nWordCount = 0;
    char *psCopy = NULL;
    char *psTrimmed = NULL;

    if (NULL == psInput || NULL == pstConfig || NULL == pstInventory || NULL == pnRunning) {
        return -1;
    }

    // Make a working copy
    psCopy = (char *)malloc(strlen(psInput) + 1);
    if (NULL == psCopy) {
        return -1;
    }
    strcpy(psCopy, psInput);
    psTrimmed = trimWhitespace(psCopy);

    // Handle empty input
    if ('\0' == *psTrimmed) {
        free(psCopy);
        return 0;
    }

    nWordCount = countWords(psTrimmed);

    memset(sWord1, 0, sizeof(sWord1));
    memset(sWord2, 0, sizeof(sWord2));
    memset(sWord3, 0, sizeof(sWord3));

    if (nWordCount >= 1) {
        getWord(psTrimmed, 0, sWord1, sizeof(sWord1));
    }
    if (nWordCount >= 2) {
        getWord(psTrimmed, 1, sWord2, sizeof(sWord2));
    }
    if (nWordCount >= 3) {
        getWord(psTrimmed, 2, sWord3, sizeof(sWord3));
    }

    // EXIT command (exactly 1 word)
    if (0 == compareIgnoreCase(sWord1, "EXIT") && 1 == nWordCount) {
        cmdExit(pstConfig, pstInventory, pnRunning);
        free(psCopy);
        return 0;
    }

    // LIST REALMS (exactly 2 words)
    if (0 == compareIgnoreCase(sWord1, "LIST") &&
        0 == compareIgnoreCase(sWord2, "REALMS") &&
        2 == nWordCount) {
        cmdListRealms(pstConfig);
        free(psCopy);
        return 0;
    }

    // LIST PRODUCTS [realm] (2 or 3 words)
    if (0 == compareIgnoreCase(sWord1, "LIST") &&
        0 == compareIgnoreCase(sWord2, "PRODUCTS")) {
        if (2 == nWordCount) {
            // No realm specified: display own inventory
            cmdListProducts(NULL, pstConfig, pstInventory);
            free(psCopy);
            return 0;
        }
        if (3 == nWordCount) {
            // With realm: Phase 1 just prints "Command OK"
            printMessage("Command OK\n");
            free(psCopy);
            return 0;
        }
        // More than 3 words: unknown command
        printMessage("Unknown command\n");
        free(psCopy);
        return -1;
    }

    // PLEDGE <realm> <sigil> (exactly 3 words)
    if (0 == compareIgnoreCase(sWord1, "PLEDGE") &&
        0 != compareIgnoreCase(sWord2, "RESPOND") &&
        0 != compareIgnoreCase(sWord2, "STATUS")) {
        if (3 == nWordCount) {
            printMessage("Command OK\n");
            free(psCopy);
            return 0;
        }
        if (1 == nWordCount) {
            // PLEDGE alone: missing arguments
            printMessage("Did you mean to send a pledge? Please review syntax.\n");
            free(psCopy);
            return 0;
        }
        if (2 == nWordCount) {
            // PLEDGE <realm> without sigil
            printMessage("Missing sigil argument. Please review syntax.\n");
            free(psCopy);
            return 0;
        }
        // More than 3 words
        printMessage("Unknown command\n");
        free(psCopy);
        return -1;
    }

    // PLEDGE RESPOND <realm> ACCEPT/REJECT (exactly 4 words)
    if (0 == compareIgnoreCase(sWord1, "PLEDGE") &&
        0 == compareIgnoreCase(sWord2, "RESPOND")) {
        if (4 == nWordCount) {
            char sWord4[MAX_BUFFER];
            memset(sWord4, 0, sizeof(sWord4));
            getWord(psTrimmed, 3, sWord4, sizeof(sWord4));
            if (0 == compareIgnoreCase(sWord4, "ACCEPT") ||
                0 == compareIgnoreCase(sWord4, "REJECT")) {
                printMessage("Command OK\n");
                free(psCopy);
                return 0;
            }
        }
        if (nWordCount < 4) {
            printMessage("Missing arguments for PLEDGE RESPOND. Please review syntax.\n");
            free(psCopy);
            return 0;
        }
        printMessage("Unknown command\n");
        free(psCopy);
        return -1;
    }

    // PLEDGE STATUS (exactly 2 words)
    if (0 == compareIgnoreCase(sWord1, "PLEDGE") &&
        0 == compareIgnoreCase(sWord2, "STATUS") &&
        2 == nWordCount) {
        printMessage("Command OK\n");
        free(psCopy);
        return 0;
    }

    // START TRADE <realm> (exactly 3 words)
    if (0 == compareIgnoreCase(sWord1, "START") &&
        0 == compareIgnoreCase(sWord2, "TRADE")) {
        if (3 == nWordCount) {
            cmdStartTrade(psTrimmed, pstConfig, pstInventory);
            free(psCopy);
            return 0;
        }
        if (2 == nWordCount) {
            printMessage("Missing arguments, can't start a trade. Please review the syntax.\n");
            free(psCopy);
            return 0;
        }
        // More than 3 words: unknown
        printMessage("Unknown command\n");
        free(psCopy);
        return -1;
    }

    // ENVOY STATUS (exactly 2 words)
    if (0 == compareIgnoreCase(sWord1, "ENVOY") &&
        0 == compareIgnoreCase(sWord2, "STATUS") &&
        2 == nWordCount) {
        printMessage("Command OK\n");
        free(psCopy);
        return 0;
    }

    // If we get here, it's an unknown command
    printMessage("Unknown command\n");
    free(psCopy);
    return -1;
}

/********************************************************************
*
* @Name: cmdListRealms
* @Def: Lists all realms found in the routing table of the config.
* @Arg: In: pstConfig = pointer to the MaesterConfig
* @Ret: None.
*
********************************************************************/
static void cmdListRealms(MaesterConfig *pstConfig) {

    int i = 0;

    for (i = 0; i < pstConfig->nNumRoutes; i++) {
        if (0 != compareIgnoreCase(pstConfig->aRoutes[i].sRealmName, "DEFAULT")) {
            printMessage("- ");
            printMessage(pstConfig->aRoutes[i].sRealmName);
            printMessage("\n");
        }
    }
}

/********************************************************************
*
* @Name: cmdListProducts
* @Def: Displays the local inventory when no realm is specified.
* @Arg: In: psRealm = realm name (NULL for local inventory)
*       In: pstConfig = pointer to the MaesterConfig
*       In: pstInventory = pointer to the loaded Inventory
* @Ret: None.
*
********************************************************************/
static void cmdListProducts(const char *psRealm, MaesterConfig *pstConfig,
                            Inventory *pstInventory) {

    (void)pstConfig;

    if (NULL != psRealm) {
        // In Phase 1, remote product listing is just "Command OK"
        printMessage("Command OK\n");
        return;
    }

    displayInventory(pstInventory);
}

/********************************************************************
*
* @Name: cmdStartTrade
* @Def: Initiates a trade session with a target realm, entering the
*       trade sub-menu where the user can add/remove items and send.
* @Arg: In: psInput = the original command string
*       In: pstConfig = pointer to the MaesterConfig
*       In: pstInventory = pointer to the loaded Inventory
* @Ret: None.
*
********************************************************************/
static void cmdStartTrade(const char *psInput, MaesterConfig *pstConfig,
                          Inventory *pstInventory) {

    char sRealm[MAX_REALM_NAME];

    (void)pstInventory;
    memset(sRealm, 0, sizeof(sRealm));
    getWord(psInput, 2, sRealm, sizeof(sRealm));

    runTradeMenu(sRealm, pstConfig);
}

/********************************************************************
*
* @Name: runTradeMenu
* @Def: Runs the interactive trade sub-menu with (trade)> prompt.
*       Supports add, remove, send, and cancel commands.
* @Arg: In: psTargetRealm = name of the realm to trade with
*       In: pstConfig = pointer to the MaesterConfig
* @Ret: None.
*
********************************************************************/
static void runTradeMenu(const char *psTargetRealm, MaesterConfig *pstConfig) {

    TradeList stTradeList;
    char *psLine = NULL;
    char *psTrimmed = NULL;
    char sWord1[MAX_BUFFER];
    char sProductName[MAX_PRODUCT_NAME];
    char sAmountStr[32];
    int nWordCount = 0;
    int nAmount = 0;
    int nTradeRunning = 1;
    int i = 0;
    int nFound = 0;

    memset(&stTradeList, 0, sizeof(TradeList));
    strncpy(stTradeList.sTargetRealm, psTargetRealm, MAX_REALM_NAME - 1);

    while (nTradeRunning) {
        printMessage(TRADE_PROMPT);
        psLine = readLine(STDIN_FILENO);

        if (NULL == psLine) {
            // EOF
            nTradeRunning = 0;
            break;
        }

        psTrimmed = trimWhitespace(psLine);

        if ('\0' == *psTrimmed) {
            free(psLine);
            continue;
        }

        nWordCount = countWords(psTrimmed);
        memset(sWord1, 0, sizeof(sWord1));
        getWord(psTrimmed, 0, sWord1, sizeof(sWord1));

        // SEND command
        if (0 == compareIgnoreCase(sWord1, "send") && 1 == nWordCount) {
            if (stTradeList.nNumItems > 0) {
                // Write trade file to the maester's working directory
                writeTradeFile(&stTradeList, pstConfig->sFolderPath);
                printMessage("Trade list sent to ");
                printMessage(psTargetRealm);
                printMessage(".\n");
            } else {
                printMessage("No items in trade list. Nothing to send.\n");
            }
            nTradeRunning = 0;
            free(psLine);
            break;
        }

        // CANCEL command
        if (0 == compareIgnoreCase(sWord1, "cancel") && 1 == nWordCount) {
            printMessage("Trade cancelled.\n");
            nTradeRunning = 0;
            free(psLine);
            break;
        }

        // ADD command: add <product_name> <amount>
        // Product name can be multi-word, so amount is the last word
        if (0 == compareIgnoreCase(sWord1, "add") && nWordCount >= 3) {
            // The last word is the amount, everything between word1 and last is product name
            memset(sAmountStr, 0, sizeof(sAmountStr));
            getWord(psTrimmed, nWordCount - 1, sAmountStr, sizeof(sAmountStr));
            nAmount = stringToInt(sAmountStr);

            // Build product name from words 1 to nWordCount-2
            memset(sProductName, 0, sizeof(sProductName));
            for (i = 1; i < nWordCount - 1; i++) {
                char sTempWord[MAX_BUFFER];
                memset(sTempWord, 0, sizeof(sTempWord));
                getWord(psTrimmed, i, sTempWord, sizeof(sTempWord));
                if (i > 1) {
                    strncat(sProductName, " ", MAX_PRODUCT_NAME - strlen(sProductName) - 1);
                }
                strncat(sProductName, sTempWord, MAX_PRODUCT_NAME - strlen(sProductName) - 1);
            }

            if (nAmount <= 0) {
                printMessage("Invalid amount.\n");
                free(psLine);
                continue;
            }

            // Check if product already in trade list, if so increase amount
            nFound = 0;
            for (i = 0; i < stTradeList.nNumItems; i++) {
                if (0 == compareIgnoreCase(stTradeList.aItems[i].sProductName, sProductName)) {
                    stTradeList.aItems[i].nAmount += nAmount;
                    nFound = 1;
                    break;
                }
            }

            if (0 == nFound && stTradeList.nNumItems < MAX_TRADE_ITEMS) {
                strncpy(stTradeList.aItems[stTradeList.nNumItems].sProductName,
                        sProductName, MAX_PRODUCT_NAME - 1);
                stTradeList.aItems[stTradeList.nNumItems].nAmount = nAmount;
                stTradeList.nNumItems++;
            }

            free(psLine);
            continue;
        }

        // REMOVE command: remove <product_name> <amount>
        if (0 == compareIgnoreCase(sWord1, "remove") && nWordCount >= 3) {
            memset(sAmountStr, 0, sizeof(sAmountStr));
            getWord(psTrimmed, nWordCount - 1, sAmountStr, sizeof(sAmountStr));
            nAmount = stringToInt(sAmountStr);

            memset(sProductName, 0, sizeof(sProductName));
            for (i = 1; i < nWordCount - 1; i++) {
                char sTempWord[MAX_BUFFER];
                memset(sTempWord, 0, sizeof(sTempWord));
                getWord(psTrimmed, i, sTempWord, sizeof(sTempWord));
                if (i > 1) {
                    strncat(sProductName, " ", MAX_PRODUCT_NAME - strlen(sProductName) - 1);
                }
                strncat(sProductName, sTempWord, MAX_PRODUCT_NAME - strlen(sProductName) - 1);
            }

            // Find and reduce or remove
            for (i = 0; i < stTradeList.nNumItems; i++) {
                if (0 == compareIgnoreCase(stTradeList.aItems[i].sProductName, sProductName)) {
                    stTradeList.aItems[i].nAmount -= nAmount;
                    if (stTradeList.aItems[i].nAmount <= 0) {
                        // Remove item by shifting array
                        int j = 0;
                        for (j = i; j < stTradeList.nNumItems - 1; j++) {
                            memcpy(&stTradeList.aItems[j], &stTradeList.aItems[j + 1],
                                   sizeof(TradeItem));
                        }
                        stTradeList.nNumItems--;
                    }
                    break;
                }
            }

            free(psLine);
            continue;
        }

        // Unknown trade command
        printMessage("Unknown trade command\n");
        free(psLine);
    }
}

/********************************************************************
*
* @Name: writeTradeFile
* @Def: Writes the trade list to a text file in the Maester's
*       working directory.
* @Arg: In: pstTradeList = pointer to the trade list to write
*       In: psFolderPath = path to the Maester's working folder
* @Ret: Returns 0 on success, -1 on error.
*
********************************************************************/
static int writeTradeFile(const TradeList *pstTradeList, const char *psFolderPath) {

    char sFilePath[MAX_PATH];
    int nFd = 0;
    int i = 0;
    char sAmountBuf[32];
    int nLen = 0;

    if (NULL == pstTradeList || NULL == psFolderPath) {
        return -1;
    }

    // Build file path
    memset(sFilePath, 0, sizeof(sFilePath));
    strncpy(sFilePath, psFolderPath, MAX_PATH - 50);
    strncat(sFilePath, "/trade_", MAX_PATH - strlen(sFilePath) - 1);
    strncat(sFilePath, pstTradeList->sTargetRealm, MAX_PATH - strlen(sFilePath) - 1);
    strncat(sFilePath, ".txt", MAX_PATH - strlen(sFilePath) - 1);

    nFd = open(sFilePath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (nFd < 0) {
        printMessage("ERROR: Could not create trade file.\n");
        return -1;
    }

    for (i = 0; i < pstTradeList->nNumItems; i++) {
        nLen = strlen(pstTradeList->aItems[i].sProductName);
        write(nFd, pstTradeList->aItems[i].sProductName, nLen);
        write(nFd, " ", 1);

        // Convert amount to string
        memset(sAmountBuf, 0, sizeof(sAmountBuf));
        int nVal = pstTradeList->aItems[i].nAmount;
        int nIdx = 0;
        if (0 == nVal) {
            sAmountBuf[0] = '0';
            nIdx = 1;
        } else {
            // Build in reverse
            char sReverse[32];
            int nRevIdx = 0;
            while (nVal > 0) {
                sReverse[nRevIdx] = '0' + (nVal % 10);
                nRevIdx++;
                nVal = nVal / 10;
            }
            // Reverse into sAmountBuf
            int k = 0;
            for (k = nRevIdx - 1; k >= 0; k--) {
                sAmountBuf[nIdx] = sReverse[k];
                nIdx++;
            }
        }
        write(nFd, sAmountBuf, nIdx);
        write(nFd, "\n", 1);
    }

    close(nFd);
    return 0;
}

/********************************************************************
*
* @Name: cmdExit
* @Def: Gracefully terminates the Maester process, freeing all
*       allocated memory and printing a farewell message.
* @Arg: In: pstConfig = pointer to MaesterConfig to clean up
*       In: pstInventory = pointer to Inventory to clean up
*       In/Out: pnRunning = flag set to 0 to stop main loop
* @Ret: None.
*
********************************************************************/
static void cmdExit(MaesterConfig *pstConfig, Inventory *pstInventory,
                    int *pnRunning) {

    (void)pstInventory;

    printMessage("The Maester of ");
    printMessage(pstConfig->sRealmName);
    printMessage(" signs off. The ravens rest.\n");
    *pnRunning = 0;
}
