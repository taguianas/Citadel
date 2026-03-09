#ifndef __GLOBALS_H__
#define __GLOBALS_H__

/***********************************************
*
* @File: globals.h
* @Purpose: Global type definitions and constants
*           for the Citadel System project.
* @Author: Anas TAGUI |  Abderrahmen MESKINE
* @Date: 2026-03-06
*
***********************************************/

// System Includes
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>

// Constant Declarations
#define MAX_REALM_NAME 100
#define MAX_PATH 256
#define MAX_IP 20
#define MAX_PRODUCT_NAME 100
#define MAX_ROUTES 50
#define MAX_BUFFER 512
#define MAX_TRADE_ITEMS 50
#define PROMPT "$ "
#define TRADE_PROMPT "(trade)> "

// Custom Type Definitions
typedef struct {
    char sRealmName[MAX_REALM_NAME];
    char sIp[MAX_IP];
    int nPort;
} Route;

typedef struct {
    char sRealmName[MAX_REALM_NAME];
    char sFolderPath[MAX_PATH];
    int nNumEnvoys;
    char sIp[MAX_IP];
    int nPort;
    Route aRoutes[MAX_ROUTES];
    int nNumRoutes;
    int nDefaultRouteIndex;
} MaesterConfig;

typedef struct {
    char sName[MAX_PRODUCT_NAME];
    int nAmount;
    float fWeight;
} Product;

typedef struct {
    Product *pProducts;
    int nNumProducts;
} Inventory;

typedef struct {
    char sProductName[MAX_PRODUCT_NAME];
    int nAmount;
} TradeItem;

typedef struct {
    TradeItem aItems[MAX_TRADE_ITEMS];
    int nNumItems;
    char sTargetRealm[MAX_REALM_NAME];
} TradeList;

#endif
