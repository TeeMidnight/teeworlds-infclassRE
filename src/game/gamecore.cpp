/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "gamecore.h"

#include "collision.h"
#include "teamscore.h"

#include <engine/shared/config.h>

const char *CTuningParams::ms_apNames[] =
{
	#define MACRO_TUNING_PARAM(Name, ScriptName, Value, Description) #ScriptName,
	#include "tuning.h"
	#undef MACRO_TUNING_PARAM
};

bool CTuningParams::Set(int Index, float Value)
{
	if(Index < 0 || Index >= Num())
		return false;
	((CTuneParam *)this)[Index] = Value;
	return true;
}

bool CTuningParams::Get(int Index, float *pValue) const
{
	if(Index < 0 || Index >= Num())
		return false;
	*pValue = (float)((CTuneParam *)this)[Index];
	return true;
}

bool CTuningParams::Set(const char *pName, float Value)
{
	for(int i = 0; i < Num(); i++)
		if(str_comp_nocase(pName, ms_apNames[i]) == 0)
			return Set(i, Value);
	return false;
}

bool CTuningParams::Get(const char *pName, float *pValue) const
{
	for(int i = 0; i < Num(); i++)
		if(str_comp_nocase(pName, ms_apNames[i]) == 0)
			return Get(i, pValue);

	return false;
}

float VelocityRamp(float Value, float Start, float Range, float Curvature)
{
	if(Value < Start)
		return 1.0f;
	return 1.0f / powf(Curvature, (Value - Start) / Range);
}

const float CCharacterCore::PassengerYOffset = 50;

void CCharacterCore::Init(CWorldCore *pWorld, CCollision *pCollision, CTeamsCore *pTeams)
{
	m_pWorld = pWorld;
	m_pCollision = pCollision;

	m_pTeams = pTeams;
	m_Id = -1;

	Reset();
}

void CCharacterCore::Reset()
{
	m_Pos = vec2(0, 0);
	m_Vel = vec2(0, 0);
	m_HookPos = vec2(0, 0);
	m_HookDir = vec2(0, 0);
	m_HookTick = 0;
	m_HookState = HOOK_IDLE;
	SetHookedPlayer(-1);
	m_AttachedPlayers.clear();
	m_Jumped = 0;
	m_JumpedTotal = 0;
	m_Jumps = 2;
	m_TriggeredEvents = 0;

	// DDNet Character
	m_Solo = false;
	m_CollisionDisabled = false;
	m_EndlessJump = false;
	m_Super = false;
	m_FreezeStart = 0;

	// Infclass Character
	m_Passenger = nullptr;
	m_IsPassenger = false;
	m_ProbablyStucked = false;

	// never initialize both to 0
	m_Input.m_TargetX = 0;
	m_Input.m_TargetY = -1;
}

void CCharacterCore::Tick(bool UseInput, CParams* pParams)
{
	bool DoDeferredTick = true;
	m_MoveRestrictions = m_pCollision->GetMoveRestrictions(m_Pos);

	const CTuningParams* pTuningParams = pParams->m_pTuningParams;
	m_TriggeredEvents = 0;

	// get ground state
	bool Grounded = false;
	if(m_pCollision->CheckPoint(m_Pos.x + PhysicalSize() / 2, m_Pos.y + PhysicalSize() / 2 + 5))
		Grounded = true;
	if(m_pCollision->CheckPoint(m_Pos.x - PhysicalSize() / 2, m_Pos.y + PhysicalSize() / 2 + 5))
		Grounded = true;

	// InfClassR taxi mode
	if(m_ProbablyStucked)
	{
		if(m_pCollision->TestBox(m_Pos, PhysicalSizeVec2()))
		{
			m_Pos.y += 1;
		}
		else
		{
			m_ProbablyStucked = false;
		}
	}
	// InfClassR taxi mode end

	vec2 TargetDirection = normalize(vec2(m_Input.m_TargetX, m_Input.m_TargetY));

	m_Vel.y += pTuningParams->m_Gravity;

	float MaxSpeed = Grounded ? pTuningParams->m_GroundControlSpeed : pTuningParams->m_AirControlSpeed;
	float Accel = Grounded ? pTuningParams->m_GroundControlAccel : pTuningParams->m_AirControlAccel;
	float Friction = Grounded ? pTuningParams->m_GroundFriction : pTuningParams->m_AirFriction;

	// handle input
	if(UseInput)
	{
		m_Direction = m_Input.m_Direction;

		// setup angle
		float a = 0;
		if(m_Input.m_TargetX == 0)
			a = atanf((float)m_Input.m_TargetY);
		else
			a = atanf((float)m_Input.m_TargetY/(float)m_Input.m_TargetX);

		if(m_Input.m_TargetX < 0)
			a = a+pi;

		m_Angle = (int)(a*256.0f);

		// handle jump
		if(m_Input.m_Jump)
		{
			if(!(m_Jumped & 1))
			{
				if(Grounded && (!(m_Jumped & 2) || m_Jumps != 0))
				{
					m_TriggeredEvents |= COREEVENT_GROUND_JUMP;
					m_Vel.y = -pTuningParams->m_GroundJumpImpulse;
					if(m_Jumps > 1)
					{
						m_Jumped |= 1;
					}
					else
					{
						m_Jumped |= 3;
					}
					m_JumpedTotal = 0;
				}
				else if(!(m_Jumped & 2))
				{
					m_TriggeredEvents |= COREEVENT_AIR_JUMP;
					m_Vel.y = -pTuningParams->m_AirJumpImpulse;
					m_Jumped |= 3;
					m_JumpedTotal++;
				}
			}
		}
		else
		{
			m_Jumped &= ~1;
		}

		// handle hook
		if(m_Input.m_Hook)
		{
			if(m_HookState == HOOK_IDLE)
			{
				m_HookState = HOOK_FLYING;
				m_HookPos = m_Pos + TargetDirection * PhysicalSize() * 1.5f;
				m_HookDir = TargetDirection;
				SetHookedPlayer(-1);
				m_HookTick = 0;
				m_TriggeredEvents |= COREEVENT_HOOK_LAUNCH;
			}
		}
		else
		{
			SetHookedPlayer(-1);
			m_HookState = HOOK_IDLE;
			m_HookPos = m_Pos;
		}
	}

	// add the speed modification according to players wanted direction
	if(m_Direction < 0)
		m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Vel.x, -Accel);
	if(m_Direction > 0)
		m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Vel.x, Accel);
	if(m_Direction == 0)
		m_Vel.x *= Friction;

	// handle jumping
	// 1 bit = to keep track if a jump has been made on this input (player is holding space bar)
	// 2 bit = to track if all air-jumps have been used up (tee gets dark feet)
	if(Grounded)
	{
		m_Jumped &= ~2;
		m_JumpedTotal = 0;
	}

	// do hook
	if(m_HookState == HOOK_IDLE)
	{
		SetHookedPlayer(-1);
		m_HookState = HOOK_IDLE;
		m_HookPos = m_Pos;
	}
	else if(m_HookState >= HOOK_RETRACT_START && m_HookState < HOOK_RETRACT_END)
	{
		m_HookState++;
	}
	else if(m_HookState == HOOK_RETRACT_END)
	{
		m_TriggeredEvents |= COREEVENT_HOOK_RETRACT;
		m_HookState = HOOK_RETRACTED;
	}
	else if(m_HookState == HOOK_FLYING)
	{
		vec2 NewPos = m_HookPos+m_HookDir*pTuningParams->m_HookFireSpeed;
		if(distance(m_Pos, NewPos) > pTuningParams->m_HookLength)
		{
			m_HookState = HOOK_RETRACT_START;
			NewPos = m_Pos + normalize(NewPos-m_Pos) * pTuningParams->m_HookLength;
		}

		// make sure that the hook doesn't go though the ground
		bool GoingToHitGround = false;
		bool GoingToRetract = false;
		bool GoingThroughTele = false;
		int Hit = m_pCollision->IntersectLineHook(m_HookPos, NewPos, &NewPos, 0);
		if(Hit)
		{
			if(Hit == TILE_NOHOOK)
				GoingToRetract = true;
			else if(Hit == TILE_TELEINHOOK)
				GoingThroughTele = true;
			else
				GoingToHitGround = true;
		}

		// Check against other players first
		if(m_pWorld)
		{
			float Distance = 0.0f;
			for(int i = 0; i < MAX_CLIENTS; i++)
			{
				CCharacterCore *pCharCore = m_pWorld->m_apCharacters[i];
				if (IsRecursePassenger(pCharCore))
					continue;
				if(!pCharCore || pCharCore == this || (!(m_Super || pCharCore->m_Super) && ((m_Id != -1 && !m_pTeams->CanHook(m_Id, i)) || pCharCore->m_Solo || m_Solo)))
					continue;
				if(m_IsPassenger)
					continue;
				if(m_InLove)
					continue;

				vec2 ClosestPoint;
				if(!closest_point_on_line(m_HookPos, NewPos, pCharCore->m_Pos, ClosestPoint))
					continue;

				if(distance(pCharCore->m_Pos, ClosestPoint) < PhysicalSize() + 2.0f)
				{
					if(m_HookedPlayer == -1 || distance(m_HookPos, pCharCore->m_Pos) < Distance)
					{
						m_TriggeredEvents |= COREEVENT_HOOK_ATTACH_PLAYER;
						m_HookState = HOOK_GRABBED;
						SetHookedPlayer(i);
						Distance = distance(m_HookPos, pCharCore->m_Pos);
					}
				}
			}
		}

		if(m_HookState == HOOK_FLYING)
		{
			// check against ground
			if(GoingToHitGround)
			{
				m_TriggeredEvents |= COREEVENT_HOOK_ATTACH_GROUND;
				m_HookState = HOOK_GRABBED;
			}
			else if(GoingToRetract)
			{
				m_TriggeredEvents |= COREEVENT_HOOK_HIT_NOHOOK;
				m_HookState = HOOK_RETRACT_START;
			}

			m_HookPos = NewPos;
		}
	}

	if(m_HookState == HOOK_GRABBED)
	{
		if(m_HookedPlayer != -1)
		{
			CCharacterCore *pCharCore = m_pWorld->m_apCharacters[m_HookedPlayer];
			if(pCharCore && m_pTeams->CanKeepHook(m_Id, pCharCore->m_Id))
				m_HookPos = pCharCore->m_Pos;
			else
			{
				// release hook
				SetHookedPlayer(-1);
				m_HookState = HOOK_RETRACTED;
				m_HookPos = m_Pos;
			}
		}

		// don't do this hook routine when we are hook to a player
		if(m_HookedPlayer == -1 && distance(m_HookPos, m_Pos) > 46.0f)
		{
			vec2 HookVel = normalize(m_HookPos-m_Pos)*pTuningParams->m_HookDragAccel;
			// the hook as more power to drag you up then down.
			// this makes it easier to get on top of an platform
			if(HookVel.y > 0)
				HookVel.y *= 0.3f;

			// the hook will boost it's power if the player wants to move
			// in that direction. otherwise it will dampen everything abit
			if((HookVel.x < 0 && m_Direction < 0) || (HookVel.x > 0 && m_Direction > 0))
				HookVel.x *= 0.95f;
			else
				HookVel.x *= 0.75f;

			vec2 NewVel = m_Vel+HookVel;

			// check if we are under the legal limit for the hook
			if(length(NewVel) < pTuningParams->m_HookDragSpeed || length(NewVel) < length(m_Vel))
			{
				m_Vel = NewVel; // no problem. apply
			}
		}

		// release hook (max hook time is 1.25)
		m_HookTick++;
		if(m_HookedPlayer != -1 && (m_HookTick > pParams->m_HookGrabTime || !m_pWorld->m_apCharacters[m_HookedPlayer]))
		{
			SetHookedPlayer(-1);
			m_HookState = HOOK_RETRACTED;
			m_HookPos = m_Pos;
		}

		if(m_HookedPlayer < 0 && pParams->m_HookMode == 1 && distance(m_HookPos, m_Pos) > g_Config.m_InfSpiderWebHookLength)
		{
			// release hook
			m_HookState = HOOK_RETRACTED;
			m_HookPos = m_Pos;
		}
	}

	if(DoDeferredTick)
		TickDeferred(pParams);
}

void CCharacterCore::TickDeferred(CParams *pParams)
{
	const CTuningParams* pTuningParams = pParams->m_pTuningParams;

	if(m_pWorld)
	{
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			CCharacterCore *pCharCore = m_pWorld->m_apCharacters[i];
			if(!pCharCore)
				continue;

			//player *p = (player*)ent;
			//if(pCharCore == this) // || !(p->flags&FLAG_ALIVE)

			if(pCharCore == this || (m_Id != -1 && !m_pTeams->CanHook(m_Id, i)))
				continue; // make sure that we don't nudge our self

			if(!(m_Super || pCharCore->m_Super) && (m_Solo || pCharCore->m_Solo))
				continue;

			// handle player <-> player collision
			float Distance = distance(m_Pos, pCharCore->m_Pos);
			if(Distance > 0)
			{
				vec2 Dir = normalize(m_Pos - pCharCore->m_Pos);

				bool CanCollide = (m_Super || pCharCore->m_Super) || (!m_CollisionDisabled && !pCharCore->m_CollisionDisabled && 1); // m_Tuning.m_PlayerCollision);

				if(CanCollide && (m_Id == -1 || m_pTeams->CanCollide(m_Id, i)) && Distance < PhysicalSize() * 1.25f && Distance > 0.0f)
				{
					float a = (PhysicalSize() * 1.45f - Distance);
					float Velocity = 0.5f;

					// make sure that we don't add excess force by checking the
					// direction against the current velocity. if not zero.
					if(length(m_Vel) > 0.0001)
						Velocity = 1 - (dot(normalize(m_Vel), Dir) + 1) / 2;

					m_Vel += Dir * a * (Velocity * 0.75f);
					m_Vel *= 0.85f;
				}

				// handle hook influence
				if(m_HookedPlayer == i)
				{
					if(Distance > PhysicalSize() * 1.50f) // TODO: fix tweakable variable
					{
						float Accel = pTuningParams->m_HookDragAccel * (Distance/pTuningParams->m_HookLength);
						float DragSpeed = pTuningParams->m_HookDragSpeed;

						// add force to the hooked player
						pCharCore->m_Vel.x = SaturatedAdd(-DragSpeed, DragSpeed, pCharCore->m_Vel.x, Accel*Dir.x*1.5f);
						pCharCore->m_Vel.y = SaturatedAdd(-DragSpeed, DragSpeed, pCharCore->m_Vel.y, Accel*Dir.y*1.5f);

						// add a little bit force to the guy who has the grip
						m_Vel.x = SaturatedAdd(-DragSpeed, DragSpeed, m_Vel.x, -Accel*Dir.x*0.25f);
						m_Vel.y = SaturatedAdd(-DragSpeed, DragSpeed, m_Vel.y, -Accel*Dir.y*0.25f);
					}
				}
			}
		}
	}

	// clamp the velocity to something sane
	if(length(m_Vel) > 6000)
		m_Vel = normalize(m_Vel) * 6000;

	UpdateTaxiPassengers();
}

void CCharacterCore::Move(CParams* pParams)
{
	const CTuningParams* pTuningParams = pParams->m_pTuningParams;
	
	float RampValue = VelocityRamp(length(m_Vel)*50, pTuningParams->m_VelrampStart, pTuningParams->m_VelrampRange, pTuningParams->m_VelrampCurvature);

	m_Vel.x = m_Vel.x*RampValue;

	vec2 NewPos = m_Pos;

	vec2 Size = PhysicalSizeVec2();
	int PosDiff = 0;

	if((g_Config.m_InfTaxiCollisions & 2) && m_PassengerNumber)
	{
		int PassengersExtraHeight = m_PassengerNumber * PassengerYOffset;
		PosDiff = PassengersExtraHeight / 2;
		Size.y += PassengersExtraHeight;
		NewPos.y -= PosDiff;
	}

	bool Grounded = false;
	m_pCollision->MoveBox(&NewPos, &m_Vel, Size,
		vec2(pTuningParams->m_GroundElasticityX,
			pTuningParams->m_GroundElasticityY),
		&Grounded);

	if(Grounded)
	{
		m_Jumped &= ~2;
		m_JumpedTotal = 0;
	}

	NewPos.y += PosDiff;

	m_Vel.x = m_Vel.x*(1.0f/RampValue);

	if(m_pWorld && !pTuningParams->m_PlayerCollision)
	{
		// check player collision
		float Distance = distance(m_Pos, NewPos);
		if(Distance > 0)
		{
			int End = Distance + 1;
			vec2 LastPos = m_Pos;
			for(int i = 0; i < End; i++)
			{
				float a = i / Distance;
				vec2 Pos = mix(m_Pos, NewPos, a);
				for(int p = 0; p < MAX_CLIENTS; p++)
				{
					CCharacterCore *pCharCore = m_pWorld->m_apCharacters[p];
					if(!pCharCore || pCharCore == this)
						continue;
					if((!(pCharCore->m_Super || m_Super) && (m_Solo || pCharCore->m_Solo || (m_Id != -1 && !m_pTeams->CanCollide(m_Id, p)))))
						continue;
					float D = distance(Pos, pCharCore->m_Pos);
					if(D < 28.0f && D >= 0.0f)
					{
						if(a > 0.0f)
							m_Pos = LastPos;
						else if(distance(NewPos, pCharCore->m_Pos) > D)
							m_Pos = NewPos;
						return;
					}
				}
				LastPos = Pos;
			}
		}
	}

	m_Pos = NewPos;
}

void CCharacterCore::Write(CNetObj_CharacterCore *pObjCore)
{
	pObjCore->m_X = round_to_int(m_Pos.x);
	pObjCore->m_Y = round_to_int(m_Pos.y);

	pObjCore->m_VelX = round_to_int(m_Vel.x * 256.0f);
	pObjCore->m_VelY = round_to_int(m_Vel.y * 256.0f);
	pObjCore->m_HookState = m_HookState;
	pObjCore->m_HookTick = m_HookTick;
	pObjCore->m_HookX = round_to_int(m_HookPos.x);
	pObjCore->m_HookY = round_to_int(m_HookPos.y);
	pObjCore->m_HookDx = round_to_int(m_HookDir.x * 256.0f);
	pObjCore->m_HookDy = round_to_int(m_HookDir.y * 256.0f);
	pObjCore->m_HookedPlayer = m_HookedPlayer;
	pObjCore->m_Jumped = m_Jumped;
	pObjCore->m_Direction = m_Direction;
	pObjCore->m_Angle = m_Angle;
}

void CCharacterCore::Read(const CNetObj_CharacterCore *pObjCore)
{
	m_Pos.x = pObjCore->m_X;
	m_Pos.y = pObjCore->m_Y;
	m_Vel.x = pObjCore->m_VelX / 256.0f;
	m_Vel.y = pObjCore->m_VelY / 256.0f;
	m_HookState = pObjCore->m_HookState;
	m_HookTick = pObjCore->m_HookTick;
	m_HookPos.x = pObjCore->m_HookX;
	m_HookPos.y = pObjCore->m_HookY;
	m_HookDir.x = pObjCore->m_HookDx / 256.0f;
	m_HookDir.y = pObjCore->m_HookDy / 256.0f;
	SetHookedPlayer(pObjCore->m_HookedPlayer);
	m_Jumped = pObjCore->m_Jumped;
	m_Direction = pObjCore->m_Direction;
	m_Angle = pObjCore->m_Angle;
}

void CCharacterCore::Quantize()
{
	CNetObj_CharacterCore Core;
	Write(&Core);
	Read(&Core);
}

void CCharacterCore::SetHookedPlayer(int HookedPlayer)
{
	if(HookedPlayer != m_HookedPlayer)
	{
		if(m_HookedPlayer != -1 && m_Id != -1 && m_pWorld)
		{
			CCharacterCore *pCharCore = m_pWorld->m_apCharacters[m_HookedPlayer];
			if(pCharCore)
			{
				pCharCore->m_AttachedPlayers.erase(m_Id);
			}
		}
		if(HookedPlayer != -1 && m_Id != -1 && m_pWorld)
		{
			CCharacterCore *pCharCore = m_pWorld->m_apCharacters[HookedPlayer];
			if(pCharCore)
			{
				pCharCore->m_AttachedPlayers.insert(m_Id);
			}
		}
		m_HookedPlayer = HookedPlayer;
	}
}

void CCharacterCore::SetTeamsCore(CTeamsCore *pTeams)
{
	m_pTeams = pTeams;
}

bool CCharacterCore::IsRecursePassenger(CCharacterCore *pMaybePassenger) const
{
	if(m_Passenger)
	{
		if(m_Passenger == pMaybePassenger)
		{
			return true;
		}

		return m_Passenger->IsRecursePassenger(pMaybePassenger);
	}

	return false;
}

void CCharacterCore::TryBecomePassenger(CCharacterCore *pTaxi)
{
	if(pTaxi->HasPassenger())
		return;

	if(IsRecursePassenger(pTaxi))
		return;

	if(m_HookProtected || pTaxi->m_HookProtected)
		return;

	if(g_Config.m_InfTaxiCollisions & 1)
	{
		if(m_pCollision->CheckPoint(pTaxi->m_Pos + vec2(PhysicalSize() / 2, -PassengerYOffset)) || m_pCollision->CheckPoint(pTaxi->m_Pos + vec2(-PhysicalSize() / 2, -PassengerYOffset)))
		{
			return;
		}

		if(m_pCollision->CheckPoint(pTaxi->m_Pos + vec2(PhysicalSize() / 2, -PassengerYOffset / 2)) || m_pCollision->CheckPoint(pTaxi->m_Pos + vec2(-PhysicalSize() / 2, -PassengerYOffset / 2)))
		{
			return;
		}
	}

	pTaxi->SetPassenger(this);
	SetHookedPlayer(-1);
	m_HookState = HOOK_RETRACTED;
	m_HookPos = m_Pos;
}

void CCharacterCore::SetPassenger(CCharacterCore *pPassenger)
{
	if(m_Passenger == pPassenger)
		return;

	if (m_Passenger)
	{
		m_Passenger->m_IsPassenger = false;
		m_Passenger->m_ProbablyStucked = true;
	}
	m_Passenger = pPassenger;
	if (pPassenger)
	{
		m_Passenger->m_IsPassenger = true;
	}
}

void CCharacterCore::UpdateTaxiPassengers()
{
	// InfClassR taxi mode, todo: cleanup & move out from core
	if(m_Passenger && ((m_Passenger->m_Input.m_Jump > 0) || (!m_Passenger->m_IsPassenger)))
	{
		SetPassenger(nullptr);
	}

	m_PassengerNumber = 0;

	if(m_IsPassenger)
	{
		// Do nothing
	}
	else
	{
		int PassengerNumber = 0;
		CCharacterCore *pPassenger = m_Passenger;
		while(pPassenger)
		{
			++PassengerNumber;

			pPassenger->m_Vel = m_Vel;
			if(abs(pPassenger->m_Vel.y) <= 1.0f)
				pPassenger->m_Vel.y = 0.0f;

			pPassenger->m_Pos.x = m_Pos.x;
			pPassenger->m_Pos.y = m_Pos.y - PassengerYOffset * PassengerNumber;

			pPassenger = pPassenger->m_Passenger;
		}

		m_PassengerNumber = PassengerNumber;
	}
}
