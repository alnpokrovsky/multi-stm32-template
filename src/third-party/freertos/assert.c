#include "FreeRTOSConfig.h"

void vAssertCalled(const char* pcFile, unsigned long ulLine) {
    (void)pcFile;
    (void)ulLine;
}
