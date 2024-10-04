#include "G_PushSet.h"

#include <ZC/Tools/Math/ZC_Vec.h>

G_PushSet::G_PushSet(const ZC_Vec3<float>& _dir, float _power)
    : dir(_dir == ZC_Vec3<float>() ? _dir : ZC_Vec::Normalize(_dir)),
    power(_power)
{}