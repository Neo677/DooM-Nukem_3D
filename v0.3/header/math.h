#ifndef MATH_H
# define MATH_H

# include "global.h"

t_v2 rotate(t_v2 v, f32 a);
t_v2 intersect_segs(t_v2 a0, t_v2 a1, t_v2 b0, t_v2 b1);
u32 abgr_mul(u32 col, u32 a);
int screenAngleToX(f32 angle);
f32 normalizeAngle(f32 a);
t_v2 worldPosToCamera(const t_engine *engine, t_v2 p);


#endif