#include "Precompiled.h"
#include "SteeringComponent.h"
#include "TransformComponent.h"
#include "SFMLObjectComponent.h"

namespace Reflex::Components
{
	std::string steeringBehaviourNames[] =
	{
		"Seek",
		"Flee",
		"Arrival",
		"Wander",
		"Pursue",
		"Evade",
		"Alignment",
		"Cohesion",
		"Separation",
		"Obstacle Avoidance",
	};

	bool Steering::SetValue( const std::string& variable, const std::string& value )
	{
		TrySetValue( "MaxForce", m_maxForce );
		TrySetValue( "Mass", m_mass );
		TrySetValue( "SlowingRadius", m_slowingRadius );
		TrySetValue( "WanderCircleRadius", m_wanderCircleRadius );
		TrySetValue( "WanderJitter", m_wanderJitter );
		TrySetValue( "WanderCircleDistance", m_wanderCircleDistance );
		TrySetValue( "NeighbourRange", m_neighbourRange );
		TrySetValue( "AlignmentForce", m_alignmentForce );
		TrySetValue( "CohesionForce", m_cohesionForce );
		TrySetValue( "SeparationForce", m_separationForce );

		for( unsigned i = 0; i < std::size( steeringBehaviourNames ); ++i )
		{
			if( variable == steeringBehaviourNames[i] )
			{
				m_behaviours.set( i, Reflex::FromString< bool >( value ) );
				return true;
			}
		}

		return false;
	}

	void Steering::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		for( unsigned i = 0; i < std::size( steeringBehaviourNames ); ++i )
			if( m_behaviours.test( i ) )
				values.emplace_back( steeringBehaviourNames[i], "true" );

		GetValue( "MaxForce", m_maxForce );
		GetValue( "Mass", m_maxForce );
		TryGetValue( "SlowingRadius", m_slowingRadius, IsBehaviourSet( SteeringBehaviours::Arrival ) );

		// Wander
		if( IsBehaviourSet( SteeringBehaviours::Wander ) )
		{
			GetValue( "WanderCircleRadius", m_wanderCircleRadius );
			GetValue( "WanderCircleDistance", m_wanderCircleDistance );
			GetValue( "WanderJitter", m_wanderJitter );
		}

		// Flocking
		if( IsBehaviourSet( SteeringBehaviours::Alignment ) ||
			IsBehaviourSet( SteeringBehaviours::Cohesion ) ||
			IsBehaviourSet( SteeringBehaviours::Separation ) )
		{
			GetValue( "NeighbourRange", m_neighbourRange );
			TryGetValue( "AlignmentForce", m_alignmentForce, IsBehaviourSet( SteeringBehaviours::Alignment ) );
			TryGetValue( "CohesionForce", m_cohesionForce, IsBehaviourSet( SteeringBehaviours::Cohesion ) );
			TryGetValue( "SeparationForce", m_separationForce, IsBehaviourSet( SteeringBehaviours::Separation ) );
		}
	}

	void Steering::Seek( const sf::Vector2f& target, const float maxVelocity )
	{
		SetBehaviourInternal( SteeringBehaviours::Seek );
		m_targetPosition = target;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Flee( const sf::Vector2f& target, const float maxVelocity )
	{
		SetBehaviourInternal( SteeringBehaviours::Flee );
		m_targetPosition = target;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Arrival( const sf::Vector2f& target, const float slowingRadius, const float maxVelocity )
	{
		SetBehaviourInternal( SteeringBehaviours::Arrival );
		m_targetPosition = target;
		m_slowingRadius = slowingRadius;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Wander( const float circleRadius, const float circleDistance, const float jitter, const float maxVelocity )
	{
		SetBehaviourInternal( SteeringBehaviours::Wander );
		m_wanderCircleRadius = circleRadius;
		m_wanderCircleDistance = circleDistance;
		m_wanderJitter = jitter;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Pursue( const Reflex::Object& target, const float slowingRadius, const float maxVelocity )
	{
		SetBehaviourInternal( SteeringBehaviours::Pursue );
		m_targetObject = target;
		m_slowingRadius = slowingRadius;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Evade( const Reflex::Object& target, const float ignoreDistance, const float maxVelocity )
	{
		SetBehaviourInternal( SteeringBehaviours::Evade );
		m_targetObject = target;
		m_ignoreDistance = ignoreDistance;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Alignment( const float neighbourRange, const float alignmentForce, const float maxVelocity )
	{
		SetBehaviourInternal( SteeringBehaviours::Alignment );
		m_neighbourRange = neighbourRange;
		m_alignmentForce = alignmentForce;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Cohesion( const float neighbourRange, const float cohesionForce, const float maxVelocity )
	{
		SetBehaviourInternal( SteeringBehaviours::Cohesion );
		m_neighbourRange = neighbourRange;
		m_cohesionForce = cohesionForce;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Separation( const float neighbourRange, const float separationForce, const float maxVelocity )
	{
		SetBehaviourInternal( SteeringBehaviours::Separation );
		m_neighbourRange = neighbourRange;
		m_separationForce = separationForce;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Flocking( const float neighbourRange, const float alignmentForce, const float cohesionForce, const float separationForce, const float maxVelocity )
	{
		Alignment( neighbourRange, alignmentForce, maxVelocity );
		Cohesion( neighbourRange, cohesionForce, maxVelocity );
		Separation( neighbourRange, separationForce, maxVelocity );
	}

	void Steering::ObstacleAvoidance( const float avoidanceForce, const float maxVelocity )
	{
		SetBehaviourInternal( SteeringBehaviours::ObstacleAvoidance );
		m_avoidanceForce = avoidanceForce;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::EnableBehaviour( const SteeringBehaviours behaviour )
	{ 
		if( ( size_t )behaviour >= 0 && ( size_t )behaviour < ( size_t )SteeringBehaviours::NumBehaviours )
			m_behaviours.set( ( size_t )behaviour );
	}

	void Steering::DisableBehaviour( const SteeringBehaviours behaviour )
	{
		if( (size_t )behaviour >= 0 && (size_t )behaviour < (size_t )SteeringBehaviours::NumBehaviours )
			m_behaviours.reset( (size_t )behaviour );
	}

	void Steering::ClearBehaviours()
	{
		m_behaviours.reset();
	}

	void Steering::SetBehaviourInternal( const SteeringBehaviours behaviour )
	{
		m_behaviours.set( ( size_t )behaviour );
	}

	bool Steering::IsBehaviourSet( const SteeringBehaviours behaviour ) const
	{
		return m_behaviours.test( ( size_t )behaviour );
	}

	void Steering::SetBehaviours( const Steering::BehaviourFlags& flags )
	{
		m_behaviours = flags;
	}

	const Steering::BehaviourFlags& Steering::GetBehaviours() const
	{
		return m_behaviours;
	}

	Steering::BehaviourFlags& Steering::GetBehaviours()
	{
		return m_behaviours;
	}

	void Steering::CopyValuesFrom( const Steering::Handle& other )
	{
		SetBehaviours( other->GetBehaviours() );
		m_mass = other->m_mass;
		m_maxForce = other->m_maxForce;
		m_slowingRadius = other->m_slowingRadius;
		m_ignoreDistance = other->m_ignoreDistance;
		m_seekForce = other->m_seekForce;
		m_wanderCircleRadius = other->m_wanderCircleRadius;
		m_wanderCircleDistance = other->m_wanderCircleDistance;
		m_wanderJitter = other->m_wanderJitter;
		m_wanderForce = other->m_wanderForce;
		m_neighbourRange = other->m_neighbourRange;
		m_alignmentForce = other->m_alignmentForce;
		m_cohesionForce = other->m_cohesionForce;
		m_separationForce = other->m_separationForce;
		m_avoidanceForce = other->m_avoidanceForce;
		m_forceMultiplier = other->m_forceMultiplier;
		m_targetObject = other->m_targetObject;
		m_targetPosition = other->m_targetPosition;
	}

	void Steering::SetTargetObject( const Reflex::Object& target )
	{
		m_targetObject = target;
	}

	void Steering::SetTargetPosition( const sf::Vector2f& position )
	{
		m_targetPosition = position;
	}

	void Steering::Render( sf::RenderTarget& target, sf::RenderStates states ) const
	{
		if( IsBehaviourSet( SteeringBehaviours::ObstacleAvoidance ) )
		{
			const auto offset = GetTransform()->GetVelocity() * m_avoidanceTraceLength;

			sf::VertexArray s( sf::PrimitiveType::Lines );
			s.append( sf::Vertex( GetTransform()->getPosition(), sf::Color::Magenta ) );
			s.append( sf::Vertex( GetTransform()->getPosition() + offset, sf::Color::Magenta ) );
			
			s.append( sf::Vertex( GetTransform()->getPosition(), sf::Color::Magenta ) );
			s.append( sf::Vertex( GetTransform()->getPosition() + Reflex::RotateVector( offset, Reflex::ToRadians( -20.0f ) ), sf::Color::Magenta ) );

			s.append( sf::Vertex( GetTransform()->getPosition(), sf::Color::Magenta ) );
			s.append( sf::Vertex( GetTransform()->getPosition() + Reflex::RotateVector( offset, Reflex::ToRadians( 20.0f ) ), sf::Color::Magenta ) );
			target.draw( s );
		}
	}
}