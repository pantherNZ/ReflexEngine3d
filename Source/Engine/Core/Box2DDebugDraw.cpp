#include "Precompiled.h"
#include "Box2DDebugDraw.h"

namespace Reflex::Core
{
	Box2DDebugDraw::Box2DDebugDraw( sf::RenderWindow& window, const float unitToPixelScale )
		: m_window( window ) 
		, m_unitToPixelScale( unitToPixelScale )
	{
	}

	void Box2DDebugDraw::DrawPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color )
	{
		sf::ConvexShape polygon( vertexCount );
		sf::Vector2f center;
		for( int i = 0; i < vertexCount; i++ )
		{
			sf::Vector2f transformedVec = Reflex::B2VecToVector2f( vertices[i], m_unitToPixelScale );
			polygon.setPoint( i, sf::Vector2f( std::floor( transformedVec.x ), std::floor( transformedVec.y ) ) ); 
		}
		polygon.setOutlineThickness( -1.f );
		polygon.setFillColor( sf::Color::Transparent );
		polygon.setOutlineColor( Reflex::ToColour( color ) );

		m_window.draw( polygon );
	}

	void Box2DDebugDraw::DrawSolidPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color )
	{
		sf::ConvexShape polygon( vertexCount );
		for( int i = 0; i < vertexCount; i++ )
		{
			sf::Vector2f transformedVec = Reflex::B2VecToVector2f( vertices[i], m_unitToPixelScale );
			polygon.setPoint( i, sf::Vector2f( std::floor( transformedVec.x ), std::floor( transformedVec.y ) ) );
		}
		polygon.setOutlineThickness( -1.f );
		polygon.setFillColor( Reflex::ToColour( color, 60 ) );
		polygon.setOutlineColor( Reflex::ToColour( color ) );

		m_window.draw( polygon );
	}

	void Box2DDebugDraw::DrawCircle( const b2Vec2& center, float radius, const b2Color& color )
	{
		sf::CircleShape circle( radius * m_unitToPixelScale );
		circle.setOrigin( radius * m_unitToPixelScale, radius * m_unitToPixelScale );
		circle.setPosition( Reflex::B2VecToVector2f( center, m_unitToPixelScale ) );
		circle.setFillColor( sf::Color::Transparent );
		circle.setOutlineThickness( -1.f );
		circle.setOutlineColor( Reflex::ToColour( color ) );

		m_window.draw( circle );
	}

	void Box2DDebugDraw::DrawSolidCircle( const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color )
	{
		sf::CircleShape circle( radius * m_unitToPixelScale );
		circle.setOrigin( radius * m_unitToPixelScale, radius * m_unitToPixelScale );
		circle.setPosition( Reflex::B2VecToVector2f( center, m_unitToPixelScale ) );
		circle.setFillColor( Reflex::ToColour( color, 60 ) );
		circle.setOutlineThickness( 1.f );
		circle.setOutlineColor( Reflex::ToColour( color ) );

		b2Vec2 endPoint = center + radius * axis;
		sf::Vertex line[2] =
		{
			sf::Vertex( Reflex::B2VecToVector2f( center, m_unitToPixelScale ), Reflex::ToColour( color ) ),
			sf::Vertex( Reflex::B2VecToVector2f( endPoint, m_unitToPixelScale ), Reflex::ToColour( color ) ),
		};

		m_window.draw( circle );
		m_window.draw( line, 2, sf::Lines );
	}

	void Box2DDebugDraw::DrawSegment( const b2Vec2& p1, const b2Vec2& p2, const b2Color& color )
	{
		sf::Vertex line[] =
		{
			sf::Vertex( Reflex::B2VecToVector2f( p1, m_unitToPixelScale ), Reflex::ToColour( color ) ),
			sf::Vertex( Reflex::B2VecToVector2f( p2, m_unitToPixelScale ), Reflex::ToColour( color ) )
		};

		m_window.draw( line, 2, sf::Lines );
	}

	void Box2DDebugDraw::DrawTransform( const b2Transform& xf )
	{
		float lineLength = 0.4f;

		/*b2Vec2 xAxis(b2Vec2(xf.p.x + (lineLength * xf.q.c), xf.p.y + (lineLength * xf.q.s)));*/
		b2Vec2 xAxis = xf.p + lineLength * xf.q.GetXAxis();
		sf::Vertex redLine[] =
		{
			sf::Vertex( Reflex::B2VecToVector2f( xf.p, m_unitToPixelScale ), sf::Color::Red ),
			sf::Vertex( Reflex::B2VecToVector2f( xAxis, m_unitToPixelScale ), sf::Color::Red )
		};

		// You might notice that the ordinate(Y axis) points downward unlike the one in Box2D testbed
		// That's because the ordinate in SFML coordinate system points downward while the OpenGL(testbed) points upward
		/*b2Vec2 yAxis(b2Vec2(xf.p.x + (lineLength * -xf.q.s), xf.p.y + (lineLength * xf.q.c)));*/
		b2Vec2 yAxis = xf.p + lineLength * xf.q.GetYAxis();
		sf::Vertex greenLine[] =
		{
			sf::Vertex( Reflex::B2VecToVector2f( xf.p, m_unitToPixelScale ), sf::Color::Green ),
			sf::Vertex( Reflex::B2VecToVector2f( yAxis, m_unitToPixelScale ), sf::Color::Green )
		};

		m_window.draw( redLine, 2, sf::Lines );
		m_window.draw( greenLine, 2, sf::Lines );
	}

	void Box2DDebugDraw::DrawPoint( const b2Vec2& p, float size, const b2Color& color )
	{
		sf::CircleShape circle( size * m_unitToPixelScale );
		circle.setOrigin( size * m_unitToPixelScale, size * m_unitToPixelScale );
		circle.setPosition( Reflex::B2VecToVector2f( p, m_unitToPixelScale ) );
		circle.setFillColor( Reflex::ToColour( color ) );

		m_window.draw( circle );
	}

}