#pragma once

namespace Reflex::Core
{
	class EventManager;

	// Event object itself
	struct BaseEvent
	{
		virtual ~BaseEvent() {}
	protected:
		static size_t typeCounter;
	};

	template< typename EventType >
	struct Event : BaseEvent
	{
		static size_t GetType()
		{
			static size_t type = BaseEvent::typeCounter++;
			return type;
		}

		Event( const EventType& event ) : event( event ) {}
		const EventType& event;
	};

	class World;

	// Event receiver
	struct EventReceiver
	{
		friend class EventManager;

		EventReceiver() { }
		virtual ~EventReceiver();

	protected:
		EventReceiver( const EventReceiver& other )
			: receiverIndex( std::nullopt )
			, eventManager( other.eventManager )
		{
		}

	protected:
		std::optional< unsigned > receiverIndex;
		EventManager* eventManager = nullptr;
	};

	// Event triggerer
	struct EventTriggerer
	{
		EventTriggerer( EventManager& eventManager );
		EventTriggerer( World& world );
		virtual ~EventTriggerer();

		template< typename EventType, typename ReceiverType >
		void Subscribe( ReceiverType& receiver, void ( ReceiverType::* func )( const EventType& ) );

		template< typename ReceiverType >
		void Unsubscribe( ReceiverType& receiver );

	protected:
		friend class EventManager;

		EventTriggerer( const EventTriggerer& other )
			: triggererIndex( std::nullopt )
			, eventManager( other.eventManager )
		{
		}

		template< typename EventType >
		void Emit( const EventType& event );

	private:
		std::optional< unsigned > triggererIndex;
		EventManager& eventManager;
	};
}