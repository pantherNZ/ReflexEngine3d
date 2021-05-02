#include "Precompiled.h"
#include "Object.h"
#include "World.h"
#include "Component.h"
#include "TransformComponent.h"

namespace Reflex
{
	Object::Object( const Object& other )
		: BaseObject( other )
	{ 
	}

	Object::Object( const BaseObject& base )
		: BaseObject( base )
	{
	}

	// Constructor can only be called by World
	Object::Object( Reflex::Core::World& world, const uint32_t index, const uint32_t counter )
		: BaseObject( world, index, counter )
	{ 
	}

	void Object::Destroy()
	{
		GetWorld().DestroyObject( *this );
	}

	void Object::RemoveAllComponents()
	{
		GetWorld().ObjectRemoveAllComponents( *this );
	}

	Reflex::Components::Transform::Handle Object::GetTransform() const
	{
		return GetComponent< Reflex::Components::Transform >();
	}

	Reflex::ComponentsMask Object::GetComponentFlags() const
	{
		return GetWorld().ObjectGetComponentFlags( *this );
	}

	bool Object::IsFlagSet( const ObjectFlags flag ) const
	{
		return GetWorld().IsObjectFlagSet( *this, flag );
	}

	bool Object::IsValid() const
	{
		return m_world && GetWorld().IsValidObject( *this );
	}

	Object::operator bool() const
	{
		return IsValid();
	}
}