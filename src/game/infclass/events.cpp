#include "events.h"

namespace
{

enum RANDOMEVENT
{
	RANDOMEVENT_INVALID = -1,
	RANDOMEVENT_NONE = 0,

	START_HUMANEVENT,
	RANDOMEVENT_STORAGE,
    RANDOMEVENT_BRAVE,
	END_HUMANEVENT,

	START_INFECTEDEVENT,
	RANDOMEVENT_THIEF,
	RANDOMEVENT_ASHES,
	END_INFECTEDEVENT,

	START_GLOBALEVENT,
	RANDOMEVENT_DISABLING,
    RANDOMEVENT_FEAR,
	END_GLOBALEVENT,

	NB_RANDOMEVENT,
	NB_HUMANEVENT = END_HUMANEVENT - START_HUMANEVENT - 1,
	NB_INFECTEDEVENT = END_INFECTEDEVENT - START_INFECTEDEVENT - 1,
	NB_GLOBALEVENT = END_GLOBALEVENT - START_GLOBALEVENT - 1,
};

}

const char *toString(ERandomEvent RandomEvent)
{
	switch (RandomEvent)
	{
	case ERandomEvent::None:
		return "none";

	case ERandomEvent::Storage:
        return "storage";
    case ERandomEvent::Brave:
        return "brave";

    case ERandomEvent::Thief:
        return "thief";
    case ERandomEvent::Ashes:
        return "ashes";

    case ERandomEvent::Disabling:
        return "disabling";
    case ERandomEvent::Fear:
        return "fear";

	case ERandomEvent::Invalid:
	case ERandomEvent::Count:
		break;
	}

	return "unknown";
}