#ifndef GAME_SERVER_ENTITIES_MEDIC_LASER_H
#define GAME_SERVER_ENTITIES_MEDIC_LASER_H

#include "infc-laser.h"

class CMedicLaser : public CInfClassLaser
{
public:
	CMedicLaser(CGameContext *pGameContext, vec2 Pos, vec2 Direction, float StartEnergy, int Owner);

protected:
	bool HitCharacter(vec2 From, vec2 To) final;

};

#endif // GAME_SERVER_ENTITIES_MEDIC_LASER_H
