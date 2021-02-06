
// Ensure that the following definition is in effect before winuser.h is included.
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501    
#endif

#include <windows.h>
#include <CommCtrl.h>
//#include <wincodec.h>
#include <magnification.h>
#include "resource.h"
#include <string>
// For simplicity, the sample uses a constant magnification factor.
#define RESTOREDWINDOWSTYLES WS_SIZEBOX | WS_SYSMENU | WS_CLIPCHILDREN | WS_CAPTION | WS_MAXIMIZEBOX
#define CURSORX_OFFSET 420
#define CURSORY_OFFSET 190
#define CURSORX_OFFSET_M 510
#define CURSORY_OFFSET_M 300
#define CURSORX_OFFSET_L 595
#define CURSORY_OFFSET_L 400
#define IDI_ICON   101
#define IDB_RADIO1 102
#define IDB_RADIO2 103
#define IDB_RADIO3 104
#define IDB_RADIO4 105
#define IDB_RADIO5 106

#define IDB_RADIO6 107
#define IDB_RADIO7 108
#define IDB_RADIO8 109
#define IDB_RADIO9 110
#define ID_BUTTON 111
#define ID_CONTRAST 112
#define ID_SCROLL 113
using namespace std;