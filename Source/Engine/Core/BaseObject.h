#pragma once

#include "Precompiled.h"

namespace Reflex
{
	namespace Core { class World; }

	template< class T >
	class Handle;

	namespace Components { class Transform; }

	enum class ObjectFlags
	{
		Deleted,
		ConstructionComplete,
		NumFlags,
	};

	class BaseObject
	{
	public:
		friend class Reflex::Core::World;

		// This constructor is only used to create null / invalid objects
		BaseObject() { }
		BaseObject( const BaseObject& other )
			: m_world( other.m_world )
			, m_counter( other.m_counter )
			, m_index( other.m_index )
		{ }

		virtual ~BaseObject() { }

		std::uint32_t GetIndex() const { return m_index; }
		std::uint32_t GetCounter() const { return m_counter; }
		Reflex::Core::World& GetWorld() const { assert( m_world ); return *m_world; }

		bool operator==( const BaseObject& other ) const
		{
			return GetIndex() == other.GetIndex() && GetCounter() == other.GetCounter();
		}

		bool operator!=( const BaseObject& other ) const
		{
			return !( *this == other );
		}

	protected:
		// Constructor can only be called by World
		BaseObject( Reflex::Core::World& world, const uint32_t index, const uint32_t counter ) 
			: m_world( &world )
			, m_index( index )
			, m_counter( counter )
		{
		}

	protected:
		std::uint32_t m_index = -1;
		std::uint32_t m_counter = -1;
		Reflex::Core::World* m_world = nullptr;
	};
}

// Hash function for handles allows it to be used with hash maps such as std::unordered_map etc.
MAKE_HASHABLE( Reflex::BaseObject, t.GetIndex(), t.GetCounter() )