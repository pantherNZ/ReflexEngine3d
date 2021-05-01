#pragma once

#include "Events.h"

#include <memory>
#include <vector>
#include <functional>

namespace Reflex::Core
{
	class EventManager
	{
	public:
		template < class EventType >
		using callType = std::function< void( const EventType& ) >;

		template< typename EventType, typename ReceiverType >
		void Subscribe( ReceiverType& receiver, void ( ReceiverType::*func )( const EventType& ) )
		{	
			SubscribeInternal( nullptr, receiver, func );
		}

		template< typename EventType, typename ReceiverType >
		void Subscribe( EventTriggerer& triggerer, ReceiverType& receiver, void ( ReceiverType::* func )( const EventType& ) )
		{
			SubscribeInternal( &triggerer, receiver, func );
		}

		void Unsubscribe( EventReceiver& receiver )
		{
			UnsubscribeInternal( nullptr, &receiver );
		}

		void Unsubscribe( EventTriggerer& triggerer, EventReceiver& receiver )
		{
			UnsubscribeInternal( &triggerer, &receiver );
		}

		void RemoveTriggerer( EventTriggerer& triggerer )
		{
			UnsubscribeInternal( &triggerer, nullptr );
		}

		template< typename EventType >
		void Emit( EventTriggerer& triggerer, const EventType& event )
		{
			if( !triggerer.triggererIndex )
				triggerer.triggererIndex = nextTriggererIndex++;

			size_t type = Event< EventType >::GetType();
			if( type >= m_subscribers.size() )
				m_subscribers.resize( type + 1 );

			Event< EventType > eventWrapper( event );
			for( auto& receiver : m_subscribers[type] )
				if( !receiver.triggererIndex || receiver.triggererIndex == triggerer.triggererIndex )
					receiver.functor( eventWrapper );
		}

		template< typename EventType >
		struct CallbackWrapper
		{
			CallbackWrapper( callType< EventType > callable ) : m_callable( callable ) {}

			void operator() ( const BaseEvent& event ) 
			{
				m_callable( static_cast< const Event< EventType >& >( event ).event );
			}

			callType< EventType > m_callable;
		};

	private:
		template< typename EventType, typename ReceiverType >
		void SubscribeInternal( EventTriggerer* triggerer, ReceiverType& receiver, void ( ReceiverType::* func )( const EventType& ) )
		{
			static_assert( std::is_convertible<ReceiverType*, EventReceiver*>::value, "To receive events you must inherit from Reflex::EventReceiver" );

			if( triggerer && !triggerer->triggererIndex )
				triggerer->triggererIndex = nextTriggererIndex++;

			const auto callable = callType< EventType >( std::bind( func, &receiver, std::placeholders::_1 ) );
			
			if( !receiver.receiverIndex )
			{
				receiver.receiverIndex = nextReceiverIndex++;
				receiver.eventManager = this;
			}

			size_t type = Event<EventType>::GetType();
			if( type >= m_subscribers.size() )
				m_subscribers.resize( type + 1 );

			const auto triggererIdx = triggerer ? triggerer->triggererIndex : std::nullopt;

			// Already subscribed? Don't subscribe twice
			if( Reflex::ContainsIf( m_subscribers[type], [&]( const ReceiverInstance& instance )
			{
				return instance.receiverIndex == triggererIdx && instance.receiverIndex == *receiver.receiverIndex;
			} ) )
				return;

			m_subscribers[type].push_back( { CallbackWrapper< EventType >( callable ), triggererIdx, *receiver.receiverIndex } );
		}

		void UnsubscribeInternal( EventTriggerer* triggerer, EventReceiver* receiver )
		{
			assert( triggerer || receiver );

			for( auto& eventType : m_subscribers )
			{
				Reflex::EraseIf( eventType, [&]( const ReceiverInstance& instance )
				{
					return ( !triggerer || ( triggerer->triggererIndex && instance.triggererIndex == triggerer->triggererIndex ) ) &&
						( !receiver || ( receiver->receiverIndex && instance.receiverIndex == receiver->receiverIndex ) );
						
				} );
			}
		}

		struct ReceiverInstance
		{
			callType< BaseEvent > functor;
			std::optional< unsigned > triggererIndex;
			unsigned receiverIndex;
		};

		unsigned nextTriggererIndex = 0;
		unsigned nextReceiverIndex = 0;
		std::vector< std::vector< ReceiverInstance > > m_subscribers;
	};

	template< typename EventType, typename ReceiverType >
	void EventTriggerer::Subscribe( ReceiverType& receiver, void ( ReceiverType::* func )( const EventType& ) )
	{
		eventManager.Subscribe( *this, receiver, func );
	}

	template< typename ReceiverType >
	void EventTriggerer::Unsubscribe( ReceiverType& receiver )
	{
		eventManager.Unsubscribe( *this, receiver );
	}

	template< typename EventType >
	void EventTriggerer::Emit( const EventType& event )
	{
		eventManager.Emit( *this, event );
	}
}