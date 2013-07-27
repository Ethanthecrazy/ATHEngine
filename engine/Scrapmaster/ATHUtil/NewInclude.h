#ifndef NEWINCLUDE_H
#define NEWINCLUDE_H

#include "OverloadedNew.h"
#include <string.h>

// A custom file macro for getting just the end of the filepath
#define __CUSTOM_FILE__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

// A macro for new so that the custom fielpath is already used
#define new new( __CUSTOM_FILE__ )

#endif