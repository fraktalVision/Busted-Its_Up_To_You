/*
	Polish.cpp
	Created by: Jamie Gault
	Created on 7/27/07

*/




#include "busted.all.h"
#include "Polish.h"



extern Texture_Manager* pTexture_man;



Particle::Particle(ParticleType t, Sprite& sprite, float vx, float vy, float vz, float dw, float dh, float dang): m_spr(sprite), type(t)
{
	m_velo[0] = vx;
	m_velo[1] = vy;
	m_velo[2] = vz;

	m_delta_dim[0] = dw;
	m_delta_dim[1] = dh;

	m_delta_rot = dang;

}

void Particle::Update(float t)
{
	m_spr.pos.x += m_velo[0]*t;
	m_spr.pos.y += m_velo[1]*t;
	m_spr.pos.z += m_velo[2]*t;
	

	if( m_spr.GetHeight() > 0.0f )
		m_spr.SetHeight(m_delta_dim[0]*t + m_spr.GetHeight() );

	if( m_spr.GetWidth() > 0.0f )
		m_spr.SetWidth(m_delta_dim[1]*t + m_spr.GetWidth() );

	m_spr.rot_angle += m_delta_rot*t;
}




ParticleFeature::ParticleFeature(ParticleType pt)
{

	switch(pt)
	{
	case PT_CONFETTI:
		LoadOutro();
		break;
	case PT_BALLOON:
		LoadBalloonIntro();
		break;
	}
	

}

ParticleFeature::~ParticleFeature()
{
	for( std::vector<Particle*>::iterator spr = m_particles.begin(); spr!= m_particles.end(); ++spr)
	{
		delete(*spr);
	}
}


void ParticleFeature::Update(float t)
{
	for( std::vector<Particle*>::iterator spr = m_particles.begin(); spr!= m_particles.end(); ++spr)
	{
		(*spr)->Update(t);

		switch((*spr)->type)
		{
		case PT_CONFETTI:

			//have the confetti reset when it's fallen out of view
			if( (*spr)->m_spr.pos.y < 0.0f )
			{
				(*spr)->m_spr.pos.y += DATA("CNF_MAX_Y");

				(*spr)->m_spr.SetWidth(DATA("CNF_WID"));
				(*spr)->m_spr.SetHeight(DATA("CNF_HT"));

			}
			break;

		}
	}
}


void ParticleFeature::Render()
{
	//render all the balloons
	for( std::vector<Particle*>::iterator spr = m_particles.begin(); spr!= m_particles.end(); ++spr)
	{
		(*spr)->m_spr.Render();
	}
}

void ParticleFeature::LoadBalloonIntro()
{
	//max balloons in a circular wheel set
	float maxballoons = 15.0f;

	//add one group of balloons
	for(float i = 0.0f; i < maxballoons; ++i)
	{
		std::string s;

		//alternate through balloon colours
		switch( ((int)i)%3)
		{
		default:
		case 0:	s = std::string("balloon1");	break;
		case 1:	s = std::string("balloon2");	break;
		case 2:	s = std::string("balloon3");	break;
		}

		float x = sin(i/maxballoons*2.0f*3.14159f);
		float z = cos(i/maxballoons*2.0f*3.14159f);
		

		Particle* p = AddParticle(PT_BALLOON, 20.0f*x, DATA("BLN1_POS_Y"), 20.0f*z, s );
		

		p = AddParticle(PT_BALLOON,10.0f*x, DATA("BLN1_POS_Y"), 10.0f*z, s );
		p->m_velo[0] = x*DATA("BLN_VELO_X");
		p->m_velo[2] = z*DATA("BLN_VELO_Z");

		 p = AddParticle(PT_BALLOON,2.0f*x, DATA("BLN1_POS_Y")+20.0f, 2.0f*z, s );
		p->m_velo[0] = x*DATA("BLN_VELO_X");
		p->m_velo[2] = z*DATA("BLN_VELO_Z");
	}

	


	//This is the ballon that is to display the message 'let's play' as the intro plays
	Particle* p = AddParticle(PT_BALLOON,DATA("BLN1_POS_X"), DATA("BLN1_POS_Y"), DATA("BLN1_POS_Z"), std::string("LPballoon") );
	p->m_velo[1] = 0.5f;
	//p->m_spr.rot_angle = 180.0f;

	//set the balloon to gradually increase in scale
	p->m_delta_dim[0] = DATA("BLN1_SCALE");
	p->m_delta_dim[1] = DATA("BLN1_SCALE");


	AddParticle(PT_BALLOON, DATA("BLN2_POS_X"), DATA("BLN2_POS_Y"), DATA("BLN2_POS_Z"), std::string("balloon2") );
	AddParticle(PT_BALLOON, DATA("BLN3_POS_X"), DATA("BLN3_POS_Y"), DATA("BLN3_POS_Z"), std::string("balloon3") );


	AddParticle(PT_BALLOON, -DATA("BLN2_POS_X"), DATA("BLN2_POS_Y"), -DATA("BLN2_POS_Z"), std::string("balloon2") );
	AddParticle(PT_BALLOON, -DATA("BLN3_POS_X"), DATA("BLN3_POS_Y"), -DATA("BLN3_POS_Z"), std::string("balloon3") );
}

void ParticleFeature::LoadOutro()
{
	//max balloons in a circular wheel set
	float maxparticles = DATA("CNF_NUM");

		//add one group of balloons
	for(float i = 0.0f; i < maxparticles; ++i)
	{
		std::string s;

		//alternate through balloon colours
		switch( ((int)i)%3)
		{
		default:
		case 0:	s = std::string("confetti1");	break;
		case 1:	s = std::string("confetti2");	break;
		case 2:	s = std::string("confetti3");	break;
		}

		//get the coordinate about the circle
		float x = sin(i/maxparticles*2.0f*3.14159f);
		float z = cos(i/maxparticles*2.0f*3.14159f);
		

		//generate 4 rings of particles that start at different heights
		Particle* p = AddParticle(PT_CONFETTI, 60.0f*x, DATA("CNF_MAX_Y")+50.0f+20.0f*x, 60.0f*z, s );
		p = AddParticle(PT_CONFETTI,70.0f*x, DATA("CNF_MAX_Y")+3.0f*x, 70.0f*z, s );
		p = AddParticle(PT_CONFETTI,50.0f*x, DATA("CNF_MAX_Y")-10.0f*z+20.0f, 50.0f*z, s );
		p = AddParticle(PT_CONFETTI,25.0f*x, DATA("CNF_MAX_Y")+30.0f+20.0f*z, 25.0f*z, s );
		p = AddParticle(PT_CONFETTI,40.0f*x, DATA("CNF_MAX_Y")+10.0f*z, 30.0f*z, s );
	
	}

}


Particle* ParticleFeature::AddParticle(ParticleType pt, float x, float y, float z, std::string s)
{
	Particle* spr;

	//generate different particle types
	switch( pt )
	{
	case PT_BALLOON:
		spr = new Particle(pt, Sprite(pTexture_man->GetTexture(s), x, y, z, DATA("BLN_WID"), DATA("BLN_HT")), 
						0.0f, DATA("BLN_VELO_Y"), 0.0f, 0.0f, 0.0f );
		break;
	case PT_CONFETTI:
		spr = new Particle(pt, Sprite(pTexture_man->GetTexture(s), x, y, z, DATA("CNF_WID"), DATA("CNF_HT")), 
			DATA("CNF_VELO_X"), DATA("CNF_VELO_Y"), DATA("CNF_VELO_Z"), DATA("CNF_DEL_W"), DATA("CNF_DEL_H"), DATA("CNF_DEL_ANG") );
		break;
	}

	m_particles.push_back(spr);
	return spr;
}


