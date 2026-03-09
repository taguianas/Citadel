/***********************************************
*
* @File: config.c
* @Purpose: Implementation of the configuration file
*           parser for maester.dat files.
* @Author: Anas TAGUI |  Abderrahmen MESKINE
* @Date: 2026-03-06
*
***********************************************/

#include "config.h"

/********************************************************************
*
* @Name: parseConfig
* @Def: Parses the maester.dat configuration file and populates
*       the MaesterConfig structure with realm info and routes.
* @Arg: In: psFilePath = path to the maester.dat file
*       Out: pstConfig = pointer to MaesterConfig struct to fill
* @Ret: Returns 0 on success, -1 on error.
*
********************************************************************/
int parseConfig(const char *psFilePath, MaesterConfig *pstConfig) {

    int nFd = 0;
    char *psLine = NULL;
    char *psTrimmed = NULL;
    int nRouteIndex = 0;
    char sWord1[MAX_BUFFER];
    char sWord2[MAX_BUFFER];
    char sWord3[MAX_BUFFER];
    int nWordCount = 0;

    if (NULL == psFilePath || NULL == pstConfig) {
        return -1;
    }

    // Initialize config
    memset(pstConfig, 0, sizeof(MaesterConfig));
    pstConfig->nDefaultRouteIndex = -1;

    nFd = open(psFilePath, O_RDONLY);
    if (nFd < 0) {
        printMessage("ERROR: Could not open configuration file.\n");
        return -1;
    }

    // Line 1: Realm name
    psLine = readLine(nFd);
    if (NULL == psLine) {
        close(nFd);
        return -1;
    }
    psTrimmed = trimWhitespace(psLine);
    strncpy(pstConfig->sRealmName, psTrimmed, MAX_REALM_NAME - 1);
    stripAmpersand(pstConfig->sRealmName);
    free(psLine);

    // Line 2: Folder path
    psLine = readLine(nFd);
    if (NULL == psLine) {
        close(nFd);
        return -1;
    }
    psTrimmed = trimWhitespace(psLine);
    strncpy(pstConfig->sFolderPath, psTrimmed, MAX_PATH - 1);
    free(psLine);

    // Line 3: Number of envoys
    psLine = readLine(nFd);
    if (NULL == psLine) {
        close(nFd);
        return -1;
    }
    psTrimmed = trimWhitespace(psLine);
    pstConfig->nNumEnvoys = stringToInt(psTrimmed);
    free(psLine);

    // Line 4: IP address
    psLine = readLine(nFd);
    if (NULL == psLine) {
        close(nFd);
        return -1;
    }
    psTrimmed = trimWhitespace(psLine);
    strncpy(pstConfig->sIp, psTrimmed, MAX_IP - 1);
    free(psLine);

    // Line 5: Port
    psLine = readLine(nFd);
    if (NULL == psLine) {
        close(nFd);
        return -1;
    }
    psTrimmed = trimWhitespace(psLine);
    pstConfig->nPort = stringToInt(psTrimmed);
    free(psLine);

    // Line 6: "--- ROUTES ---" separator
    psLine = readLine(nFd);
    if (NULL == psLine) {
        close(nFd);
        return -1;
    }
    free(psLine);

    // Remaining lines: routes
    nRouteIndex = 0;
    while (1) {
        psLine = readLine(nFd);
        if (NULL == psLine) {
            break;
        }

        psTrimmed = trimWhitespace(psLine);
        if ('\0' == *psTrimmed) {
            free(psLine);
            continue;
        }

        nWordCount = countWords(psTrimmed);

        if (nWordCount >= 3) {
            // Format: RealmName IP Port  OR  DEFAULT IP Port
            memset(sWord1, 0, sizeof(sWord1));
            memset(sWord2, 0, sizeof(sWord2));
            memset(sWord3, 0, sizeof(sWord3));

            getWord(psTrimmed, 0, sWord1, sizeof(sWord1));
            getWord(psTrimmed, 1, sWord2, sizeof(sWord2));
            getWord(psTrimmed, 2, sWord3, sizeof(sWord3));

            if (0 == compareIgnoreCase(sWord1, "DEFAULT")) {
                // Default route
                strncpy(pstConfig->aRoutes[nRouteIndex].sRealmName, "DEFAULT", MAX_REALM_NAME - 1);
                strncpy(pstConfig->aRoutes[nRouteIndex].sIp, sWord2, MAX_IP - 1);
                pstConfig->aRoutes[nRouteIndex].nPort = stringToInt(sWord3);
                pstConfig->nDefaultRouteIndex = nRouteIndex;
            } else {
                // Named route
                strncpy(pstConfig->aRoutes[nRouteIndex].sRealmName, sWord1, MAX_REALM_NAME - 1);
                stripAmpersand(pstConfig->aRoutes[nRouteIndex].sRealmName);
                strncpy(pstConfig->aRoutes[nRouteIndex].sIp, sWord2, MAX_IP - 1);
                pstConfig->aRoutes[nRouteIndex].nPort = stringToInt(sWord3);
            }

            nRouteIndex++;
        }

        free(psLine);

        if (nRouteIndex >= MAX_ROUTES) {
            break;
        }
    }

    pstConfig->nNumRoutes = nRouteIndex;
    close(nFd);

    return 0;
}

/********************************************************************
*
* @Name: freeConfig
* @Def: Frees any dynamically allocated memory in a MaesterConfig.
* @Arg: In/Out: pstConfig = pointer to MaesterConfig to clean up
* @Ret: None.
*
********************************************************************/
void freeConfig(MaesterConfig *pstConfig) {

    // Currently no dynamic allocation inside config, but kept for future
    if (NULL == pstConfig) {
        return;
    }
    memset(pstConfig, 0, sizeof(MaesterConfig));
}
