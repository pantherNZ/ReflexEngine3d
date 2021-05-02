#include "Precompiled.h"
#include "SteeringSystem.h"
#include "TransformComponent.h"
#include "World.h"
#include "Logging.h"
#include "SFMLObjectComponent.h"

namespace Reflex::Systems
{
	using namespace Reflex::Components;

	void SteeringSystem::RegisterComponents()
	{
		RequiresComponent( Reflex::Components::Steering );
	}

	void SteeringSystem::Update( const float deltaTime )
	{
		PROFILE;
		ForEachObject< Reflex::Components::Steering >( [&]( const Reflex::Components::Steering::Handle& boid )
		{
			Integrate( boid, deltaTime );
		} );
	}

	void SteeringSystem::Integrate( const Steering::Handle& boid, const float deltaTime ) const
	{
		PROFILE;
		auto transform = boid->GetObject().GetTransform();

		if( boid->m_maxForce <= 0.0f || transform->GetMaxVelocity() <= 0.0f )
			return;

		boid->m_steering = Steering( boid );
		const auto acceleration = boid->m_steering / boid->m_mass;
		transform->SetVelocity( transform->GetVelocity() + acceleration * deltaTime );
	}

	sf::Vector2f SteeringSystem::Steering( const Steering::Handle& boid ) const
	{
		sf::Vector2f steering;
		if( boid->IsBehaviourSet( SteeringBehaviours::Seek ) )					steering += Seek( boid, boid->m_targetObject ? boid->m_targetObject.GetTransform()->getPosition() : boid->m_targetPosition );
		if( boid->IsBehaviourSet( SteeringBehaviours::Flee ) )					steering += Flee( boid, boid->m_targetObject ? boid->m_targetObject.GetTransform()->getPosition() : boid->m_targetPosition );
		if( boid->IsBehaviourSet( SteeringBehaviours::Arrival ) )				steering += Arrival( boid, boid->m_targetObject ? boid->m_targetObject.GetTransform()->getPosition() : boid->m_targetPosition );
		if( boid->IsBehaviourSet( SteeringBehaviours::Wander ) )				steering += Wander( boid );
		if( boid->IsBehaviourSet( SteeringBehaviours::Pursue ) )				steering += Pursue( boid, boid->m_targetObject );
		if( boid->IsBehaviourSet( SteeringBehaviours::Evade ) )					steering += Evade( boid, boid->m_targetObject );
		if( boid->IsBehaviourSet( SteeringBehaviours::Alignment ) ||
			boid->IsBehaviourSet( SteeringBehaviours::Cohesion ) ||
			boid->IsBehaviourSet( SteeringBehaviours::Separation ) )			steering += Flocking( boid );
		if( boid->IsBehaviourSet( SteeringBehaviours::ObstacleAvoidance ) )		steering += ObstacleAvoidance( boid );

		return Reflex::Truncate( steering, boid->m_maxForce );
	}

	sf::Vector2f SteeringSystem::Seek( const Steering::Handle& boid, const sf::Vector2f& target ) const
	{
		const auto transform = boid->GetTransform();
		const auto pos = transform->getPosition();

		if( target == pos )
			return {};

		if( boid->m_ignoreDistance > 0.0f && Reflex::GetDistanceSq( target, pos ) >= boid->m_ignoreDistance * boid->m_ignoreDistance )
			return {};

		return ( Reflex::ScaleTo( target - pos, transform->GetMaxVelocity() ) - transform->GetVelocity() ) * boid->m_seekForce * boid->m_forceMultiplier;
	}

	sf::Vector2f SteeringSystem::Flee( const Steering::Handle& boid, const sf::Vector2f& target ) const
	{
		return -Seek( boid, target );
	}

	sf::Vector2f SteeringSystem::Arrival( const Steering::Handle& boid, const sf::Vector2f& target ) const
	{
		const auto transform = boid->GetTransform();
		const auto direction = target - transform->getPosition();

		if( boid->m_ignoreDistance > 0.0f && Reflex::GetMagnitudeSq( direction ) >= boid->m_ignoreDistance * boid->m_ignoreDistance )
			return {};

		const auto length = Reflex::GetMagnitude( direction );

		if( length <= 0.00001f )
			return {};

		const auto speedModifier = length < boid->m_slowingRadius ? length / boid->m_slowingRadius : 1.0f;
		return ( ( direction / length ) * transform->GetMaxVelocity() * speedModifier ) - transform->GetVelocity();
	}

	sf::Vector2f SteeringSystem::Wander( const Steering::Handle& boid ) const
	{
		PROFILE;
		assert( boid->m_wanderCircleRadius > 0.0f );
		if( boid->m_wanderCircleRadius <= 0.0f )
			return {};

		auto transform = boid->GetObject().GetTransform();

		if( transform->GetVelocity().x == 0.0f && transform->GetVelocity().y == 0.0f )
			transform->SetVelocity( Reflex::RandomUnitVector() );

		boid->m_wanderDirection.x += Reflex::RandomFloat( -boid->m_wanderJitter / 2.0f, boid->m_wanderJitter / 2.0f ) * GetWorld().GetDeltaTime();
		boid->m_wanderDirection.y += Reflex::RandomFloat( -boid->m_wanderJitter / 2.0f, boid->m_wanderJitter / 2.0f ) * GetWorld().GetDeltaTime();
		Reflex::ScaleTo( boid->m_wanderDirection, boid->m_wanderCircleRadius );
		const auto targetForce = Reflex::ScaleTo( Reflex::ScaleTo( transform->GetVelocity(), boid->m_wanderCircleDistance ) + boid->m_wanderDirection, transform->GetMaxVelocity() );
		return ( targetForce - transform->GetVelocity() ) * boid->m_wanderForce * boid->m_forceMultiplier;
	}

	sf::Vector2f SteeringSystem::Pursue( const Steering::Handle& boid, const Object& target, const bool useArrival ) const
	{
		assert( target );
		if( !target )
			return {};

		const auto speed = Reflex::GetMagnitude( boid->GetObject().GetTransform()->GetVelocity() );
		const auto time = speed <= 0.0001f ? 0.0f : ( Reflex::GetDistance( boid->GetTransform()->getPosition(), target.GetTransform()->getPosition() ) / speed );
		const auto targetPos = target.GetTransform()->getPosition() + target.GetTransform()->GetVelocity() * time;
		return useArrival ? Arrival( boid, targetPos ) : Seek( boid, targetPos );
	}

	sf::Vector2f SteeringSystem::Evade( const Steering::Handle& boid, const Object& target ) const
	{
		return -Pursue( boid, target, false );
	}

	sf::Vector2f SteeringSystem::Flocking( const Steering::Handle& boid ) const
	{
		sf::Vector2f alignment, cohesion, separation;
		const auto pos = boid->GetTransform()->getPosition();
		unsigned counter = 0;

#ifndef DISABLE_TILEMAP
		GetWorld().GetTileMap().ForEachInRange( pos, boid->m_neighbourRange, [&]( const Reflex::Object& nearby )
		{
			if( boid == nearby || !nearby.HasComponent< Reflex::Components::Steering >() )
				return;
#else
		ForEachObject< Reflex::Components::Steering >( [&]( const Reflex::Components::Steering::Handle& steering )
		{
			const auto nearby = steering->GetObject();

			if( boid == nearby )
				return;
#endif
			const auto nearbyTransform = nearby.GetTransform();
			const auto nearbyPos = nearbyTransform->getPosition();
			const auto direction = pos - nearbyPos;

			if( Reflex::GetMagnitudeSq( direction ) > boid->m_neighbourRange * boid->m_neighbourRange )
				return;

			counter++;
			alignment += Reflex::Normalise( nearbyTransform->GetVelocity() );
			cohesion += nearbyPos;
			separation += direction / -Reflex::GetMagnitudeSq( direction );
		} );

		if( counter )
		{
			const auto direction = Reflex::Normalise( boid->GetTransform()->GetVelocity() );
			
			cohesion /= ( float )counter;
			cohesion = Reflex::Normalise( Reflex::Normalise( cohesion - boid->GetTransform()->getPosition() ) - direction );

			alignment /= ( float )counter;
			alignment = Reflex::Normalise( alignment - direction );

			separation /= ( float )counter;
			separation = Reflex::Normalise( separation - direction );
		}

		const auto alignmentForce = boid->IsBehaviourSet( SteeringBehaviours::Alignment ) ? boid->m_alignmentForce * boid->m_forceMultiplier : 0.0f;
		const auto cohesionForce = boid->IsBehaviourSet( SteeringBehaviours::Cohesion ) ? boid->m_cohesionForce * boid->m_forceMultiplier : 0.0f;
		const auto separationForce = boid->IsBehaviourSet( SteeringBehaviours::Separation ) ? boid->m_separationForce * boid->m_forceMultiplier : 0.0f;

		return alignment * alignmentForce + cohesion * cohesionForce + separation * separationForce;
	}

	sf::Vector2f SteeringSystem::ObstacleAvoidance( const Steering::Handle& boid ) const
	{
		if( Reflex::GetMagnitudeSq( boid->GetTransform()->GetVelocity() ) <= 0.0001f )
			return {};

		const auto thisPos = boid->GetTransform()->getPosition();
		const auto offset = boid->GetTransform()->GetVelocity() * boid->m_avoidanceTraceLength;

		if( const auto middle = boid->GetWorld().RayCast( thisPos, thisPos + offset ) )
		{
			const auto steeringDirection = ( Reflex::ScaleTo( middle.normal, boid->GetTransform()->GetMaxVelocity() ) - boid->GetTransform()->GetVelocity() );
			return steeringDirection * boid->m_avoidanceForce * boid->m_forceMultiplier;
		}

		if( const auto left = boid->GetWorld().RayCast( thisPos, thisPos + Reflex::RotateVector( offset, Reflex::ToRadians( 20.0f ) ) ) )
		{
			const auto steeringDirection = ( Reflex::ScaleTo( left.normal, boid->GetTransform()->GetMaxVelocity() ) - boid->GetTransform()->GetVelocity() );
			return steeringDirection * boid->m_avoidanceForce * boid->m_forceMultiplier;
		}

		if( const auto right = boid->GetWorld().RayCast( thisPos, thisPos + Reflex::RotateVector( offset, Reflex::ToRadians( -20.0f ) ) ) )
		{
			const auto steeringDirection = ( Reflex::ScaleTo( right.normal, boid->GetTransform()->GetMaxVelocity() ) - boid->GetTransform()->GetVelocity() );
			return steeringDirection * boid->m_avoidanceForce * boid->m_forceMultiplier;
		}

		return {};
	}

}