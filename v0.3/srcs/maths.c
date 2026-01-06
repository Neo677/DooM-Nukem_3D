#include "../header/global.h"

t_v2 rotate(t_v2 v, f32 a)
{
    f32 c = cosf(a);
    f32 s = sinf(a);
    
    return (t_v2) { v.x * c - v.y * s, v.x * s + v.y * c };
}

t_v2 intersect_segs(t_v2 a0, t_v2 a1, t_v2 b0, t_v2 b1)
{
    f32 d = ((a0.x - a1.x) * (b0.y - b1.y)) 
           - ((a0.y - a1.y) * (b0.x - b1.x));

    if (fabsf(d) < 0.000001f)
        return (t_v2) {NAN, NAN};

    f32 t = (((a0.x - b0.x) * (b0.y - b1.y)) - ((a0.y - b0.y) * (b0.x - b1.x))) / d;
    f32 u = (((a0.x - b0.x) * (a0.y - a1.y)) - ((a0.y - b0.y) * (a0.x - a1.x))) / d;

    if (t < 0.0f || t > 1.0f || u < 0.0f || u > 1.0f)
        return (t_v2) {NAN, NAN};

    return (t_v2) { a0.x + (t * (a1.x - a0.x)), a0.y + (t * (a1.y - a0.y)) };
}

u32 abgr_mul(u32 col, u32 a) 
{
    u32 br, g;

    br = ((col & 0xFF00FF) * a) >> 8;
    g = ((col & 0x00FF00) * a) >> 8;
    return 0xFF00000 | (br & 0xFF00FF) | (g & 0x00FF00);
}

int screenAngleToX(f32 angle) {
    f32 ratio = (angle + (HFOV / 2.0f)) / HFOV;
    f32 screenAngle = (ratio * PI) - PI_2;
    return (int)((SCREENW / 2) * (1.0f + tanf(screenAngle)));
    // return ((int) (SCREENW / 2)) * (1.0f - tan(((angle + (HFOV / 2.0)) / HFOV) * PI_2 - PI_4));
}

f32 normalizeAngle(f32 a) {
    return (a - (TAU * floorf((a + PI) / TAU)));
}

t_v2 worldPosToCamera(const t_engine *engine, t_v2 p) {
    t_v2 u = { p.x - engine->camera.pos.x, p.y - engine->camera.pos.y };

    return (t_v2) { 
        u.x * engine->camera.cosA - u.y * engine->camera.sinA,
        u.x * engine->camera.sinA + u.y * engine->camera.cosA
    };
}

