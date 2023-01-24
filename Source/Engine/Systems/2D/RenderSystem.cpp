#include "Precompiled.h"
#include "Core/World.h"
#include "RenderSystem.h"
#include "Components/2D/SFMLObjectComponent.h"
#include "Components/2D/TransformComponent.h"

namespace Reflex::Systems
{
	bool RenderSystem::ShouldAddObject( const Object& object ) const
	{
		for( unsigned i = 0; i < Reflex::MaxComponents; ++i )
			if( const auto* cmp = GetWorld().ObjectGetComponent( object, i ) )
				if( cmp->IsRenderComponent() )
					return true;

		return false;
	}

	void RenderSystem::AddComponent( const Object& object )
	{
		m_releventObjects.insert( GetInsertionIndex( object ), object );
	}

	std::vector< Reflex::Object >::const_iterator RenderSystem::GetInsertionIndex( const Object& object ) const
	{
		return std::lower_bound( m_releventObjects.begin(), m_releventObjects.end(), object, []( const Reflex::Object& left, const Reflex::Object& right )
		{
			return left.GetTransform()->GetRenderIndex() < right.GetTransform()->GetRenderIndex();
		} );
	}

	void RenderSystem::OnComponentAdded( const Reflex::Object& object )
	{
		object.GetTransform()->Subscribe< Components::Transform::RenderIndexChangedEvent >( *this, &RenderSystem::OnRenderIndexChanged );
	}

	void RenderSystem::OnRenderIndexChanged( const Components::Transform::RenderIndexChangedEvent& e )
	{
		m_releventObjects.erase( Reflex::Find( m_releventObjects, e.object ) );
		const auto newPos = GetInsertionIndex( e.object );
		m_releventObjects.insert( newPos, e.object );
	}

	void RenderSystem::Render( sf::RenderTarget& target, sf::RenderStates states ) const
	{
		PROFILE;
		sf::RenderStates copied_states( states );

		for( const auto& object : m_releventObjects )
		{
			for( unsigned i = 0; i < Reflex::MaxComponents; ++i )
			{
				const auto* cmp = GetWorld().ObjectGetComponent( object, i );

				if( !cmp || !cmp->IsRenderComponent() )
					continue;

				copied_states.transform = object.GetTransform()->GetWorldTransform();
				cmp->Render( target, copied_states );
			}
		}
	}
}