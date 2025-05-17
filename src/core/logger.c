#include <genesis.h>
#include <types.h>
#include "xtypes.h"
#include "kdebug.h"

void debug_log(const char* format, ...) {
    char buffer[128];
  
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
  
    KDebug_Alert(buffer); // Pode adaptar para exibir onde quiser
  }