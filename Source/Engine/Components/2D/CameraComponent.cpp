#include "CameraComponent.h"
#include "TransformComponent.h"
#include "Core/World.h"

namespace Reflex::Components
{
	Camera::Camera( const Reflex::Object& owner )
		: Component< Camera >( owner )
	{

	}

	Camera::Camera( const Reflex::Object& owner, const sf::Vector2f& centre, const sf::Vector2f& size, const bool activate )
		: Component< Camera >( owner )
		, sf::View( centre, size )
	{
		flags.set( StartActivated, activate );
		flags.set( AdditivePanning );
		flags.set( NormaliseDiagonalPanning );
	}

	Camera::Camera( const Reflex::Object& owner, const sf::FloatRect& viewRect )
		: Component< Camera >( owner )
		, sf::View( viewRect )
	{
	}

	Camera::~Camera()
	{
	}

	namespace
	{
		std::string flagNames[] =
		{
			"MousePanning",
			"WASDPanning",
			"ArrowPanning",
			"MouseZooming",
			"ZoomCentreOnMouse",
			"AdditivePanning",
			"NormaliseDiagonalPanning",
			"StartActivated",
		};
	}

	bool Camera::SetValue( const std::string& variable, const std::string& value )
	{
		static_assert( std::size( flagNames ) == ( size_t )NumFlags );

		for( unsigned i = 0; i < std::size( flagNames ); ++i )
		{
			if( variable == flagNames[i] )
			{
				flags.set( i, Reflex::FromString< bool >( value ) );
				return true;
			}
		}

		if( variable == "PanSpeed" )
		{
			panSpeed = Reflex::FromString< sf::Vector2f >( value );
			return true;
		}
		else if( variable == "PanMouseMargin" )
		{
			panMouseMargin = Reflex::FromString< sf::Vector2f >( value );
			return true;
		}
		else if( variable == "FollowInterpSpeed" )
		{
			followInterpSpeed = Reflex::FromString< float >( value );
			return true;
		}
		else if( variable == "ZoomScaleFactor" )
		{
			zoomScaleFactor = Reflex::FromString< float >( value );
			return true;
		}

		return false;
	}

	void Camera::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		for( unsigned i = 0; i < std::size( flagNames ); ++i )
			if( flags.test( i ) )
				values.emplace_back( flagNames[i], "true" );

		if( !Reflex::IsDefault( panSpeed ) )
			values.emplace_back( "PanSpeed", Reflex::ToString( panSpeed ) );
		if( !Reflex::IsDefault( panMouseMargin ) )
			values.emplace_back( "PanMouseMargin", Reflex::ToString( panMouseMargin ) );
		if( followInterpSpeed )
			values.emplace_back( "FollowInterpSpeed", Reflex::ToString( followInterpSpeed ) );
		if( zoomScaleFactor )
			values.emplace_back( "ZoomScaleFactor", Reflex::ToString( zoomScaleFactor ) );
	}

	void Camera::OnConstructionComplete()
	{
		if( flags.test( StartActivated ) || !GetObject().GetWorld().GetActiveCamera().IsValid() )
			SetActiveCamera();

		GetObject().GetTransform()->setPosition( getCenter() );
	}

	void Camera::FollowObject( const Reflex::Object& object, const float interpolationSpeed )
	{
		followTarget = object;
		followInterpSpeed = interpolationSpeed;
	}

	bool Camera::IsActiveCamera() const
	{
		return GetObject().GetWorld().GetActiveCamera().object == GetObject();
	}

	void Camera::SetActiveCamera()
	{
		GetObject().GetWorld().SetActiveCamera( GetHandle() );
	}

	void Camera::EnableMousePanning( const sf::Vector2f& margin, const sf::Vector2f& speed )
	{
		flags.set( MousePanning );
		panMouseMargin = margin;

		if( speed.x != 0.0f && speed.y != 0.0f )
			panSpeed = speed;
	}

	void Camera::EnableMouseZooming( const float scaleFactor, const bool centreOnMouse )
	{
		flags.set( MouseZooming );
		flags.set( ZoomCentreOnMouse, centreOnMouse );
		
		if( scaleFactor != 0.0f )
			zoomScaleFactor = scaleFactor;
	}

	void Camera::EnableWASDPanning( const sf::Vector2f& speed )
	{
		flags.set( WASDPanning );

		if( speed.x != 0.0f && speed.y != 0.0f )
			panSpeed = speed;
	}

	void Camera::EnableArrowKeyPanning( const sf::Vector2f& speed )
	{
		flags.set( ArrowPanning );
		
		if( speed.x != 0.0f && speed.y != 0.0f )
			panSpeed = speed;
	}
}
