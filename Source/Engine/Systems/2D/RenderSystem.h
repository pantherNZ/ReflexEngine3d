#pragma once

#include "Systems/System.h"
#include "Components/2D/TransformComponent.h"

namespace Reflex::Systems
{
	class RenderSystem : public System, public EventReceiver
	{
	public:
		using System::System;

		void RegisterComponents() final { }
		bool ShouldAddObject( const Object& object ) const final;
		void AddComponent( const Object& object ) final;
		void OnComponentAdded( const Reflex::Object& object ) final;

		void Render( sf::RenderTarget& target, sf::RenderStates states ) const final;
		void OnSystemStartup() final {}
		void OnSystemShutdown() final { }

		// Transform event callback
		void OnRenderIndexChanged( const Components::Transform::RenderIndexChangedEvent& e );

		std::vector< Reflex::Object >::const_iterator GetInsertionIndex( const Object& object ) const;

	protected:
		std::vector< Reflex::ComponentFamily > m_objectRenderComponents;
	};
}