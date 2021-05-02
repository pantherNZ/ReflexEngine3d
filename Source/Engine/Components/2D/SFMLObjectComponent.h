#pragma once

#include "Component.h"

namespace Reflex::Components
{
	// Class definition
	class CircleShape : public Component< CircleShape >, public sf::CircleShape
	{
	public:
		explicit CircleShape( const Reflex::Object& owner, const float radius, const std::size_t pointCount = 30, const std::optional< sf::Color > colour = std::nullopt );
		explicit CircleShape( const Reflex::Object& owner, const std::optional< sf::Color > colour = std::nullopt );

		static std::string GetComponentName() { return "CircleShape"; }
		bool SetValue( const std::string& variable, const std::string& value ) override;
		void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const override;
		bool IsRenderComponent() const final { return true; }
		void Render( sf::RenderTarget& target, sf::RenderStates states ) const final { target.draw( *this, states ); }

		void CreateRigidBody( const b2BodyType type = b2BodyType::b2_staticBody );
	};

	class RectangleShape : public Component< RectangleShape >, public sf::RectangleShape
	{
	public:
		explicit RectangleShape( const Reflex::Object& owner, const sf::Vector2f& size, const std::optional< sf::Color > colour = std::nullopt );
		explicit RectangleShape( const Reflex::Object& owner, const std::optional< sf::Color > colour = std::nullopt );

		static std::string GetComponentName() { return "RectangleShape"; }
		bool SetValue( const std::string& variable, const std::string& value ) override;
		void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const override;
		bool IsRenderComponent() const final { return true; }
		void Render( sf::RenderTarget& target, sf::RenderStates states ) const final { target.draw( *this, states ); }

		void CreateRigidBody( const b2BodyType type = b2BodyType::b2_staticBody );
	};

	class ConvexShape : public Component< ConvexShape >, public sf::ConvexShape
	{
	public:
		explicit ConvexShape( const Reflex::Object& owner, const std::size_t pointCount, const std::optional< sf::Color > colour = std::nullopt );
		explicit ConvexShape( const Reflex::Object& owner, const std::vector< sf::Vector2f >& points, const std::optional< sf::Color > colour = std::nullopt );
		explicit ConvexShape( const Reflex::Object& owner, const std::optional< sf::Color > colour = std::nullopt );

		static std::string GetComponentName() { return "ConvexShape"; }
		bool SetValue( const std::string& variable, const std::string& value ) override;
		void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const override;
		bool IsRenderComponent() const final { return true; }
		void Render( sf::RenderTarget& target, sf::RenderStates states ) const final { target.draw( *this, states ); }

		void CreateRigidBody( const b2BodyType type = b2BodyType::b2_staticBody );
	};

	class Sprite : public Component< Sprite >, public sf::Sprite
	{
	public:
		explicit Sprite( const Reflex::Object& owner, const sf::Texture& texture, const std::optional< sf::Color > colour = std::nullopt );
		explicit Sprite( const Reflex::Object& owner, const sf::Texture& texture, const sf::IntRect& rectangle, const std::optional< sf::Color > colour = std::nullopt );
		explicit Sprite( const Reflex::Object& owner, const std::optional< sf::Color > colour = std::nullopt );

		static std::string GetComponentName() { return "Sprite"; }
		bool SetValue( const std::string& variable, const std::string& value ) override;
		void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const override;
		bool IsRenderComponent() const final { return true; }
		void Render( sf::RenderTarget& target, sf::RenderStates states ) const final { target.draw( *this, states ); }
	};

	class Text : public Component< Text >, public sf::Text
	{
	public:
		explicit Text( const Reflex::Object& owner, const sf::String& string, const sf::Font& font, const unsigned characterSize = 30, const std::optional< sf::Color > colour = std::nullopt );
		explicit Text( const Reflex::Object& owner, const std::optional< sf::Color > colour = std::nullopt );

		static std::string GetComponentName() { return "Text"; }
		bool SetValue( const std::string& variable, const std::string& value ) override;
		void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const override;
		bool IsRenderComponent() const final { return true; }
		void Render( sf::RenderTarget& target, sf::RenderStates states ) const final { target.draw( *this, states ); }
	};

	template< typename V >
	void GetColourValues( std::vector< std::pair< std::string, std::string > >& values, const V& shape )
	{
		if( !Reflex::IsDefault( shape.getFillColor() ) )
			values.emplace_back( "FillColour", Reflex::ToString( shape.getFillColor() ) );
		if( !Reflex::IsDefault( shape.getOutlineColor() ) )
			values.emplace_back( "OutlineColour", Reflex::ToString( shape.getOutlineColor() ) );
	}

	template< typename V >
	void GetShapeValues( std::vector< std::pair< std::string, std::string > >& values, const V& shape )
	{
		values.emplace_back( "PointCount", Reflex::ToString( shape.getPointCount() ) );
		values.emplace_back( "Points", "" );
		for( size_t i = 0; i < shape.getPointCount(); ++i )
			values.back().second += ( i > 0 ? ", " : "" ) + Reflex::ToString( shape.getPoint( i ) );
	}

}