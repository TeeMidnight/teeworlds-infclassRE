/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "flyingpoint.h"

#include <engine/server.h>
#include <engine/server/roundstatistics.h>
#include <game/server/infclass/classes/infcplayerclass.h>
#include <game/server/infclass/entities/infccharacter.h>
#include <game/server/infclass/infcgamecontroller.h>

CFlyingPoint::CFlyingPoint(CGameContext *pGameContext, vec2 Pos, int TrackedPlayer, int Points, vec2 InitialVel)
	: CInfCEntity(pGameContext, CGameWorld::ENTTYPE_FLYINGPOINT, Pos)
{
	m_TrackedPlayer = TrackedPlayer;
	m_InitialVel = InitialVel;
	m_Points = Points;
	m_InitialAmount = 1.0f;
	GameWorld()->InsertEntity(this);
}

void CFlyingPoint::Tick()
{
	CInfClassCharacter *OwnerChar = GameController()->GetCharacter(m_TrackedPlayer);
	if(OwnerChar)
	{
		float Dist = distance(m_Pos, OwnerChar->GetPos());
		if(Dist < 24.0f)
		{
			OwnerChar->GetClass()->OnFloatingPointCollected(m_Points);
			GameWorld()->DestroyEntity(this);
		}
		else
		{
			vec2 Dir = normalize(OwnerChar->GetPos() - m_Pos);
			m_Pos += Dir*clamp(Dist, 0.0f, 16.0f) * (1.0f - m_InitialAmount) + m_InitialVel * m_InitialAmount;
			
			m_InitialAmount *= 0.98f;
		}
	}
	else
	{
		MarkForDestroy();
	}
}

void CFlyingPoint::Snap(int SnappingClient)
{
	GameController()->SendHammerDot(GetPos(), m_ID);
}
