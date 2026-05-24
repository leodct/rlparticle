/**
 * @file main.cpp
 * @brief Demonstration of the particle system.
 */

#include <particles.hpp>
#include <string>

int main(){
    InitWindow(800, 600, "rlparticle");
    
    ParticleConfig cfg = {
        .acceleration = {
            .value = {0,0},
            .variation = {150, 150}
        },
        .blending = true,
        .emission = 500,
        .fade_in = {
            .value = 0.05,
            .variation = 0.025
        },
        .fade_out = {
            .value = 0.5,
            .variation = 0.2
        },
        .initial_velocity = {
            .value = {0,0},
            .variation = {0,0},
        },
        .lifetime = {
            .value = 1,
            .variation = 0.5
        },
        .MAX_PARTICLES = PARTICLE_CAP,
        .position_var = {80, 80},
        .rgb_var_end = {
            .r = 90,
            .g = 50,
            .b = 20,
            .a = 0
        },
        .rgb_var_start = {
            .r = 0,
            .g = 30,
            .b = 50,
            .a = 0
        },
        .start_colour = {
            .r = 50,
            .g = 100,
            .b = 250,
            .a = 255
        },
        .start_rotation = {
            .value = 0,
            .variation = 360
        },
        .start_scale = {
            .value = {1,1},
            .variation = {0,0}
        },
        .target_colour = {
            .r = 220,
            .g = 60,
            .b = 50,
            .a = 255
        },
        .target_rotation = {
            .value = 0,
            .variation = 360
        },
        .target_scale = {
            .value = {1,1},
            .variation = {0,0}
        }
    };

    ParticleSpawner particles(
        {
            .position = {400, 300},
            .rotation = 0,
            .scale = {1,1}
        },
        cfg,
        "example_texture.png"
    );
    
    std::string particle_count = "Particles: ";

    while (!WindowShouldClose())
    {
        particles.Update();

        BeginDrawing();
            ClearBackground(BLACK);

            particles.Draw();
            DrawFPS(10, 10);
            DrawText((particle_count + std::to_string(particles.Count())).c_str(), 10, 40, 10, WHITE);

        EndDrawing();
    }
};