#pragma once

#include "base/tl/ic_array.h"

enum class ERandomEvent
{
	Invalid = -1,
	None = 0,

    /* human */
	Storage,
    Brave,

    /* infected */
	Thief,
	Ashes,

    /* global */
    Disabling,
    Fear,
	Christmas,

	Count
};

constexpr int NB_RANDOMEVENT = static_cast<int>(ERandomEvent::Count);

static constexpr icArray<ERandomEvent, NB_RANDOMEVENT> AllHumanEvents{
	ERandomEvent::Storage,
    ERandomEvent::Brave
};

static constexpr icArray<ERandomEvent, NB_RANDOMEVENT> AllInfectedEvents{
	ERandomEvent::Thief,
	ERandomEvent::Ashes
};

static constexpr icArray<ERandomEvent, NB_RANDOMEVENT> AllGlobalEvents{
	ERandomEvent::None,

    ERandomEvent::Disabling,
    ERandomEvent::Fear,
	ERandomEvent::Christmas
};

constexpr int NB_HUMANEVENT = AllHumanEvents.Size();
constexpr int NB_INFECTEDEVENT = AllInfectedEvents.Size();
constexpr int NB_GLOBALEVENT = AllGlobalEvents.Size();
static_assert(NB_HUMANEVENT + NB_INFECTEDEVENT + NB_GLOBALEVENT == NB_RANDOMEVENT);

inline bool IsHumanEvent(ERandomEvent E)
{
	return AllHumanEvents.Contains(E);
}

inline bool IsInfectedEvent(ERandomEvent E)
{
	return AllInfectedEvents.Contains(E);
}

inline bool IsGlobalEvent(ERandomEvent E)
{
	return AllGlobalEvents.Contains(E);
}

const char *toString(ERandomEvent RandomEvent);
