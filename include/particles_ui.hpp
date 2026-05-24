#ifndef PARTICLES_UI_H
#define PARTICLES_UI_H

#include "particles.hpp"
#include <imgui.h>

/**
 * @brief Draws a Dear ImGui interface that allows for real-time editing of a 
 * given particle configuration. Use alongside ParticleSpawner::GetConfig() to 
 * update a particle spawner in real-time.
 * 
 * @param cfg A reference to a ParticleConfig object. Said object's data will be
 * modifiable via the drawn GUI.
 * @param tag A name for the ImGui window.
 * @warning Do not draw more than one `DrawParticleConfigUI` with the same @b tag,
 * as these are used internally by @b Dear @b ImGui.
 */
void DrawParticleConfigUI(ParticleConfig &cfg, const char *tag);


#endif // PARTICLES_UI_H
