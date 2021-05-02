#include "Precompiled.h"
#include "World.h"
#include "InteractableSystem.h"
#include "InteractableComponent.h"
#include "SFMLObjectComponent.h"

using namespace Reflex::Components;

namespace Reflex::Systems
{
	void InteractableSystem::RegisterComponents()
	{
		RequiresComponent( Transform );
		RequiresComponent( Interactable );
	}

	bool InteractableSystem::CheckCollision( const Reflex::Components::Transform::Handle& transform, const sf::FloatRect& localBounds, const sf::Vector2f& mousePosition ) const
	{
		sf::Transform transformFinal;
		transformFinal.scale( transform->GetWorldScale() ).translate( transform->GetWorldTranslation() );
		auto globalBounds = transformFinal.transformRect( localBounds );
		globalBounds.left -= localBounds.width / 2.0f;
		globalBounds.top -= localBounds.height / 2.0f;
		return Reflex::BoundingBox( globalBounds, transform->GetWorldRotation() ).contains( mousePosition );
	}

	void InteractableSystem::Update( const float deltaTime )
	{
		PROFILE;
		const auto& window = GetWorld().GetWindow();
		const auto mousePosition = window.mapPixelToCoords( sf::Mouse::getPosition( window ) );

		ForEachObject< Transform, Interactable >(
			[&]( const Transform::Handle& transform, const Interactable::Handle& interactable)
			{
			auto* ptr = interactable.Get();
			//auto collisionObj = sfmlObj;
			//
			//if( interactable->m_replaceCollisionObject )
			//	collisionObj = interactable->m_replaceCollisionObject.GetComponent< SFMLObject >();
			//
			bool collision = false;
			//
			//// Collision with bounds
			//switch( collisionObj->GetType() )
			//{
			//case SFMLObjectType::Circle:
			//	collision = Reflex::Circle( transform->GetWorldPosition(), collisionObj->GetCircleShape().getRadius() ).Contains( mousePosition );
			//	break;
			//case SFMLObjectType::Rectangle:
			//	collision = CheckCollision( transform, collisionObj->GetRectangleShape().getLocalBounds(), mousePosition );
			//	break;
			//case SFMLObjectType::Convex:
			//	collision = CheckCollision( transform, collisionObj->GetConvexShape().getLocalBounds(), mousePosition );
			//	break;
			//case SFMLObjectType::Sprite:
			//	collision = CheckCollision( transform, collisionObj->GetSprite().getLocalBounds(), mousePosition );
			//	break;
			//case SFMLObjectType::Text:
			//	collision = CheckCollision( transform, collisionObj->GetText().getLocalBounds(), mousePosition );
			//	break;
			//}

				// Focus / highlighting
				if( ptr->isFocussed != collision )
				{
					ptr->isFocussed = collision;

					if( !collision && ptr->focusChangedCallback )
						ptr->focusChangedCallback( *ptr, false );
					else if( collision && ptr->focusChangedCallback )
						ptr->focusChangedCallback( *ptr, true );

					// Lost highlight, then we also unselect
					if( !collision && !ptr->selectionIsToggle && ptr->unselectIfLostFocus )
						ptr->Deselect();
				}

				// Selection (or can be deselection for toggle mode)
				if( ptr->isFocussed && m_mousePressed )
				{
					ptr->isSelected&& ptr->selectionIsToggle ? ptr->Deselect() : ptr->Select();
					m_mousePressed = false;
				}

				// Un-selection
				if( m_mouseReleased && !ptr->selectionIsToggle )
					ptr->Deselect();
			} );

		m_mouseReleased = false;
	}

	void InteractableSystem::ProcessEvent( const sf::Event& event )
	{
		if( event.type == sf::Event::MouseButtonPressed )
		{
			m_mousePressed = true;
		}
		else if( event.type == sf::Event::MouseButtonReleased )
		{
			m_mousePressed = false;
			m_mouseReleased = true;
		}
	}
}