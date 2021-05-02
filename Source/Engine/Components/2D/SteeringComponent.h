#pragma once

#include "Component.h"
#include "Utility.h"
#include "Object.h"

namespace Reflex::Systems { class SteeringSystem; }

namespace Reflex::Components
{
	enum class SteeringBehaviours
	{
		Seek,
		Flee,
		Arrival,
		Wander,
		Pursue,
		Evade,
		Alignment,
		Cohesion,
		Separation,
		ObstacleAvoidance,
		NumBehaviours,
	};

	extern std::string steeringBehaviourNames[( size_t )SteeringBehaviours::NumBehaviours];

	// Class definition
	class Steering : public Component< Steering >
	{
	public:
		friend class Reflex::Systems::SteeringSystem;
		typedef std::bitset< ( size_t )SteeringBehaviours::NumBehaviours > BehaviourFlags;

		using Component< Steering >::Component;
		bool SetValue( const std::string& variable, const std::string& value ) override;
		void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const override;
		static std::string GetComponentName() { return "Steering"; }

		void Seek( const sf::Vector2f& target, const float maxVelocity );
		void Flee( const sf::Vector2f& target, const float maxVelocity );
		void Arrival( const sf::Vector2f& target, const float slowingRadius, const float maxVelocity );
		void Wander( const float circleRadius, const float circleDistance, const float jitter, const float maxVelocity );
		void Pursue( const Reflex::Object& target, const float slowingRadius, const float maxVelocity );
		void Evade( const Reflex::Object& target, const float ignoreDistance, const float maxVelocity );
		
		void Alignment( const float neighbourRange, const float alignmentForce, const float maxVelocity );
		void Cohesion( const float neighbourRange, const float cohesionForce, const float maxVelocity );
		void Separation( const float neighbourRange, const float separationForce, const float maxVelocity );
		void Flocking( const float neighbourRange, const float alignmentForce, const float cohesionForce, const float separationForce, const float maxVelocity );
		
		void ObstacleAvoidance( const float avoidanceForce, const float maxVelocity );

		void EnableBehaviour( const SteeringBehaviours behaviour );
		void DisableBehaviour( const SteeringBehaviours behaviour );
		void ClearBehaviours();
		bool IsBehaviourSet( const SteeringBehaviours behaviour ) const;
		void SetBehaviours( const BehaviourFlags& flags );
		const BehaviourFlags& GetBehaviours() const;
		BehaviourFlags& GetBehaviours();
		void CopyValuesFrom( const Steering::Handle& other );

		void SetTargetObject( const Reflex::Object& target );
		void SetTargetPosition( const sf::Vector2f& location );

		bool IsRenderComponent() const final { return true; }
		void Render( sf::RenderTarget& target, sf::RenderStates states ) const final;

		// Generic
		float m_mass = 1.0f;
		float m_maxForce = 100.0f;
		float m_forceMultiplier = 100.0f;

		// Arrival
		float m_slowingRadius = 100.0f;

		// Seek / Flee
		float m_ignoreDistance = 0.0f;
		float m_seekForce = 1.0f;

		// Wander
		float m_wanderCircleRadius = 1.2f;
		float m_wanderCircleDistance = 2.0f;
		float m_wanderJitter = 80.0f;
		float m_wanderForce = 1.0f;

		// Alignment / Cohesion / Separation
		float m_neighbourRange = 300.0f;
		float m_alignmentForce = 1.0f;
		float m_cohesionForce = 1.0f;
		float m_separationForce = 1.0f;

		// Avoidance
		float m_avoidanceForce = 1.0f;
		float m_avoidanceTraceLength = 2.0f;

	protected:
		void SetBehaviourInternal( const SteeringBehaviours behaviour );

		std::bitset< ( size_t )SteeringBehaviours::NumBehaviours > m_behaviours;
		sf::Vector2f m_desired;
		sf::Vector2f m_steering;
		Reflex::Object m_targetObject;
		sf::Vector2f m_targetPosition;
		sf::Vector2f m_wanderDirection;
	};
}