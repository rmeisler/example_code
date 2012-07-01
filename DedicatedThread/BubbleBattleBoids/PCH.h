#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <cmath>
#include <cstdlib>
#include "../Dependencies/glut.h"
#include <xutility>
#include <Xinput.h>
#include <intsafe.h>

// Adding library include
#pragma comment(lib, "XInput.lib")

// RenderThread includes
#include "../../Common/ConcurrentQueue.hpp"
#include <vector>

#include "Debug.h"