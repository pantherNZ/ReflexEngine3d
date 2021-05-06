#pragma once

#include "Systems/System.h"
#include "Components/2D/SteeringComponent.h"

namespace Reflex::Systems
{
	using Reflex::Components::Steering;

	class SteeringSystem : public System
	{
	public:
		using System::System;

		void RegisterComponents() final;
		void Update( const float deltaTime ) final;

	protected:
		void Integrate( const Steering::Handle& boid, const float deltaTime ) const;
		sf::Vector2f Steering( const Steering::Handle& boid ) const;

		sf::Vector2f Seek( const Steering::Handle& boid, const sf::Vector2f& target ) const;
		sf::Vector2f Flee( const Steering::Handle& boid, const sf::Vector2f& target ) const;
		sf::Vector2f Arrival( const Steering::Handle& boid, const sf::Vector2f& target ) const;
		sf::Vector2f Wander( const Steering::Handle& boid ) const;
		sf::Vector2f Pursue( const Steering::Handle& boid, const Object& target, const bool useArrival = true ) const;
		sf::Vector2f Evade( const Steering::Handle& boid, const Object& target ) const;
		sf::Vector2f Flocking( const Steering::Handle& boid ) const;
		sf::Vector2f ObstacleAvoidance( const Steering::Handle& boid ) const;
	};
}