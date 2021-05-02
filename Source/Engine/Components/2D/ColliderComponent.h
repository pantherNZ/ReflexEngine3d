#pragma once

#include "Component.h"
#include "RigidBodyComponent.h"
#include "Events.h"

namespace Reflex::Components
{
	template< typename T, typename Shape >
	class BaseCollider 
		: public Component< T, RigidBody >
		, protected Shape
		, public Core::EventReceiver
	{
	public:
		using Component< T, RigidBody >::Component;
		static_assert( std::is_convertible< Shape*, b2Shape* >::value, "Shape must inherit from b2Shape class" );

		float GetDensity() const { return m_density; }
		void SetDensity( const float density, const bool recreate = true ) { m_density = density; if( recreate ) Recreate(); }
		void OnConstructionComplete() final { Recreate(); }

		virtual void Recreate()
		{
			const auto rigidBody = Component< T, RigidBody >::GetObject().template GetComponent< Components::RigidBody >();
			rigidBody->Subscribe< Components::RigidBody::RigidBodyRecreatedEvent >( *this, &BaseCollider::OnRigidBodyRecreated );

			if( m_fixture )
				rigidBody->GetBody().DestroyFixture( m_fixture );

			m_fixture = rigidBody->GetBody().CreateFixture( this, m_density );
		}

	protected:
		void OnRigidBodyRecreated( const Components::RigidBody::RigidBodyRecreatedEvent& e )
		{
			e.oldBody.DestroyFixture( m_fixture );
			m_fixture = nullptr;
			Recreate();
		}

	protected:
		b2Fixture* m_fixture = nullptr;
		float m_density = 1.0f;
	};

	class CircleCollider : public BaseCollider< CircleCollider, b2CircleShape >
	{
	public:
		CircleCollider( const Reflex::Object& object, const float radius = 0.0f, const float density = 1.0f )
			: BaseCollider< CircleCollider, b2CircleShape >( object )
		{
			m_radius = Reflex::ToBox2DUnits( radius );
		}

		static std::string GetComponentName() { return "CircleCollider"; }
		float GetRadius() const { return Reflex::ToWorldUnits( m_radius ); }
		void SetRadius( const float radius, const bool recreate = true ) { m_radius = Reflex::ToBox2DUnits( radius ); if( recreate ) Recreate(); }
	};

	class RectangleCollider : public BaseCollider< RectangleCollider, b2PolygonShape >
	{
	public:
		RectangleCollider( const Reflex::Object& object, const sf::Vector2f& size = {}, const float density = 1.0f )
			: BaseCollider< RectangleCollider, b2PolygonShape >( object )
			, m_size( size )
		{
			
		}

		static std::string GetComponentName() { return "RectangleCollider"; }
		sf::Vector2f GetSize() const { return m_size; }
		void SetSize( const sf::Vector2f& size, const bool recreate = true ) { m_size = size; if( recreate ) Recreate(); }

		void Recreate() final
		{
			const auto b2Size = Reflex::Vector2fToB2Vec( m_size );
			SetAsBox( b2Size.x / 2.0f, b2Size.y / 2.0f, b2Vec2( 0.0f, 0.0f), Reflex::ToRadians( GetObject().GetTransform()->getRotation() ) );
			BaseCollider< RectangleCollider, b2PolygonShape >::Recreate();
		}

	protected:
		sf::Vector2f m_size;
	};

	class ConvexCollider : public BaseCollider< ConvexCollider, b2PolygonShape >
	{
	public:
		ConvexCollider( const Reflex::Object& object, const std::vector< sf::Vector2f >& points = {}, const float density = 1.0f )
			: BaseCollider< ConvexCollider, b2PolygonShape >( object )
		{
		}

		static std::string GetComponentName() { return "ConvexCollider"; }

		void Recreate() final
		{
			std::vector< b2Vec2 > b2Points( m_points.size() );

			for( size_t i = 0; i < b2Points.size(); i++ )
				b2Points[i] = Reflex::Vector2fToB2Vec( m_points[i] );

			Set( b2Points.data(), ( int )b2Points.size() );

			BaseCollider< ConvexCollider, b2PolygonShape >::Recreate();
		}

		// Note this currently returns the same points passed in, ideally it should return the post-processed points after calling set.
		// From the box2d manual: "Also note that the convex hull function may eliminate and/or re-order the points you provide" (https://box2d.org/documentation/)
		const std::vector< sf::Vector2f >& GetPoints() const { return m_points; }
		void SetPoints( const std::vector< sf::Vector2f >& points, const bool recreate = true ) { m_points = points; if( recreate ) Recreate(); }

	protected:
		std::vector< sf::Vector2f > m_points;
	};
}