#include "SFMLObjectComponent.h"
#include "Objects/Object.h"
#include "Components/2D/ColliderComponent.h"

namespace Reflex::Components
{
	CircleShape::CircleShape( const Reflex::Object& owner, const float radius, const std::size_t pointCount, const std::optional< sf::Color > colour )
		: Component< CircleShape >( owner )
		, sf::CircleShape( radius, pointCount )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setFillColor( *colour );
	}

	CircleShape::CircleShape( const Reflex::Object& owner, const std::optional< sf::Color > colour )
		: Component< CircleShape >( owner )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setFillColor( *colour );
	}

	RectangleShape::RectangleShape( const Reflex::Object& owner, const sf::Vector2f& size, const std::optional< sf::Color > colour )
		: Component< RectangleShape >( owner )
		, sf::RectangleShape( size )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setFillColor( *colour );
	}

	RectangleShape::RectangleShape( const Reflex::Object& owner, const std::optional< sf::Color > colour )
		: Component< RectangleShape >( owner )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setFillColor( *colour );
	}

	ConvexShape::ConvexShape( const Reflex::Object& owner, const std::size_t pointCount, const std::optional< sf::Color > colour )
		: Component< ConvexShape >( owner )
		, sf::ConvexShape( pointCount )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setFillColor( *colour );
	}

	ConvexShape::ConvexShape( const Reflex::Object& owner, const std::vector< sf::Vector2f >& points, const std::optional< sf::Color > colour )
		: Component< ConvexShape >( owner )
		, sf::ConvexShape( points.size() )
	{
		Reflex::CenterOrigin( *this );
		for( size_t i = 0; i < points.size(); ++i )
			setPoint( i, points[i] );

		if( colour )
			setFillColor( *colour );
	}

	ConvexShape::ConvexShape( const Reflex::Object& owner, const std::optional< sf::Color > colour )
		: Component< ConvexShape >( owner )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setFillColor( *colour );
	}

	Sprite::Sprite( const Reflex::Object& owner, const sf::Texture& texture, const std::optional< sf::Color > colour )
		: Component< Sprite >( owner )
		, sf::Sprite( texture )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setColor( *colour );
	}

	Sprite::Sprite( const Reflex::Object& owner, const sf::Texture& texture, const sf::IntRect& rectangle, const std::optional< sf::Color > colour )
		: Component< Sprite >( owner )
		, sf::Sprite( texture, rectangle )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setColor( *colour );
	}
	
	Sprite::Sprite( const Reflex::Object& owner, const std::optional< sf::Color > colour )
		: Component< Sprite >( owner )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setColor( *colour );
	}

	Text::Text( const Reflex::Object& owner, const sf::String& string, const sf::Font& font, const unsigned characterSize, const std::optional< sf::Color > colour )
		: Component< Text >( owner )
		, sf::Text( string, font, characterSize )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setFillColor( *colour );
	}

	Text::Text( const Reflex::Object& owner, const std::optional< sf::Color > colour )
		: Component< Text >( owner )
	{
		Reflex::CenterOrigin( *this );
		if( colour )
			setFillColor( *colour );
	}

	TODO( "SFML object set values (deserialisation" );
	bool CircleShape::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	bool RectangleShape::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	bool ConvexShape::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	bool Sprite::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	bool Text::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	void CircleShape::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		values.emplace_back( "Radius", Reflex::ToString( getRadius() ) );
		GetShapeValues( values, *this);
		GetColourValues( values, *this );
	}

	void RectangleShape::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		GetShapeValues( values, *this );
		GetColourValues( values, *this );
	}

	void ConvexShape::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		GetShapeValues( values, *this );
		GetColourValues( values, *this );
	}

	void Sprite::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		TODO( "Serialise sprite texture path" );
		if( !Reflex::IsDefault( getColor() ) )
			values.emplace_back( "Colour", Reflex::ToString( getColor() ) );
	}

	void Text::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		GetColourValues( values, *this );
		values.emplace_back( "String", getString() );
		values.emplace_back( "Style", Reflex::ToString( getStyle() ) );
	}

	void CircleShape::CreateRigidBody( const b2BodyType type )
	{
		GetObject().TryAddComponent< Reflex::Components::RigidBody >( type );
		GetObject().AddComponent< Reflex::Components::CircleCollider >( getRadius() );
	}

	void RectangleShape::CreateRigidBody( const b2BodyType type )
	{
		GetObject().TryAddComponent< Reflex::Components::RigidBody >( type );
		GetObject().AddComponent< Reflex::Components::RectangleCollider >( getSize() );
	}

	void ConvexShape::CreateRigidBody( const b2BodyType type )
	{
		GetObject().TryAddComponent< Reflex::Components::RigidBody >( type );
		GetObject().AddComponent< Reflex::Components::ConvexCollider >( m_points );
	}
}