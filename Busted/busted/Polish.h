/*
	Polish.h
	Created by: Jamie Gault
	Created on 7/27/07

*/

#pragma once

#include "busted.all.h"
#include "../engine/core/stat_manager/stat_manager.h"
#include "../engine/core/Questions/Questions.h"
#include "../engine/core/Sprite.h"

enum ParticleType{ PT_BALLOON, PT_CONFETTI };

//!class for particle based sprites that have velocity
class Particle
{
public:
	Particle(ParticleType t, Sprite& sprite, float vx, float vy, float vz,
			float dw = 0.0f, float dh = 0.0f, float dang = 0.0f );

	void Update(float t);

	Sprite	m_spr;
	float	m_velo[3]; //!<change in position
	float	m_delta_dim[2]; //!<change in scale
	float	m_delta_rot; //!<change in rotation
	ParticleType type;
};

//!class for managing particle systems
class ParticleFeature
{
public:
	ParticleFeature(ParticleType pt);
	~ParticleFeature();

	void Update(float t);
	void Render();

private:

	void LoadBalloonIntro();

	Particle* AddParticle(ParticleType pt, float x, float y, float z, std::string s);
	std::vector<Particle*> m_particles; //all the balloons that are displayed in the group

	void LoadOutro();
};
