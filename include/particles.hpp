#ifndef PARTICLES_H
#define PARTICLES_H

#include <raylib.h>
#include <chrono>
#include <vector>

#ifndef _TRANSFORM2D
/**
 * @name The Transform2D Struct.
 * @brief This struct stores the position, rotation, and scale of an object in 2D space. 
 * 
 * @param position A `Vector2` representing the position of the object in 2D space.
 * @param rotation A `float` value representing the rotation of the object in degrees.
 * @param scale A `Vector2` representing the scaling factor of the object, where `1.0f` is the default (no scaling).
 */
 typedef struct Transform2D {
    /**
     * @brief The position of the object in 2D space.
     */
    Vector2 position = {0, 0};

    /**
     * @brief The rotation of the object in radians.
     */
    float rotation = 0.0f;

    /**
     * @brief The scaling factor of the object.
     */
    Vector2 scale = {1.0f, 1.0f};
} Transform2D;
#define _TRANSFORM2D
#endif


constexpr int PARTICLE_CAP = 1e6; // 1 000 000 particles is the absolute maximum ever

template <typename T>
struct VariableValue{
    T value, variation;
};
template <typename T>
struct DataPack{
    T origin, current, target;
};

struct ParticleConfig{
    VariableValue<Vector2> acceleration     = {{0,0},{0,0}};     // Constant acceleration range.
    bool                   blending         = true;              // Additive blending mode
    float                  emission         = 1;                 // Particles per second.
    VariableValue<float>   fade_in          = {0,0};             // % of lifetime spent fading in [0,1].
    VariableValue<float>   fade_out         = {0,0};             // % of lifetime spent fading out [0,1].
    VariableValue<Vector2> initial_velocity = {{0,0},{0,0}};     // Starting velocity range.
    VariableValue<float>   lifetime         = {0.5,0.5};         // Maximum particle age (seconds) range.
    const int              MAX_PARTICLES    = PARTICLE_CAP;      // Particle limit. If reached, no more particles shall spawn.
    Vector2                position_var     = {0,0};             // Starting position range [origin - pos_var, origin + pos_var].  
    Color                  rgb_var_end      = {0,0,0,0};         // How much variation can a particle's end tint have.
    Color                  rgb_var_start    = {0,0,0,0};         // How much variation can a particle's starting tint have.
    Color                  start_colour     = {255,255,255,255}; // Initial particle colour.
    VariableValue<float>   start_rotation   = {0,0};             // Starting rotation range.
    VariableValue<Vector2> start_scale      = {{1,1},{1,1}};     // Starting scale range.
    Color                  target_colour    = {255,255,255,255}; // Target particle colour.
    VariableValue<float>   target_rotation  = {0,0};             // Target rotation range.
    VariableValue<Vector2> target_scale     = {{1,1},{1,1}};     // Target scale range.
};

struct ParticleData{
    Vector2 position, velocity, acceleration;
    DataPack<Vector2> scale;
    DataPack<float> rotation;
    DataPack<Color> tint;
    float age, lifetime, inverse_lifetime;
    float fade_in, fade_out;
};

class ParticleSpawner {
    public:
    /**
     * @brief Construct a new `ParticleSpawner` object.
     * @param transform Transform data for the object. Includes position, rotation and scale.
     * @param config Particle configuration dictating the object's behaviour.
     * @param texture_path The path to the texture to be used as particles.
     * @warning Create only after window initialization.
     */
    ParticleSpawner(Transform2D transform, ParticleConfig config, const char *texture_path);
    ~ParticleSpawner();

    /**
     * @brief Update both the spawner and all it's associated particle objects.
     * @post All particles will have been updated and expired particles will be removed from storage.
     */
    void Update();

    /**
     * @brief Draw all particles in storage.
     */
    void Draw() const;

    /**
     * @brief Get a reference to the object's internal `ParticleConfig`.
     * @retval `config` - A reference to the object's configuration.
     */
    ParticleConfig &GetConfig();
    /**
     * @brief Get a @b const reference to the object's internal `ParticleConfig`.
     * @retval `config` - A @b const reference to the object's configuration.
     */
    const ParticleConfig &GetConfig() const;
    /**
     * @brief Count how many particles the object owns.
     * @retval `particles.size()` - The amount of active particles that belong to this object.
     */
    int Count() const;

    /**
     * @brief Get a reference to the object's transform data.
     */
    Transform2D &GetTransform();

    /**
     * @brief Get a @b const reference to the object's transform data.
     */
    const Transform2D &GetTransform() const;
    
    private:
    class Particle {
    public:
        Particle(ParticleData data);

        void Update(float dt);
        //void Draw() const;
        bool IsDead() const;

        const ParticleData& GetData() const;

    private:
        ParticleData data;
    };

    std::vector<Particle> particles; // We'll see how memory efficient this is... I don't wanna mess around with raw pointers unless strictly necessary.
    float spawn_accumulator;         // Keep track of how many particles to spawn.
    Transform2D transform;           // This object's transform data.

    // -------------------
    // Config
    ParticleConfig config;
    // ---------------------
    const Texture2D particle_texture;

    // Auxiliary functions for spawning individual particles.
    inline void SpawnParticle();
};

#endif // PARTICLES_H