#pragma once

#define WIN32_LEAN_AND_MEAN

// Windows Header Files
#define NOMINMAX
#define NOGDI
#define NOGDICAPMASKS
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// Common headers
#include <sstream>
#include <array>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cassert>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <time.h>
#include <iomanip>
#include <type_traits>
#include <queue>
#include <optional>
#include <algorithm>
#include <bitset>
#include <typeindex>
#include <future>
#include <math.h>
#include <limits>
#include <utility>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui-SFML.h"

#include "JSON/json/json.h"
#include "JSON/json/json-forwards.h"

#include "Box2D/box2d.h"

#include "Core/Utility.h"
#include "Core/OSUtility.h"
#include "Core/Logging.h"