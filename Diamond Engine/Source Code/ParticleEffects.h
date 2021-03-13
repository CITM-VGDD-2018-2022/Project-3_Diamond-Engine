#ifndef __PARTICLE_EFFECTS_H__
#define __PARTICLE_EFFECTS_H__

struct Particle;

enum class PARTICLE_EFFECT_TYPE : int
{
    NONE = -1,
    //SPAWN,//DEPRECATED?
    AREA_SPAWN,
    FORCE_OVER_LIFETIME,
    RANDOM_MOVE,
    ROTATE_OVER_LIFETIME,
    FADE,
	MAX
};

class ParticleEffect 
{
public:
    ParticleEffect(PARTICLE_EFFECT_TYPE type);
    virtual ~ParticleEffect();


    virtual void Spawn(Particle& particle);
    virtual void Update(Particle& particle, float dt);

#ifndef STANDALONE
    virtual void OnEditor(int emitterIndex);
#endif // !STANDALONE

public:
    PARTICLE_EFFECT_TYPE type;
};

#endif // !__PARTICLE_EFFECTS