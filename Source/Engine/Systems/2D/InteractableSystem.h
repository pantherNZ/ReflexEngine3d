#pragma once

#include "System.h"
#include "TransformComponent.h"

namespace Reflex::Systems
{
	class InteractableSystem : public System
	{
	public:
		using System::System;

		void RegisterComponents() final;
		void Update( const float deltaTime ) final;
		void ProcessEvent( const sf::Event& event ) final;

		void OnSystemStartup() final { }
		void OnSystemShutdown() final { }

	protected:
		bool CheckCollision( const Reflex::Components::Transform::Handle& transform, const sf::FloatRect& localBounds, const sf::Vector2f& mousePosition ) const;

	protected:
		bool m_mousePressed = false;
		bool m_mouseReleased = false;
	};
}