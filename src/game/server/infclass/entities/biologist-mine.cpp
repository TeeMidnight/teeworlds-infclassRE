/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/server/gamecontext.h>
#include <engine/shared/config.h>

#include "biologist-laser.h"
#include "biologist-mine.h"
#include "infccharacter.h"

int CBiologistMine::EntityId = CGameWorld::ENTTYPE_BIOLOGIST_MINE;

CBiologistMine::CBiologistMine(CGameContext *pGameContext, vec2 Pos, vec2 EndPos, int Owner, int Lasers, int Damage, int Vertices) :
	CPlacedObject(pGameContext, EntityId, Pos, Owner),
	m_Vertices(minimum<int>(Vertices, NUM_SIDE)),
	m_Lasers(Lasers),
	m_PerLaserDamage(Damage)
{
	m_InfClassObjectType = INFCLASS_OBJECT_TYPE_BIOLOGIST_MINE;
	m_EndPos = EndPos;
	GameWorld()->InsertEntity(this);
	
	for(int i=0; i<NUM_IDS; i++)
	{
		m_IDs[i] = Server()->SnapNewID();
	}
}

CBiologistMine::~CBiologistMine()
{
	for(int i=0; i<NUM_IDS; i++)
	{
		Server()->SnapFreeID(m_IDs[i]);
	}
}

void CBiologistMine::Explode()
{
	float AngleStep = 2.0f * pi / m_Lasers;
	float RandomShift = random_float() * 2.0f * pi;
	for(int i = 0; i < m_Lasers; i++)
	{
		new CBiologistLaser(GameServer(), m_Pos, direction(RandomShift + AngleStep * i), m_Owner, m_PerLaserDamage);
	}
	
	GameWorld()->DestroyEntity(this);
}

void CBiologistMine::Snap(int SnappingClient)
{
	if(!DoSnapForClient(SnappingClient))
		return;

	if(Server()->GetClientInfclassVersion(SnappingClient))
	{
		CNetObj_InfClassObject *pInfClassObject = SnapInfClassObject();
		if(!pInfClassObject)
			return;
	}

	float AngleStep = 2.0f * pi / m_Vertices;
	float Radius = 32.0f;

	int SnappingClientVersion = GameServer()->GetClientVersion(SnappingClient);
	CSnapContext Context(SnappingClientVersion);

	for(int i = 0; i < m_Vertices; i++)
	{
		vec2 VertexPos = m_Pos + direction(AngleStep * i) * Radius;
		GameServer()->SnapLaserObject(Context, m_IDs[i], VertexPos, m_Pos, Server()->Tick() - 4, GetOwner());
	}

	GameServer()->SnapLaserObject(Context, m_IDs[m_Vertices], m_EndPos, m_Pos, Server()->Tick() - 4, GetOwner());
}

void CBiologistMine::Tick()
{
	if(m_MarkedForDestroy) return;
	
	
	// Find other players
	for(CInfClassCharacter *p = (CInfClassCharacter*) GameWorld()->FindFirst(CGameWorld::ENTTYPE_CHARACTER); p; p = (CInfClassCharacter *)p->TypeNext())
	{
		if(p->IsHuman()) continue;
		if(!p->CanDie()) continue;

		vec2 IntersectPos;
		if(!closest_point_on_line(m_Pos, m_EndPos, p->m_Pos, IntersectPos))
			continue;

		float Len = distance(p->m_Pos, IntersectPos);
		if(Len < p->m_ProximityRadius)
		{
			Explode();
			break;
		}
	}
}
