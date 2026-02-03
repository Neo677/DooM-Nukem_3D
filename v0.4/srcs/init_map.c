#include "env.h"

// Prototype
int load_map(t_env *env, const char *filename);

void init_map(t_env *env)
{
    // Charger la map par defaut
    // TODO: Gerer argv[1] pour map custom plus tard
    if (load_map(env, "maps/default.map") != 0)
    {
        printf("Failed to load map. Creating fallback 8x8 map.\n");
        // Fallback simple si echec
        env->map.width = 8;
        env->map.height = 8;
        env->map.grid = malloc(sizeof(int*) * 8);
        for(int i=0; i<8; i++) {
            env->map.grid[i] = calloc(8, sizeof(int));
            if(i==0 || i==7) {
                for(int j=0; j<8; j++) env->map.grid[i][j] = 1;
            } else {
                env->map.grid[i][0] = 1;
                env->map.grid[i][7] = 1;
            }
        }
    }
}