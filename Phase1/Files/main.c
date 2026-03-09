/***********************************************
*
* @File: main.c
* @Purpose: Entry point for the Maester process.
*           Reads configuration and inventory files,
*           then runs the interactive command terminal.
*           Usage: Maester <config.dat> <stock.db>
* @Author: Anas TAGUI |  Abderrahmen MESKINE
* @Date: 2026-03-06
*
***********************************************/

#include "main.h"

// Global variables for signal handler cleanup
static MaesterConfig gstConfig;
static Inventory gstInventory;
static int gnRunning = 1;

/********************************************************************
*
* @Name: handleSignal
* @Def: Signal handler for SIGINT (CTRL+C). Performs graceful
*       shutdown by freeing resources and exiting cleanly.
* @Arg: In: nSignal = signal number received
* @Ret: None.
*
********************************************************************/
static void handleSignal(int nSignal) {

    if (SIGINT == nSignal) {
        printMessage("\n");
        printMessage("The Maester of ");
        printMessage(gstConfig.sRealmName);
        printMessage(" signs off. The ravens rest.\n");
        freeInventory(&gstInventory);
        freeConfig(&gstConfig);
        gnRunning = 0;
        _exit(0);
    }
}

/********************************************************************
*
* @Name: main
* @Def: Entry point of the Maester process. Parses command line
*       arguments, loads configuration and inventory, then enters
*       the interactive command loop.
* @Arg: In: argc = argument count (must be 3)
*       In: argv = argument values [program, config.dat, stock.db]
* @Ret: Returns 0 on clean exit, 1 on error.
*
********************************************************************/
int main(int argc, char *argv[]) {

    char *psLine = NULL;
    struct sigaction stAction;

    // Validate arguments
    if (NUM_REQUIRED_ARGS != argc) {
        printMessage("Usage: Maester <config.dat> <stock.db>\n");
        return 1;
    }

    // Set up signal handler for SIGINT
    memset(&stAction, 0, sizeof(stAction));
    stAction.sa_handler = handleSignal;
    sigemptyset(&stAction.sa_mask);
    stAction.sa_flags = 0;
    sigaction(SIGINT, &stAction, NULL);

    // Initialize structures
    memset(&gstConfig, 0, sizeof(MaesterConfig));
    memset(&gstInventory, 0, sizeof(Inventory));

    // Parse configuration file
    if (0 != parseConfig(argv[1], &gstConfig)) {
        printMessage("ERROR: Failed to parse configuration file.\n");
        return 1;
    }

    // Load inventory
    if (0 != loadInventory(argv[2], &gstInventory)) {
        printMessage("ERROR: Failed to load inventory file.\n");
        freeConfig(&gstConfig);
        return 1;
    }

    // Print initialization message
    printMessage("Maester of ");
    printMessage(gstConfig.sRealmName);
    printMessage(" initialized. The board is set.\n");

    // Main command loop
    gnRunning = 1;
    while (gnRunning) {
        printMessage(PROMPT);
        psLine = readLine(STDIN_FILENO);

        if (NULL == psLine) {
            // EOF received
            break;
        }

        processCommand(psLine, &gstConfig, &gstInventory, &gnRunning);
        free(psLine);
    }

    // Cleanup
    freeInventory(&gstInventory);
    freeConfig(&gstConfig);

    return 0;
}
