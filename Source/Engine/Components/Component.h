#pragma once

#include "Precompiled.h"
#include "BaseObject.h"

#undef GetObject

namespace Reflex 
{ 
	class Object; 

	template< class T >
	class Handle;

	namespace Core { class World; }
	namespace Systems { class RenderSystem; }
}

namespace Reflex::Components
{
	class Transform;

	// Abstract class for internal use, custom components should inherit from Component below
	class BaseComponent
	{
	public:
		friend class Reflex::Core::World;
		friend class Reflex::Systems::RenderSystem;

		BaseComponent( const Reflex::Object& object );
		Reflex::Object GetObject() const;
		Reflex::Handle< Transform > GetTransform() const;
		Reflex::Core::World& GetWorld() const;

	protected:
		BaseComponent() {}
		BaseComponent( const BaseComponent& other );
		virtual ~BaseComponent() {}
		virtual void OnConstructionComplete() = 0;
		virtual void OnDestructionBegin() = 0;
		static std::string GetComponentName() { assert( false ); }

		// Serialisation
		virtual bool SetValue( const std::string& variable, const std::string& value ) { return false; }
		virtual void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const { }

		// Component rendering
		virtual bool IsRenderComponent() const { return false; }
		virtual void Render( sf::RenderTarget& target, sf::RenderStates states ) const { }

		BaseObject m_object;
		static ComponentFamily s_componentFamilyIdx;
	};

	// Custom components should inherit from this class and pass the custom class as type T
	// RequiresComponents is an optional parameter that defines components that are required for the component to work
	//	When a component is added, it will automatically add all required components if not already added
	template< typename T, typename... RequiresComponents >
	class Component : public BaseComponent
	{
	public:
		friend class Reflex::Core::World;

		typedef Reflex::Handle< T > Handle;
		Handle GetHandle() const { return Handle( GetObject() ); }

		static ComponentFamily GetFamily()
		{
			static ComponentFamily s_family = s_componentFamilyIdx++;
			assert( s_family < MaxComponents );
			return s_family;
		}

		static ComponentsMask GetRequiredComponents()
		{
			ComponentsMask mask;
			unsigned long ul;
			ul = ( ( 1 << RequiresComponents::GetFamily() ) | ... | 0 );
			return ComponentsMask( ul );
		}

		virtual void OnConstructionComplete() override { };
		virtual void OnDestructionBegin() override { }

	protected:
		using BaseComponent::BaseComponent;
	};
}

// Helper macros used for serialisation
#define TrySetValue( name, mem ) \
	if( variable == name ) { \
		mem = Reflex::FromString< decltype( mem ) >( value ); \
		return true; } 

#define GetValue( name, mem ) \
	values.emplace_back( name, Reflex::ToString( mem ) );

#define TryGetValue( name, mem, cond ) \
	if( cond ) \
		values.emplace_back( name, Reflex::ToString( mem ) );
