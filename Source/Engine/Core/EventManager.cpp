#include "Precompiled.h"
#include "EventManager.h"
#include "World.h"

namespace Reflex::Core
{
	size_t BaseEvent::typeCounter = 0;

	EventTriggerer::EventTriggerer( EventManager& eventManager )
		: eventManager( eventManager )
	{
	}

	EventTriggerer::EventTriggerer( World& world )
		: eventManager( world.GetEventManager() )
	{
	}

	EventReceiver::~EventReceiver()
	{
		if( eventManager )
			eventManager->Unsubscribe( *this );
	}

	EventTriggerer::~EventTriggerer()
	{
		eventManager.RemoveTriggerer( *this );
	}
}