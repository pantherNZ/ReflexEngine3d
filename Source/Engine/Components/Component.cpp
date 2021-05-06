#include "Precompiled.h"
#include "Component.h"
#include "Objects/Object.h"

namespace Reflex::Components
{
	ComponentFamily BaseComponent::s_componentFamilyIdx = 0;

	BaseComponent::BaseComponent( const Object& object )
		: m_object( object )
	{

	}

	BaseComponent::BaseComponent( const BaseComponent& other )
		: m_object( other.m_object )
	{

	}

	Reflex::Object BaseComponent::GetObject() const
	{
		return m_object;
	}

	Reflex::Handle< Transform > BaseComponent::GetTransform() const
	{ 
		return GetObject().GetTransform(); 
	}

	Reflex::Core::World& BaseComponent::GetWorld() const
	{
		return GetObject().GetWorld();
	}
}