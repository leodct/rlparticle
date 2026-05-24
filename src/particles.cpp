#include "particles.hpp"
#include <cmath>

// Auxiliary functions
inline Transform2D TransformMin(const Transform2D &a, const Transform2D &b){
    return
        {
            // Position
            {
                std::min(a.position.x, b.position.x),
                std::min(a.position.y, b.position.y)
            },
            // Rotation
                std::min(a.rotation, b.rotation),
            // Scale
            {
                std::min(a.scale.x, b.scale.x),
                std::min(a.scale.y, b.scale.y)
            }
        };
}
inline Transform2D TransformMax(const Transform2D &a, const Transform2D &b){
    return
        {
            // Position
            {
                std::max(a.position.x, b.position.x),
                std::max(a.position.y, b.position.y)
            },
            // Rotation
                std::max(a.rotation, b.rotation),
            // Scale
            {
                std::max(a.scale.x, b.scale.x),
                std::max(a.scale.y, b.scale.y)
            }
        };
}
// Return a random value within [min, max]
inline float GetRandomFloat(float min, float max){
    return min + (max - min) * (rand() / (float)RAND_MAX);
}
// Lerp a colour
/*inline Color LerpCol(Color origin, Color dest, float t){
    return {
        (std::uint8_t)std::clamp(std::lerp(origin.r, dest.r, t), 0.0, 255.0),
        (std::uint8_t)std::clamp(std::lerp(origin.g, dest.g, t), 0.0, 255.0),
        (std::uint8_t)std::clamp(std::lerp(origin.b, dest.b, t), 0.0, 255.0),
        (std::uint8_t)std::clamp(std::lerp(origin.a, dest.a, t), 0.0, 255.0),
    };
}*/
/**
 * Calculate fading modifier
 * @param fade_in [0,1]
 * @param fade_out [0,1]
 * @param t [0,1]
 * @retval The fading multiplier `[0,1]`
 */
inline float CalculateFade(float fade_in, float fade_out, float t){
    float fin = 1;
    float fout = 1;
    if ((fade_in > 0) && (t < fade_in)){
        fin = t / fade_in;
    }
    if ((fade_out > 0) && (t > 1.0f - fade_out))
    {
        fout = 1.0f - ((t - (1.0f - fade_out)) / fade_out);
    }
    return std::clamp(fin * fout, 0.0f, 1.0f);
}

inline Transform2D GetRandomTransform(const Transform2D &min, const Transform2D &max){
    return {
        {
            GetRandomFloat(min.position.x, max.position.x),
            GetRandomFloat(min.position.y, max.position.y)
        },
        GetRandomFloat(min.rotation, max.rotation),
        {
            GetRandomFloat(min.scale.x, max.scale.x),
            GetRandomFloat(min.scale.y, max.scale.y)
        }
    };
}


ParticleSpawner::ParticleSpawner(Transform2D _transform, ParticleConfig _config, const char *texture_name) : 
config(
  ParticleConfig{
    .acceleration = _config.acceleration,
    .blending     = _config.blending,
    // [0,+inf)
    .emission = std::max(_config.emission, 0.0f),
    .fade_in  = _config.fade_in,
    .fade_out = _config.fade_out,
    .initial_velocity = _config.initial_velocity,
    .lifetime         = _config.lifetime,
    .MAX_PARTICLES    = std::min(_config.MAX_PARTICLES, PARTICLE_CAP),
    .position_var     = _config.position_var,
    .rgb_var_end      = _config.rgb_var_end,
    .rgb_var_start    = _config.rgb_var_start,
    .start_colour     = _config.start_colour,
    .start_rotation   = _config.start_rotation,
    .start_scale      = _config.start_scale,
    .target_colour    = _config.target_colour,
    .target_rotation  = _config.target_rotation,
    .target_scale     = _config.target_scale
  }  
),
particle_texture(LoadTexture(texture_name))
{
    particles.reserve(config.MAX_PARTICLES);
    transform  = _transform;
    spawn_accumulator = 0;
}

ParticleSpawner::~ParticleSpawner()
{
    UnloadTexture(particle_texture);
}

void ParticleSpawner::Update()
{
    float dt = GetFrameTime();
    // Clean up dead particles
    {
        // This thing rearranges the entire particle list and marks for removal all dead ones by
        // moving them after the iterator it returns, which becomes the new `end()`
        auto nend = std::remove_if(
            particles.begin(), 
            particles.end(), 
            [](const Particle &p){return p.IsDead();}
        );
        // And now we actually get rid of them
        particles.erase(nend, particles.end());
    }

    // Spawn new particles
    spawn_accumulator += dt * config.emission;
    while (spawn_accumulator >= 1 && Count() < config.MAX_PARTICLES){
        //std::cout << "ATTEMPTING PARTICLE SPAWN!\n";
        SpawnParticle();
        spawn_accumulator -= 1;
    }

    // Update 'em all
    for (auto &p : particles){
        p.Update(dt);
    }
}

void ParticleSpawner::Draw() const
{
    if (config.blending){
        BeginBlendMode(BLEND_ADDITIVE);
    }
    else {
        BeginBlendMode(BLEND_ALPHA);
    }
    
    Vector2 ptex = { (float)particle_texture.width, (float)particle_texture.height };
    Color particle_col;
    for (const auto &p : particles){
        const ParticleData &pdata = p.GetData();
        Vector2 ppos = pdata.position;
        Rectangle rsource = {0, 0, ptex.x, ptex.y};
        Rectangle rdest   = { 
            ppos.x,
            ppos.y,
            ptex.x * pdata.scale.current.x,
            ptex.y * pdata.scale.current.y
        };
        float t = std::clamp(pdata.age / pdata.lifetime, 0.0f, 1.0f);
        particle_col    = pdata.tint.current;
        particle_col.a *= CalculateFade(pdata.fade_in, pdata.fade_out, t);


        DrawTexturePro(
            particle_texture,
            rsource,
            rdest,
            {
                rdest.width / 2.0f,
                rdest.height / 2.0f
            },
            pdata.rotation.current,
            particle_col
        );

    }
    EndBlendMode();
    //DrawRectanglePro(Rectangle{transform.position.x, transform.position.y, 10, 10}, {5, 5}, 0, LIME);
    //DrawTexturePro(*particle_texture, {0, 0, (float)particle_texture->width, (float)particle_texture->height}, {transform.position.x, transform.position.y, 50, 50}, {25, 25}, 0, WHITE);
}

ParticleConfig &ParticleSpawner::GetConfig()
{
    return config;
}

const ParticleConfig &ParticleSpawner::GetConfig() const
{
    return config;
}

int ParticleSpawner::Count() const
{
    return particles.size();
}

// ============================================================================================================
// ------------------------------------------------------------------------------------------------------------
// ============================================================================================================

ParticleSpawner::Particle::Particle(ParticleData _data) :
data(_data)
{
}

void ParticleSpawner::Particle::Update(float dt)
{
    data.age += dt;

    // Physics thingiemagik yay!
    data.velocity.x += data.acceleration.x * dt;
    data.velocity.y += data.acceleration.y * dt;

    data.position.x += data.velocity.x * dt;
    data.position.y += data.velocity.y * dt;

    float t = data.age * data.inverse_lifetime; // Division is expensive, it's better to directly store 1/lifetime.
    t = std::clamp(t, 0.0f, 1.0f);

    data.rotation.current = std::lerp(data.rotation.origin, data.rotation.target, t);
    data.scale.current.x  = std::lerp(data.scale.origin.x, data.scale.target.x, t);
    data.scale.current.y  = std::lerp(data.scale.origin.y, data.scale.target.y, t);
    data.tint.current = {
        .r = static_cast<uint8_t>(std::clamp(std::lerp(data.tint.origin.r, data.tint.target.r, t), 0.0, 255.0)),
        .g = static_cast<uint8_t>(std::clamp(std::lerp(data.tint.origin.g, data.tint.target.g, t), 0.0, 255.0)),
        .b = static_cast<uint8_t>(std::clamp(std::lerp(data.tint.origin.b, data.tint.target.b, t), 0.0, 255.0)),
        .a = static_cast<uint8_t>(std::clamp(std::lerp(data.tint.origin.a, data.tint.target.a, t), 0.0, 255.0))
    };
}

/*void ParticleSpawner::Particle::Draw() const
{
    Rectangle rsource = {0, 0, (float)texture->width, (float)texture->height};
    Rectangle rdest   = { current_transform.position.x, current_transform.position.y, texture->width * current_transform.scale.x, texture->height * current_transform.scale.y };
    DrawTexturePro(*texture, rsource, rdest, {0.5, 0.5}, current_transform.rotation, WHITE);
}*/

bool ParticleSpawner::Particle::IsDead() const
{
    return data.age > data.lifetime;
}

const ParticleData& ParticleSpawner::Particle::GetData() const
{
    return data;
}

inline void ParticleSpawner::SpawnParticle()
{
    float startrot = transform.rotation + config.start_rotation.value + GetRandomFloat(-config.start_rotation.variation, config.start_rotation.variation);
    Vector2 startscale = {
                transform.scale.x + config.start_scale.value.x + GetRandomFloat(-config.start_scale.variation.x, config.start_scale.variation.x),
                transform.scale.y + config.start_scale.value.y + GetRandomFloat(-config.start_scale.variation.y, config.start_scale.variation.y)
    };

    ParticleData data;
        data.acceleration = {
            config.acceleration.value.x + GetRandomFloat(-config.acceleration.variation.x, config.acceleration.variation.x),
            config.acceleration.value.y + GetRandomFloat(-config.acceleration.variation.y, config.acceleration.variation.y)
        };
        data.age = 0;
        data.lifetime = config.lifetime.value + GetRandomFloat(-config.lifetime.variation, config.lifetime.variation);
        data.inverse_lifetime = 1 / data.lifetime;
        data.position = {
            transform.position.x + GetRandomFloat(-config.position_var.x, config.position_var.x),
            transform.position.y + GetRandomFloat(-config.position_var.y, config.position_var.y)
        };
        data.rotation = {
            .origin  = startrot,
            .current = startrot,
            .target  = transform.rotation + config.target_rotation.value + GetRandomFloat(-config.target_rotation.variation, config.target_rotation.variation)
        };
        data.scale = {
            .origin  = startscale,
            .current = startscale,
            .target  = {
                transform.scale.x + config.target_scale.value.x + GetRandomFloat(-config.target_scale.variation.x, config.target_scale.variation.x),
                transform.scale.y + config.target_scale.value.y + GetRandomFloat(-config.target_scale.variation.y, config.target_scale.variation.y)
            }
        };
        data.velocity = {
            config.initial_velocity.value.x + GetRandomFloat(-config.initial_velocity.variation.x, config.initial_velocity.variation.x),
            config.initial_velocity.value.y + GetRandomFloat(-config.initial_velocity.variation.y, config.initial_velocity.variation.y)
        };
        data.fade_in  = config.fade_in.value  + GetRandomFloat(-config.fade_in.variation, config.fade_in.variation);
        data.fade_out = config.fade_out.value + GetRandomFloat(-config.fade_out.variation, config.fade_out.variation);

        auto RandomChannel = [](int channel, int variation){
            return (uint8_t)GetRandomValue(std::max(0, channel - variation), std::min(255, channel + variation));
        };

        data.tint.origin = data.tint.current = {
            .r = RandomChannel(config.start_colour.r, config.rgb_var_start.r),
            .g = RandomChannel(config.start_colour.g, config.rgb_var_start.g),
            .b = RandomChannel(config.start_colour.b, config.rgb_var_start.b),
            .a = RandomChannel(config.start_colour.a, config.rgb_var_start.a)
        };
        data.tint.target = {
            .r = RandomChannel(config.target_colour.r, config.rgb_var_end.r),
            .g = RandomChannel(config.target_colour.g, config.rgb_var_end.g),
            .b = RandomChannel(config.target_colour.b, config.rgb_var_end.b),
            .a = RandomChannel(config.target_colour.a, config.rgb_var_end.a)
        };
    particles.emplace_back(data);
}