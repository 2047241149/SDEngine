#pragma once
#include "SDEngine/Log.h"

#define HR(X) {if(FAILED(X)) { Log::Error("Create Failed, file = {0}, line = {1}, func = {2}", __FILE__, __LINE__, __FUNCTION__); return false;}}
#define ReleaseCOM(x) { if (x) { x->Release(); x = 0; } }
#define ReleaseShader(x) { if (x) { x->Shutdown();delete x; x = 0; } }
#define CheckoutPtr {if(FAILED(X)) { return FAILED; }}