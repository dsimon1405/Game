#pragma once

enum G_ObjectType
{
    G_OT__None          = 0,
    G_OT__Pushable      = 1,
    G_OT__Damageable    = 1 << 1,
    G_OT__Ground        = 1 << 2,
    G_OT__Player        = 1 << 3,
    G_OT__Star          = 1 << 4,
    G_OT__Platform      = 1 << 5,
};
typedef int G_ObjectTypeMask;