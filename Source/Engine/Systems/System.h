#pragma once

#include "Precompiled.h"
#include "Component.h"
#include "Object.h"
#include "BaseSystem.h"

namespace Reflex::Core { class World; }

namespace Reflex::Systems
{
	using namespace Reflex::Core;

#define RequiresComponent( T ) \
	GetWorld().RegisterComponent< T >(); \
	m_requiredComponents.set( T::GetFamily() );

	class System : public BaseSystem
	{
	public:
		friend class Reflex::Core::World;

		// Constructors / Destructors
		System( Reflex::Core::World& world ) : BaseSystem( world ) { }
		virtual ~System() { }

		const std::vector< Reflex::Object >& GetObjects() const { return m_releventObjects; }

		template< typename... Args, typename Func >
		void ForEachObject( Func f ) const
		{
			for( auto& object : m_releventObjects )
				f( ( object.template GetComponent< Args >() )... );
		}

	protected:
		virtual bool ShouldAddObject( const Object& object ) const override 
		{ 
			return ( object.GetComponentFlags() & GetRequiredComponents() ) == GetRequiredComponents(); 
		}

		virtual void AddComponent( const Object& object ) override
		{
			m_releventObjects.push_back( object );
		}

	protected:
		std::vector< Reflex::Object > m_releventObjects;
	};
}