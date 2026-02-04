#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846

// Helper to clamp values
static double clamp_d(double value, double min, double max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Get the face index and horizontal texture coordinate (u) for the vertical walls
// Also returns distance to the wall for perspective projection
static void get_skybox_wall_params(double dir_x, double dir_y, int *face, double *u, double *dist)
{
    double abs_dx = fabs(dir_x);
    double abs_dy = fabs(dir_y);
    
    // We assume the skybox is a unit cube [-1, 1]
    // Walls are at X = 1, X = -1, Y = 1, Y = -1 (in Doom coordinates where Z is Up)
    // Wait, Doom coords differ.
    // In draw_skybox_column.c:
    // dir_z was "vertical" in the raycaster logic which was confusing.
    // Let's stick to standard 2D map: X, Y are horizontal.
    // dir_x, dir_y are the ray direction in the horizontal plane.
    
    /*
        Faces mapping (from loader):
        [0]=bottom (Floor)
        [1]=top (Ceiling)
        [2]=back 
        [3]=left
        [4]=front
        [5]=right
        
        Assumptions:
        Right (X+) -> Face 5
        Left (X-) -> Face 3
        Front (Y- ?) -> Face 4
        Back (Y+ ?) -> Face 2
    */

    if (abs_dx >= abs_dy)
    {
        // Intersecting Left or Right Plane (x = +/- 1)
        *dist = 1.0 / abs_dx;
        double y_hit = dir_y * (*dist); // Range [-1, 1]
        
        if (dir_x > 0) // East / Right
        {
            *face = 5;
            // Map y_hit [-1, 1] to u [0, 1]
            // Need to check orientation. Usually counter-clockwise.
            // If we look Right (X+), Y goes from -1 (North) to 1 (South).
            // U usually goes Left to Right.
            // So -1 -> 0, 1 -> 1.
            *u = (y_hit + 1.0) * 0.5;
        }
        else // West / Left
        {
            *face = 3;
            // Map y_hit [-1, 1] to u [0, 1]
            // Looking Left (X-), Y goes 1 (South) to -1 (North).
            // U goes Left to Right.
            // So 1 -> 0, -1 -> 1.
            *u = (1.0 - y_hit) * 0.5;
        }
    }
    else
    {
        // Intersecting Front or Back Plane (y = +/- 1)
        *dist = 1.0 / abs_dy;
        double x_hit = dir_x * (*dist); // Range [-1, 1]
        
        if (dir_y > 0) // South / Back
        {
            *face = 2; // Back
            // Looking South (Y+), X goes 1 (Right) to -1 (Left).
            // Wait, standard Cartesian:
            // Looking +Y. Right is -X? No, Right is -X.
            // Screen X goes Left->Right.
            // World: East(X+) is Left of South(Y+)? No. S is Down. E is Right.
            // Coordinate system: X right, Y down (2D map).
            // Looking +Y (Down). Right is -X (Left)?
            // X=1 is East. Y=1 is South.
            // Looking South. Left is East(1). Right is West(-1).
            // So x_hit goes 1 -> -1 (Left -> Right on texture).
            // U goes 0 -> 1.
            // So 1 -> 0, -1 -> 1.
            *u = (1.0 - x_hit) * 0.5;
        }
        else // North / Front
        {
            *face = 4; // Front
            // Looking North (Y-). Right is East(X+). Left is West(X-).
            // x_hit goes -1 -> 1.
            // U goes 0 -> 1.
            *u = (x_hit + 1.0) * 0.5;
        }
    }
    
    // Reverse U purely for texture flipping issues if commonly seen
    // *u = 1.0 - *u; 
}

void draw_skybox_geometric(t_env *env, int x, int y1, int y2, double angle)
{
    if (!env->skybox_enabled || env->current_skybox < 0) return;
    
    t_skybox *sb = &env->skyboxes[env->current_skybox];
    
    // 1. Calculate Ray Direction (Horizontal)
    double dir_x = cos(angle);
    double dir_y = sin(angle);
    
    // 2. Determine Vertical Wall params
    double dist_to_wall;
    int wall_face;
    double wall_u;
    
    get_skybox_wall_params(dir_x, dir_y, &wall_face, &wall_u, &dist_to_wall);
    
    // 3. Calculate Projected Wall Heights
    // The skybox is a cube [-1, 1] in all 3 axes.
    // Wall height is from Z = -1 to Z = 1.
    // Dist is distance to the vertical plane.
    // Projection: ScreenY = Horizon - (WorldZ / Dist) * Scale
    
    double fov_scale = (env->w / 2.0) / tan((FOV * PI / 180.0) / 2.0);
    double horizon = env->player.horizon; // Usually h/2 + pitch_offset
    
    // Top of wall (Z = 1.0)
    // Note: In OpenGL/Doom, usually Z is up.
    // Looking UP -> Screen Y decreases.
    // projected_y = horizon - (height / dist) * scale
    double y_top_proj = horizon - (1.0 / dist_to_wall) * fov_scale;
    
    // Bottom of wall (Z = -1.0)
    double y_bot_proj = horizon - (-1.0 / dist_to_wall) * fov_scale;
    
    int y_start_wall = (int)y_top_proj;
    int y_end_wall = (int)y_bot_proj;
    
    // Pre-calculate Wall Texture
    t_texture *tex_wall = &sb->textures[wall_face];
    int tex_w_wall = tex_wall->width;
    int tex_h_wall = tex_wall->height;
    int u_pix_wall = (int)(wall_u * (tex_w_wall - 1));
    if (u_pix_wall < 0) u_pix_wall = 0;
    if (u_pix_wall >= tex_w_wall) u_pix_wall = tex_w_wall - 1;
    
    // Pre-calculate Floor/Ceiling Textures
    t_texture *tex_floor = &sb->textures[0]; // Bottom
    t_texture *tex_ceil = &sb->textures[1];  // Top
    
    // Loop
    for (int y = y1; y <= y2; y++)
    {
        Uint32 color = 0;
        
        if (y < y_start_wall) // CEILING (Top Face)
        {
            if (tex_ceil->pixels)
            {
                // Raycast to Plane Z = 1
                // Ray: Origin(0,0,0), Dir(dx, dy, dz)
                // dz depends on y.
                // Screen Y = Horizon - (Z / distH) * Scale
                // (ScreenY - Horizon) / Scale = -Z / distH
                // distH = -Z * Scale / (ScreenY - Horizon)
                // For ceiling Z = 1.
                // distH = - Scale / (y - horizon)
                // y is < horizon (since y < y_start_wall which is < horizon usually)
                // (y - horizon) is negative.
                
                double dy_screen = (double)y - horizon;
                if (dy_screen > -0.001) dy_screen = -0.001; // Avoid div zero or flip
                
                double dist_h = -1.0 * fov_scale / dy_screen;
                
                double map_x = dir_x * dist_h;
                double map_y = dir_y * dist_h;
                
                // Map [-1, 1] to [0, 1]
                double u = (map_x + 1.0) * 0.5;
                double v = (map_y + 1.0) * 0.5;
                
                // Clamp
                u = clamp_d(u, 0.0, 1.0);
                v = clamp_d(v, 0.0, 1.0);
                
                int tx = (int)(u * (tex_ceil->width - 1));
                int ty = (int)(v * (tex_ceil->height - 1));
                
                color = tex_ceil->pixels[ty * tex_ceil->width + tx];
            }
        }
        else if (y > y_end_wall) // FLOOR (Bottom Face)
        {
            if (tex_floor->pixels)
            {
                // Raycast to Plane Z = -1
                // distH = - (-1) * Scale / (y - horizon) = Scale / (y - horizon)
                
                double dy_screen = (double)y - horizon;
                if (dy_screen < 0.001) dy_screen = 0.001;
                
                double dist_h = fov_scale / dy_screen;
                
                double map_x = dir_x * dist_h;
                double map_y = dir_y * dist_h;
                
                // Map [-1, 1] to [0, 1]
                double u = (map_x + 1.0) * 0.5;
                double v = (map_y + 1.0) * 0.5; // v might need invert?
                // Depending on texture orientation.
                
                u = clamp_d(u, 0.0, 1.0);
                v = clamp_d(v, 0.0, 1.0);
                
                int tx = (int)(u * (tex_floor->width - 1));
                int ty = (int)(v * (tex_floor->height - 1));
                
                color = tex_floor->pixels[ty * tex_floor->width + tx];
            }
        }
        else // WALL (Side Face)
        {
            if (tex_wall->pixels)
            {
                // Linear mapping of Y to V
                // y_start_wall -> v = 0 (Top)
                // y_end_wall -> v = 1 (Bottom)
                // double wall_height = (double)(y_end_wall - y_start_wall);
                
                // v = (y - y_start) / height
                double v = ((double)y - y_top_proj) / (y_bot_proj - y_top_proj);
                
                v = clamp_d(v, 0.0, 1.0);
                
                int v_pix = (int)(v * (tex_h_wall - 1));
                color = tex_wall->pixels[v_pix * tex_w_wall + u_pix_wall];
            }
        }
        
        env->sdl.texture_pixels[y * env->w + x] = color;
    }
}
