
#include "IUtils.h"
IUtils *iUtils;
void register_jwtBoss(const char* pn);
extern "C" void
#ifdef _WIN32
__declspec(dllexport)
#endif
#ifdef DEBUG
registerModuleDebug
#else
registerModule
#endif
(IUtils* f,const char* pn)
{

    iUtils=f;
    register_jwtBoss(pn);
}

