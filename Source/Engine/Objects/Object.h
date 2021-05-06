#pragma once

#include "Precompiled.h"
#include "BaseObject.h"
#include "Components/Component.h"
#include "Core/World.h"

namespace Reflex::Core
{
	class World;
}

namespace Reflex
{
	class Object : public BaseObject
	{
	public:
		friend class Reflex::Core::World;

		// This constructor is only used to create null / invalid objects
		Object() { }
		Object( const Object& other );
		Object( const BaseObject& base );
		virtual ~Object() { }

		void Destroy();

		// Creates and adds a new component of the template type and returns a handle to it
		template< class T, typename... Args >
		Handle< T > AddComponent( Args&& ... args );

		// Same as above, but it is not an error if the component has already been added
		template< class T, typename... Args >
		Handle< T > TryAddComponent( Args&& ... args );

		// Removes all components
		void RemoveAllComponents();

		// Removes component matching the template type
		template< class T >
		bool RemoveComponent();

		// Checks if this object has a component of template type
		template< class T >
		bool HasComponent() const;

		// Returns a component handle of template type if this object has one
		template< class T >
		Handle< T > GetComponent() const;

		Handle< Reflex::Components::Transform > GetTransform() const;

		//template< class T >
		//std::vector< Handle< T > > GetComponents() const;

		// Copy components from another object to this object
		//template< typename... Args >
		//void CopyComponentsFrom( const Object& other );
		Reflex::ComponentsMask GetComponentFlags() const;

		bool IsFlagSet( const ObjectFlags flag ) const;

		bool IsValid() const;
		explicit operator bool() const;

	protected:
		//template< typename T, typename... Args >
		//void CopyComponentsFromInternal( const Object& other );
		//
		//// Remove the base case where there is 0 argument types (above function calls itself recursively until we reach 0 template arguments)
		//template< typename... Args >
		//typename std::enable_if< sizeof...( Args ) == 0 >::type CopyComponentsFromInternal( const Object& other ) { LOG_INFO( "Copy components complete" ); }

		// Constructor can only be called by World
		Object( Reflex::Core::World& world, const uint32_t index, const uint32_t counter );
	};

	template< class T >
	class Handle
	{
	public:
		Handle() {}
		Handle( const Object& object ) : object( object ) { }
		T* Get() const;
		T* operator->() const { return Get(); }
		T& operator*() const { return *Get(); }
		bool IsValid() const;

		explicit operator bool() const { return IsValid(); }
		bool operator==( const Handle< T >& other ) const;
		bool operator!=( const Handle< T >& other ) const;

		template< class V >
		Handle( const Handle< V >& handle ) = delete;

		Object object;
	};

	// Template definitions
	template< class T, typename... Args >
	Handle< T > Object::AddComponent( Args&& ... args )
	{
		GetWorld().template ObjectAddComponent< T >( *this, std::forward<Args>( args )... );
		return Handle< T >( *this );
	}

	template< class T, typename... Args >
	Handle< T > Object::TryAddComponent( Args&& ... args )
	{
		if( HasComponent< T >() )
			return Handle< T >( *this );
		return AddComponent< T >( std::forward<Args>( args )... );
	}

	template< class T >
	bool Object::RemoveComponent()
	{
		return GetWorld().template ObjectRemoveComponent< T >( *this );
	}

	template< class T >
	bool Object::HasComponent() const
	{
		return GetWorld().template ObjectHasComponent< T >( *this );
	}

	template< class T >
	Handle< T > Object::GetComponent() const
	{
		assert( IsValid() );
		if( !HasComponent< T >() )
			return { };
		return Handle< T >( *this );
	}

	//template< typename... Args >
	//void Object::CopyComponentsFrom( const Object& other )
	//{
	//	CopyComponentsFromInternal< Args... >( other );
	//}
	//
	//template< typename T, typename... Args >
	//void Object::CopyComponentsFromInternal( const Object& other )
	//{
	//	RemoveAllComponents();
	//
	//	auto component = other.GetComponent< T >();
	//
	//	if( component )
	//		AddComponent< T >( *component.Get() );
	//
	//	// Recursively pop arguments off the variadic template args and continue
	//	CopyComponentsFrom< Args... >( other );
	//}

	template< class T >
	T* Handle< T >::Get() const
	{
		assert( object.IsValid() );
		return object.GetWorld().template ObjectGetComponent< T >( object );
	}

	template< class T >
	bool Handle< T >::IsValid() const
	{
		return object.IsValid() && object.HasComponent< T >();
	}

	template< class T >
	bool Handle< T >::operator==( const Handle< T >& other ) const
	{
		return object == other.object;
	}

	template< class T >
	bool Handle< T >::operator!=( const Handle< T >& other ) const
	{
		return !( *this == other );
	}
}

// Hash function for handles allows it to be used with hash maps such as std::unordered_map etc.
MAKE_HASHABLE( Reflex::Object, t.GetIndex(), t.GetCounter() )

namespace std
{
	template< class T >
	struct hash< Reflex::Handle< T > >
	{
		std::size_t operator()( const Reflex::Handle< T >& handle ) const
		{
			return hash< Reflex::Object >()( handle.object ) ^ ( hash< Reflex::ComponentFamily >()( T::GetFamily() ) + 0x9e3779b9 );
		}
	};
}