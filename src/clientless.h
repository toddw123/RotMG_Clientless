#pragma once

#ifndef CLIENTLESS_H
#define CLIENTLESS_H

// Include standard c/c++ headers
#include <sstream>
#include <math.h>
#include <vector>
#include <iterator>
#include <algorithm>

//Include curl headers
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/curlbuild.h>

// Misc
//#include "pugixml/pugixml.hpp"
#include "Client.h"
#include "utilities/ConnectionHelper.h"
#include "utilities/DebugHelper.h"
#include "utilities/CryptoHelper.h"
#include "objects/ObjectLibrary.h"


#endif // !CLIENTLESS_H
