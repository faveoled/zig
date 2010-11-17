#ifndef BEAM_H
#define BEAM_H

#include "dudes.h"


class Beam : public Dude
{
public:
	struct Params
	{
		Params() :
			warmuptime( 2.0f ),
			ontime( 3.0f ),
			width( 32.0f ),
			length( 400.0f )
		{}
		float warmuptime;
		float ontime;
		float width;
		float length;
	};

	Beam( Agent& owner,
		vec2 const& relpos,
		float relheading,
		Params const* params=0 );
	~Beam();
	virtual void Draw();
	virtual void Tick();
	virtual void Respawn();
	virtual void OnHitPlayer( Player& player );
private:
	Agent& m_Owner;
	vec2 m_RelPos;
	float m_RelHeading;

	enum { WARMUP, ON } m_State;
	float m_Timer;
	Params m_Params;
};

#endif // BEAM_H


