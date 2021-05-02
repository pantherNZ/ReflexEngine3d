#include "Precompiled.h"
#include "PhysicsSystem.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"

namespace Reflex::Systems
{
	void PhysicsSystem::RegisterComponents()
	{
		RequiresComponent( Reflex::Components::RigidBody );
	}

	void PhysicsSystem::Update( const float deltaTime )
	{
		for( const auto& object : m_releventObjects )
		{
			if( const auto rigidBody = object.GetComponent< Reflex::Components::RigidBody >() )
			{
				object.GetTransform()->setPosition( rigidBody->GetPosition() );
				object.GetTransform()->setRotation( rigidBody->GetRotation() );
			}
		}
	}
}