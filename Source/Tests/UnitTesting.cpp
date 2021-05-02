#pragma once

#include "../ReflexEngine/Include.h"
#include "UnitTesting.h"

using namespace Reflex;

class TestState;

int main()
{
	Core::Engine engine( false );
	engine.RegisterState< TestState >( true );
	engine.Run();

	return 0;
}

class TestState : public Core::State, public UnitTesting
{
public:
	TestState( Core::StateManager& stateManager )
		: State( stateManager )
	{
		RegisterSection( "---- Reflex Utility functions -------" );
		RegisterTest( std::bind( Reflex::IsDefault< sf::Vector2f >, sf::Vector2f() ), true, "Reflex::IsDefault with sf::Vector2f" );
		RegisterTest( std::bind( Reflex::IsDefault< sf::Vector2f >, sf::Vector2f( 1.0f, 0.0f ) ), false, "Reflex::IsDefault with sf::Vector2f non zero" );
		RegisterTest( std::bind( Reflex::IsDefault< sf::Color >, sf::Color() ), true, "Reflex::IsDefault with sf::Vector2f non zero" );
		RegisterTest( std::bind( Reflex::IsDefault< sf::Color >, sf::Color( 1, 1, 1, 255 ) ), false, "Reflex::IsDefault with sf::Vector2f non zero" );

		//RegisterSection( "---- Reflex Create Object -------" );

		//const auto test = CreateObject( "Data/Objects/Test.ro" );
		//auto test = CreateObject( sf::Vector2f( 0.0f, 0.0 ), 1.0f );
		//test.AddComponent< Reflex::Components::Camera >();
		//CreateROFile( "test", test );

		RegisterSection( "---- Reflex Event System -------" );
		RegisterTest( std::bind( &TestState::TestEventGeneric, this ), true, "Testing Subscribe / Emit with a generic event by transfering an int value through an event" );
		RegisterTest( std::bind( &TestState::TestEventSpecific, this ), true, "Testing Subscribe / Emit on a specific target object (test we get the callback from the target" );
		RegisterTest( std::bind( &TestState::TestEventSpecific2, this ), true, "Testing Subscribe / Emit on a specific target object (ensure we don't get callbacks from other objects)" );
		RegisterTest( std::bind( &TestState::TestEventScopeSafety, this ), true, "Test automatic unsubscribing" );
		RegisterTest( std::bind( &TestState::TestEventsMulti, this ), true, "Test multiple subscribing (different objects)" );
		RegisterTest( std::bind( &TestState::TestEventsRenderSystem, this ), true, "Test the first real usage of the event system (Render System updating object render index when it changes)" );

		Run();
	}

protected:
	struct TestEvent{ int test = 0; };
	struct TestEvent2{ int test = 0; };

	struct SpecificEventTriggerer : public Reflex::Core::EventTriggerer
	{
		using Reflex::Core::EventTriggerer::EventTriggerer;

		void Emit( Reflex::Core::EventManager& events, const int value )
		{
			events.Emit( *this, TestEvent{ value } );
		}
	};

	struct SpecificEventReceiver : public Reflex::Core::EventReceiver
	{
		void OnTestEvent( const TestEvent& t )
		{
			value = t.test;
		}

		int value = 0;
	};

	bool TestEventGeneric()
	{
		SpecificEventTriggerer triggerer( GetWorld() );
		SpecificEventReceiver receiver;

		GetWorld().GetEventManager().Subscribe< TestEvent >( receiver, &SpecificEventReceiver::OnTestEvent );
		triggerer.Emit( GetWorld().GetEventManager(), 12346 );
		return receiver.value == 12346;
	}

	bool TestEventSpecific()
	{
		SpecificEventTriggerer triggerer( GetWorld() );
		SpecificEventReceiver receiver;

		GetWorld().GetEventManager().Subscribe< TestEvent >( triggerer, receiver, &SpecificEventReceiver::OnTestEvent );
		triggerer.Emit( GetWorld().GetEventManager(), 12346 );
		return receiver.value == 12346;
	}

	bool TestEventSpecific2()
	{
		SpecificEventTriggerer triggerer( GetWorld() );
		SpecificEventTriggerer triggerer2( GetWorld() );
		SpecificEventReceiver receiver;

		GetWorld().GetEventManager().Subscribe< TestEvent >( triggerer, receiver, &SpecificEventReceiver::OnTestEvent );
		triggerer2.Emit( GetWorld().GetEventManager(), 3455 );
		return receiver.value == 0;
	}

	struct ScopeSafeCheck : public Reflex::Core::EventReceiver
	{
		void OnTestEvent( const TestEvent& t )
		{
			Reflex::SetConsoleTextAttributes a( FOREGROUND_RED );
			std::cout << "\nERROR: This should have unsubscribed!";
		}
	};

	// This test would just crash if it failed
	bool TestEventScopeSafety()
	{
		{
			ScopeSafeCheck scopedReceiver;
			GetWorld().GetEventManager().Subscribe< TestEvent >( scopedReceiver, &ScopeSafeCheck::OnTestEvent );
		}

		return true;
	}

	bool TestEventsMulti()
	{
		SpecificEventTriggerer triggerer( GetWorld() );
		SpecificEventTriggerer triggerer2( GetWorld() );
		SpecificEventReceiver receiver;

		GetWorld().GetEventManager().Subscribe< TestEvent >( triggerer, receiver, &SpecificEventReceiver::OnTestEvent );
		GetWorld().GetEventManager().Subscribe< TestEvent >( triggerer2, receiver, &SpecificEventReceiver::OnTestEvent );
		triggerer.Emit( GetWorld().GetEventManager(), 4444 );
		const auto value1 = receiver.value;
		triggerer2.Emit( GetWorld().GetEventManager(), 234234 );
		return value1 == 4444 && receiver.value == 234234;
	}

	bool TestEventsRenderSystem()
	{
		auto object = GetWorld().CreateObject();
		object.AddComponent< Reflex::Components::CircleShape >( 5.0f );

		auto object2 = GetWorld().CreateObject();
		object2.AddComponent< Reflex::Components::CircleShape >( 5.0f );

		const auto* render = GetWorld().GetSystem< Reflex::Systems::RenderSystem >();

		const auto startOrdering = render->GetObjects()[0] == object && render->GetObjects()[1] == object2;
		object.GetTransform()->SetZOrder( 100 );
		const auto newOrdering = render->GetObjects()[0] == object2 && render->GetObjects()[1] == object;

		return startOrdering && newOrdering;
	}
};
