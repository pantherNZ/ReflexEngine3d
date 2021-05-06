#pragma once

#include "Components/Component.h"
#include "Core/Utility.h"
#include "Systems/2D/InteractableSystem.h"
#include "Components/2D/SFMLObjectComponent.h"

namespace Reflex::Components
{
	// Class definition
	class Interactable : public Component< Interactable >
	{
	public:
		friend class Reflex::Systems::InteractableSystem;

		Interactable( const Reflex::Object& owner, const Reflex::Object& collisionObjectOverride = Reflex::Object() );

		bool SetValue( const std::string& variable, const std::string& value ) override;
		void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const override;
		static std::string GetComponentName() { return "Interactable"; }

		// Settings, change as you want
		bool selectionIsToggle = true;
		bool unselectIfLostFocus = false;
		bool isEnabled = true;

		// Callbacks
		std::function< void( const Interactable&, const bool ) > focusChangedCallback;
		std::function< void( const Interactable&, const bool ) > selectionChangedCallback;

		bool IsFocussed() const;
		bool IsSelected() const;

	protected:
		void Select();
		void Deselect();

		bool isFocussed = false;
		bool isSelected = false;

		Object m_replaceCollisionObject;
	};
}