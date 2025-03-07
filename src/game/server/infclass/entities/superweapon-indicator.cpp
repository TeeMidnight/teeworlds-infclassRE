/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/server/gamecontext.h>
#include <engine/shared/config.h>
#include "superweapon-indicator.h"

#include <game/server/infclass/classes/humans/human.h>
#include <game/server/infclass/entities/infccharacter.h>
#include <game/server/infclass/infcgamecontroller.h>
#include <game/server/infclass/infcplayer.h>

CSuperWeaponIndicator::CSuperWeaponIndicator(CGameContext *pGameContext, vec2 Pos, int Owner)
	: CInfCEntity(pGameContext, CGameWorld::ENTTYPE_SUPERWEAPON_INDICATOR, Pos, Owner)
{
	GameWorld()->InsertEntity(this);
	m_Radius = 40.0f;
	m_StartTick = Server()->Tick();
	m_OwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_warmUpCounter = Server()->TickSpeed()*3;
	m_IsWarmingUp = true;
	
	m_IDs.set_size(3);
	for(int i=0; i<m_IDs.size(); i++)
	{
		m_IDs[i] = Server()->SnapNewID();
	}
}

CSuperWeaponIndicator::~CSuperWeaponIndicator()
{
	for(int i=0; i<m_IDs.size(); i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CSuperWeaponIndicator::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	const CInfClassPlayer *pPlayer = GameController()->GetPlayer(SnappingClient);
	const bool AntiPing = pPlayer && pPlayer->GetAntiPingEnabled();

	if(AntiPing)
		return;

	float time = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();
	float angle = fmodf(time*pi/2, 2.0f*pi);
	
	for(int i=0; i<m_IDs.size(); i++)
	{	
		float shiftedAngle = angle + 2.0*pi*static_cast<float>(i)/static_cast<float>(m_IDs.size());
		vec2 ParticlePos = m_Pos + vec2(cos(shiftedAngle), sin(shiftedAngle)) * m_Radius;

		GameController()->SendHammerDot(ParticlePos, m_IDs[i]);
	}
}

void CSuperWeaponIndicator::Tick()
{
	if(IsMarkedForDestroy())
		return;
	CInfClassCharacter *pOwnerChar = GetOwnerCharacter();
	CInfClassHuman *pHuman = pOwnerChar ? CInfClassHuman::GetInstance(pOwnerChar->GetPlayer()) : nullptr;

	if(!pHuman)
		return;

	//refresh indicator position
	m_Pos = m_OwnerChar->Core()->m_Pos;
	
	if (m_IsWarmingUp) 
	{
		if ( m_warmUpCounter > 0 ) 
		{
			m_warmUpCounter--;
		} else {
			m_IsWarmingUp = false;
			pHuman->GiveWhiteHole();
		}
	} 
	else 	
	{
		if(!pOwnerChar->HasSuperWeaponIndicator())
		{
			GameWorld()->DestroyEntity(this);
		} 
	}
}
