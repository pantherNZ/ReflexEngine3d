#pragma once

#include "Components/Component.h"
#include "Systems/2D/CameraSystem.h"
#include "Objects/Object.h"

namespace Reflex::Components
{
	class Camera : public Component< Camera >, public sf::View
	{
	public:
		friend class Reflex::Systems::CameraSystem;

		Camera( const Reflex::Object& owner );
		Camera( const Reflex::Object& owner, const sf::Vector2f& centre, const sf::Vector2f& size, const bool activate = true );
		Camera( const Reflex::Object& owner, const sf::FloatRect& viewRect );
		~Camera();

		bool SetValue( const std::string& variable, const std::string& value ) override;
		void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const override;
		static std::string GetComponentName() { return "Camera"; }
		void OnConstructionComplete() final;

		bool IsActiveCamera() const;
		void SetActiveCamera();

		//void ApplyShake( const float force );
		void FollowObject( const Reflex::Object& object, const float interpolationSpeed = 0.0f );
		bool IsFollowingObject() const { return followTarget.IsValid(); }

		void EnableMousePanning( const sf::Vector2f& margin, const sf::Vector2f& speed = sf::Vector2f( 0.0f, 0.0f ) );
		void DisableMousePanning() { flags.reset( MousePanning ); }

		void EnableMouseZooming( const float scaleFactor = 0.0f, const bool centreOnMouse = false );
		void DisableMouseZooming() { flags.reset( MouseZooming ); }

		void EnableWASDPanning( const sf::Vector2f& speed = sf::Vector2f( 0.0f, 0.0f ) );
		void DisableWASDPanning() { flags.reset( WASDPanning ); }

		void EnableArrowKeyPanning( const sf::Vector2f& speed = sf::Vector2f( 0.0f, 0.0f ) );
		void DisableArrowKeyPanning() { flags.reset( ArrowPanning ); }

		void SetPanSpeed( const sf::Vector2f& speed ) { panSpeed = speed; }
		sf::Vector2f GetPanSpeed() const { return panSpeed; }
		void SetZoomScaleFactor( const float speed ) { zoomScaleFactor = speed; }
		float GetZoomScaleFactor() const { return zoomScaleFactor; }

		void SetAdditivePanning( const bool additive ) { flags[AdditivePanning] = additive; }
		void SetNormaliseDiagonalPanning( const bool normalise ) { flags[NormaliseDiagonalPanning] = normalise; }

	protected:
		enum Flags
		{
			MousePanning,
			WASDPanning,
			ArrowPanning,
			MouseZooming,
			ZoomCentreOnMouse,
			AdditivePanning,
			NormaliseDiagonalPanning,
			StartActivated,
			NumFlags,
		};

		std::bitset< NumFlags > flags;
		float zoomScaleFactor = 1.0f;
		sf::Vector2f panSpeed;
		sf::Vector2f panMouseMargin;
		float followInterpSpeed = 0.0f;
		Reflex::Object followTarget;
	};
}