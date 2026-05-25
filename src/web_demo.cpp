/**
 * @file web_demo.cpp
 * @brief Web version - Demonstration of the particle system (UI Included).
 */

#include <particles_ui.hpp>
#include <string>
#include <rlImGui.h>
#include <emscripten.h>
#include <emscripten/html5.h>

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
ParticleSpawner *particles = nullptr;
std::string particle_count = "Particles: ";

const float DBL_CLICK_THRESHOLD = 0.5;

void EmscriptenMainLoop(){
    particles->Update();

    BeginDrawing();
        ClearBackground(BLACK);

        // Start ImGui
        rlImGuiBegin();

        // Draw all particles.
        particles->Draw();

        // Draw FPS counter.
        DrawFPS(10, 10);
        // Draw particle counter.
        DrawText((particle_count + std::to_string(particles->Count())).c_str(), 10, 40, 10, WHITE);

        // Detect if a double click happened. If so, move the settings window to that position.
        static double last_click_time = 0.0;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ImGui::GetIO().WantCaptureMouse)
        {
            double now = GetTime();
        
            if ((now - last_click_time) <= DBL_CLICK_THRESHOLD)
            {
                ImGui::SetNextWindowPos(
                    {
                        static_cast<float>(GetMouseX()),
                        static_cast<float>(GetMouseY())
                    },
                    ImGuiCond_Always,
                    {0.5f, 0.5f}
                );
            
                last_click_time = 0.0;
            }
            else
            {
                last_click_time = now;
            }
        }

        // Draw particle editor.
        DrawParticleConfigUI(particles->GetConfig(), "particles");

        // Render ImGui
        rlImGuiEnd();
    EndDrawing();
}

int main(){
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);

    int canvas_size = 800;

    emscripten_set_canvas_element_size("#canvas", canvas_size, canvas_size);

    InitWindow(canvas_size, canvas_size, "rlparticle");
    SetWindowSize(canvas_size, canvas_size);

    particles = new ParticleSpawner(
    {
        .position = {
            GetScreenWidth()  / 2.0f,
            GetScreenHeight() / 2.0f
        },
        .rotation = 0,
        .scale = {1,1}
    },
    cfg,
    "example_texture.png"
    );

    // Setup ImGui
    rlImGuiSetup(true);

    emscripten_set_main_loop(EmscriptenMainLoop, 0, 1);

    // Shutdown ImGui. Never happens in web versions but it's still good practice to keep it here just in case.
    rlImGuiShutdown();

    delete particles;
};