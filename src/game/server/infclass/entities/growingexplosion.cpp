/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include "growingexplosion.h"

#include <engine/shared/config.h>
#include <game/server/gamecontext.h>
#include <game/server/infclass/classes/infcplayerclass.h>
#include <game/infclass/damage_type.h>
#include <game/server/infclass/infcgamecontroller.h>

#include "infccharacter.h"

CGrowingExplosion::CGrowingExplosion(CGameContext *pGameContext, vec2 Pos, vec2 Dir, int Owner, int Radius, GROWING_EXPLOSION_EFFECT ExplosionEffect) :
	CGrowingExplosion(pGameContext, Pos, Dir, Owner, Radius, EDamageType::NO_DAMAGE)
{
	m_ExplosionEffect = ExplosionEffect;
}

CGrowingExplosion::CGrowingExplosion(CGameContext *pGameContext, vec2 Pos, vec2 Dir, int Owner, int Radius, EDamageType DamageType) :
	CInfCEntity(pGameContext, CGameWorld::ENTTYPE_GROWINGEXPLOSION, Pos, Owner)
{
	m_DamageType = DamageType;
	m_TriggeredByCID = Owner;
	CInfClassGameController::DamageTypeToWeapon(DamageType, &m_TakeDamageMode);

	switch(DamageType)
	{
		case EDamageType::STUNNING_GRENADE:
			m_ExplosionEffect = GROWING_EXPLOSION_EFFECT::FREEZE_INFECTED;
			break;
		case EDamageType::MERCENARY_GRENADE:
			m_ExplosionEffect = GROWING_EXPLOSION_EFFECT::POISON_INFECTED;
			break;
		case EDamageType::MERCENARY_BOMB:
			m_ExplosionEffect = GROWING_EXPLOSION_EFFECT::BOOM_INFECTED;
			break;
		case EDamageType::SCIENTIST_LASER:
			m_ExplosionEffect = GROWING_EXPLOSION_EFFECT::BOOM_INFECTED;
			break;
		case EDamageType::SCIENTIST_MINE:
			m_ExplosionEffect = GROWING_EXPLOSION_EFFECT::ELECTRIC_INFECTED;
			break;
		case EDamageType::WHITE_HOLE:
			m_ExplosionEffect = GROWING_EXPLOSION_EFFECT::BOOM_INFECTED;
			break;
		default:
			break;
	}

	m_MaxGrowing = Radius;
	m_GrowingMap_Length = (2*m_MaxGrowing+1);
	m_GrowingMap_Size = (m_GrowingMap_Length * m_GrowingMap_Length);

	m_pGrowingMap.resize(m_GrowingMap_Size);
	m_pGrowingMapVec.resize(m_GrowingMap_Size);

	m_StartTick = Server()->Tick();
	
	mem_zero(m_Hit, sizeof(m_Hit));

	GameWorld()->InsertEntity(this);	
	
	vec2 explosionTile = vec2(16.0f, 16.0f) + vec2(
		static_cast<float>(static_cast<int>(round(m_Pos.x))/32)*32.0,
		static_cast<float>(static_cast<int>(round(m_Pos.y))/32)*32.0);
	
	//Check is the tile is occuped, and if the direction is valide
	if(GameServer()->Collision()->CheckPoint(explosionTile) && length(Dir) <= 1.1)
	{
		m_SeedPos = vec2(16.0f, 16.0f) + vec2(
		static_cast<float>(static_cast<int>(round(m_Pos.x + 32.0f*Dir.x))/32)*32.0,
		static_cast<float>(static_cast<int>(round(m_Pos.y + 32.0f*Dir.y))/32)*32.0);
	}
	else
	{
		m_SeedPos = explosionTile;
	}
	
	m_SeedX = static_cast<int>(round(m_SeedPos.x))/32;
	m_SeedY = static_cast<int>(round(m_SeedPos.y))/32;
	
	for(int j=0; j<m_GrowingMap_Length; j++)
	{
		for(int i=0; i<m_GrowingMap_Length; i++)
		{
			vec2 Tile = m_SeedPos + vec2(32.0f*(i-m_MaxGrowing), 32.0f*(j-m_MaxGrowing));
			if(GameServer()->Collision()->CheckPoint(Tile) || distance(Tile, m_SeedPos) > m_MaxGrowing*32.0f)
			{
				m_pGrowingMap[j*m_GrowingMap_Length+i] = -2;
			}
			else
			{
				m_pGrowingMap[j*m_GrowingMap_Length+i] = -1;
			}
			
			m_pGrowingMapVec[j*m_GrowingMap_Length+i] = vec2(0.0f, 0.0f);
		}
	}
	
	m_pGrowingMap[m_MaxGrowing*m_GrowingMap_Length+m_MaxGrowing] = Server()->Tick();
	
	switch(m_ExplosionEffect)
	{
	case GROWING_EXPLOSION_EFFECT::FREEZE_INFECTED:
		if(random_prob(0.1f))
		{
			GameServer()->CreateHammerHit(m_SeedPos);
		}
		break;
	case GROWING_EXPLOSION_EFFECT::POISON_INFECTED:
		if(random_prob(0.1f))
		{
			GameServer()->CreateDeath(m_SeedPos, m_Owner);
		}
		break;
	case GROWING_EXPLOSION_EFFECT::ELECTRIC_INFECTED:
	{
		//~ GameServer()->CreateHammerHit(m_SeedPos);

		vec2 EndPoint = m_SeedPos + vec2(-16.0f + random_float()*32.0f, -16.0f + random_float()*32.0f);
		m_pGrowingMapVec[m_MaxGrowing*m_GrowingMap_Length+m_MaxGrowing] = EndPoint;
	}
		break;
	default:
		break;
	}
}

void CGrowingExplosion::Tick()
{
	if(m_MarkedForDestroy) return;

	int tick = Server()->Tick();
	//~ if((tick - m_StartTick) > Server()->TickSpeed())
	if((tick - m_StartTick) > m_MaxGrowing)
	{
		GameWorld()->DestroyEntity(this);
		return;
	}
	
	bool NewTile = false;
	
	for(int j=0; j<m_GrowingMap_Length; j++)
	{
		for(int i=0; i<m_GrowingMap_Length; i++)
		{
			if(m_pGrowingMap[j*m_GrowingMap_Length+i] == -1)
			{
				bool FromLeft = (i > 0 && m_pGrowingMap[j*m_GrowingMap_Length+i-1] < tick && m_pGrowingMap[j*m_GrowingMap_Length+i-1] >= 0);
				bool FromRight = (i < m_GrowingMap_Length-1 && m_pGrowingMap[j*m_GrowingMap_Length+i+1] < tick && m_pGrowingMap[j*m_GrowingMap_Length+i+1] >= 0);
				bool FromTop = (j > 0 && m_pGrowingMap[(j-1)*m_GrowingMap_Length+i] < tick && m_pGrowingMap[(j-1)*m_GrowingMap_Length+i] >= 0);
				bool FromBottom = (j < m_GrowingMap_Length-1 && m_pGrowingMap[(j+1)*m_GrowingMap_Length+i] < tick && m_pGrowingMap[(j+1)*m_GrowingMap_Length+i] >= 0);
				
				if(FromLeft || FromRight || FromTop || FromBottom)
				{
					m_pGrowingMap[j*m_GrowingMap_Length+i] = tick;
					NewTile = true;
					m_VisualizedTiles++;
					vec2 TileCenter = m_SeedPos + vec2(32.0f*(i-m_MaxGrowing) - 16.0f + random_float()*32.0f, 32.0f*(j-m_MaxGrowing) - 16.0f + random_float()*32.0f);
					switch(m_ExplosionEffect)
					{
					case GROWING_EXPLOSION_EFFECT::FREEZE_INFECTED:
						if(random_prob(0.1f))
						{
							GameServer()->CreateHammerHit(TileCenter);
						}
						break;
					case GROWING_EXPLOSION_EFFECT::POISON_INFECTED:
						if(random_prob(0.1f))
						{
							GameServer()->CreateDeath(TileCenter, m_Owner);
						}
						break;
					case GROWING_EXPLOSION_EFFECT::HEAL_HUMANS:
						if(m_VisualizedTiles % 8 == 0)
						{
							GameServer()->CreateDeath(TileCenter, m_Owner);
						}
						break;
					case GROWING_EXPLOSION_EFFECT::LOVE_INFECTED:
						if(random_prob(0.2f))
						{
							GameServer()->CreateLoveEvent(TileCenter);
						}
						break;
					case GROWING_EXPLOSION_EFFECT::BOOM_INFECTED:
						if(random_prob(0.2f))
						{
							float DamageFactor = m_DamageType == EDamageType::MERCENARY_BOMB ? 0 : 1;
							GameController()->CreateExplosion(TileCenter, m_Owner, m_DamageType, DamageFactor);
						}
						break;
					case GROWING_EXPLOSION_EFFECT::ELECTRIC_INFECTED:
					{
						vec2 EndPoint = m_SeedPos + vec2(32.0f*(i-m_MaxGrowing) - 16.0f + random_float()*32.0f, 32.0f*(j-m_MaxGrowing) - 16.0f + random_float()*32.0f);
						m_pGrowingMapVec[j*m_GrowingMap_Length+i] = EndPoint;

						int NumPossibleStartPoint = 0;
						vec2 PossibleStartPoint[4];

						if(FromLeft)
						{
							PossibleStartPoint[NumPossibleStartPoint] = m_pGrowingMapVec[j*m_GrowingMap_Length+i-1];
							NumPossibleStartPoint++;
						}
						if(FromRight)
						{
							PossibleStartPoint[NumPossibleStartPoint] = m_pGrowingMapVec[j*m_GrowingMap_Length+i+1];
							NumPossibleStartPoint++;
						}
						if(FromTop)
						{
							PossibleStartPoint[NumPossibleStartPoint] = m_pGrowingMapVec[(j-1)*m_GrowingMap_Length+i];
							NumPossibleStartPoint++;
						}
						if(FromBottom)
						{
							PossibleStartPoint[NumPossibleStartPoint] = m_pGrowingMapVec[(j+1)*m_GrowingMap_Length+i];
							NumPossibleStartPoint++;
						}

						if(NumPossibleStartPoint > 0)
						{
							int randNb = random_int(0, NumPossibleStartPoint-1);
							vec2 StartPoint = PossibleStartPoint[randNb];
							GameServer()->CreateLaserDotEvent(StartPoint, EndPoint, Server()->TickSpeed()/6);
						}

						if(random_prob(0.1f))
						{
							GameServer()->CreateSound(EndPoint, SOUND_LASER_BOUNCE);
						}
					}
						break;
					default:
						break;
					}
				}
			}
		}
	}
	
	if(NewTile)
	{
		switch(m_ExplosionEffect)
		{
		case GROWING_EXPLOSION_EFFECT::POISON_INFECTED:
			if(random_prob(0.1f))
			{
				GameServer()->CreateSound(m_Pos, SOUND_PLAYER_DIE);
			}
			break;
		default:
			break;
		}
	}
	
	// Find other players
	for(TEntityPtr<CInfClassCharacter> p = GameWorld()->FindFirst<CInfClassCharacter>(); p; ++p)
	{
		int tileX = m_MaxGrowing + static_cast<int>(round(p->m_Pos.x))/32 - m_SeedX;
		int tileY = m_MaxGrowing + static_cast<int>(round(p->m_Pos.y))/32 - m_SeedY;
		
		if(tileX < 0 || tileX >= m_GrowingMap_Length || tileY < 0 || tileY >= m_GrowingMap_Length)
			continue;
		
		if(m_Hit[p->GetCID()])
			continue;
		
		int k = tileY*m_GrowingMap_Length+tileX;

		if(m_pGrowingMap[k] >= 0)
		{
			if(tick - m_pGrowingMap[k] < Server()->TickSpeed()/4)
			{
				switch(m_ExplosionEffect)
				{
				case GROWING_EXPLOSION_EFFECT::HEAL_HUMANS:
					if(!p->IsHuman())
					{
						continue;
					}
					p->GiveArmor(1, GetOwner());
					m_Hit[p->GetCID()] = true;
					break;
				case GROWING_EXPLOSION_EFFECT::BOOM_INFECTED:
				{
					ProcessMercenaryBombHit(p);
					break;
				}
				default:
					break;
				}
			}
		}

		if(p->IsHuman())
			continue;

		if(m_pGrowingMap[k] >= 0)
		{
			if(tick - m_pGrowingMap[k] < Server()->TickSpeed()/4)
			{
				switch(m_ExplosionEffect)
				{
				case GROWING_EXPLOSION_EFFECT::FREEZE_INFECTED:
					p->Freeze(3.0f, m_Owner, FREEZEREASON_FLASH);
					GameServer()->SendEmoticon(p->GetCID(), EMOTICON_QUESTION);
					m_Hit[p->GetCID()] = true;
					break;
				case GROWING_EXPLOSION_EFFECT::POISON_INFECTED:
				{
					int Damage = maximum(Config()->m_InfPoisonDamage, 1);
					const float PoisonDurationSeconds = Config()->m_InfPoisonDuration / 1000.0;
					const float DamageIntervalSeconds = PoisonDurationSeconds / Damage;

					p->Poison(Damage, m_Owner, EDamageType::MERCENARY_GRENADE, DamageIntervalSeconds);
					p->GetClass()->DisableHealing(Config()->m_InfPoisonDuration / 1000.0f, m_Owner, EDamageType::MERCENARY_GRENADE);
				}
					GameServer()->SendEmoticon(p->GetCID(), EMOTICON_DROP);
					m_Hit[p->GetCID()] = true;
					break;
				case GROWING_EXPLOSION_EFFECT::HEAL_HUMANS:
					// empty
					break;
				case GROWING_EXPLOSION_EFFECT::BOOM_INFECTED:
				{
					m_Hit[p->GetCID()] = true;
					break;
				}
				case GROWING_EXPLOSION_EFFECT::LOVE_INFECTED:
				{
					p->LoveEffect(5);
					GameServer()->SendEmoticon(p->GetCID(), EMOTICON_HEARTS);
					m_Hit[p->GetCID()] = true;
					break;
				}
				case GROWING_EXPLOSION_EFFECT::ELECTRIC_INFECTED:
				{
					int Damage = GetActualDamage();
					if(Damage)
					{
						p->TakeDamage(normalize(p->m_Pos - m_SeedPos) * 4.0f, Damage, m_Owner, m_DamageType);
					}
					m_Hit[p->GetCID()] = true;
					break;
				}
				default:
					break;
				}
			}
		}
	}

	// clean slug slime
	if (m_ExplosionEffect == GROWING_EXPLOSION_EFFECT::FREEZE_INFECTED)
	{
		for(TEntityPtr<CEntity> e = GameWorld()->FindFirst(CGameWorld::ENTTYPE_SLUG_SLIME); e; ++e)
		{
			int tileX = m_MaxGrowing + static_cast<int>(round(e->m_Pos.x)) / 32 - m_SeedX;
			int tileY = m_MaxGrowing + static_cast<int>(round(e->m_Pos.y)) / 32 - m_SeedY;

			if(tileX < 0 || tileX >= m_GrowingMap_Length || tileY < 0 || tileY >= m_GrowingMap_Length)
				continue;

			int k = tileY * m_GrowingMap_Length + tileX;
			if(m_pGrowingMap[k] >= 0)
			{
				if(tick - m_pGrowingMap[k] < Server()->TickSpeed() / 4)
				{
					e->Reset();
				}
			}
		}
	}
}

void CGrowingExplosion::TickPaused()
{
	++m_StartTick;
}

void CGrowingExplosion::SetDamage(int Damage)
{
	m_Damage = Damage;
}

int CGrowingExplosion::GetActualDamage()
{
	 if(m_Damage < 0)
		 return 5 + 20.0f * (m_MaxGrowing - minimum(Server()->Tick() - m_StartTick, m_MaxGrowing)) / m_MaxGrowing;

	 // Sci mine victim is typically hit on 2nd tick.
	 // It means 5 + 20 * (6 - 2) / 6 = 5 + 13.333 = 18 dmg
	 return m_Damage;
}

void CGrowingExplosion::SetTriggeredBy(int CID)
{
	 m_TriggeredByCID = CID;
}

void CGrowingExplosion::ProcessMercenaryBombHit(CInfClassCharacter *pCharacter)
{
	float Power = m_MaxGrowing / 16.0; // 0..1
	float InnerRadius = 96.0f;
	float OuterRadius = m_MaxGrowing * 32;
	if(InnerRadius >= OuterRadius)
	{
		InnerRadius = OuterRadius * 0.9;
	}
	bool AffectOwner = true;
	if(m_DamageType == EDamageType::WHITE_HOLE)
		AffectOwner = false;

	if(!AffectOwner && (pCharacter->GetCID() == GetOwner()))
		return;

	if(!Config()->m_InfShockwaveAffectHumans)
	{
		if(pCharacter->GetCID() == GetOwner())
		{
			//owner selfharm
		}
		else if(pCharacter->IsHuman())
		{
			// humans are not affected by force
			return;
		}
	}
	vec2 Diff = pCharacter->m_Pos - GetPos();
	vec2 ForceDir(0,1);
	float l = length(Diff);
	if(l)
		ForceDir = normalize(Diff);

	float Ratio = (l-InnerRadius)/(OuterRadius-InnerRadius);

	l = 1-clamp(Ratio, 0.0f, 1.0f);
	float Dmg = Config()->m_InfMercBombMaxDamage * l * Power;
	int DamageFromCID = GetOwner();
	const vec2 Force = ForceDir * Dmg * 2;

	if(pCharacter->GetCID() == GetOwner())
	{
		Dmg *= 0.5f;
		DamageFromCID = m_TriggeredByCID;
	}

	if(Dmg)
	{
		pCharacter->TakeDamage(Force, Dmg, DamageFromCID, m_DamageType);
	}

	m_Hit[pCharacter->GetCID()] = true;
}
