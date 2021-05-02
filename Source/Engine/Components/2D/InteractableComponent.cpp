#include "Precompiled.h"
#include "InteractableComponent.h"
#include "SFMLObjectComponent.h"
#include "Object.h"

namespace Reflex::Components
{
	Interactable::Interactable( const Reflex::Object& owner, const Reflex::Object& collisionObjectOverride )
		: Component< Interactable >( owner )
		, m_replaceCollisionObject( collisionObjectOverride )
	{

	}

	bool Interactable::SetValue( const std::string& variable, const std::string& value )
	{
		if( variable == "SelectionIsToggle" )
		{
			selectionIsToggle = Reflex::FromString< bool >( value );
			return true;
		}
		else if( variable == "UnselectIfLostFocus" )
		{
			unselectIfLostFocus = Reflex::FromString< bool >( value );
			return true;
		}
		else if( variable == "IsEnabled" )
		{
			isEnabled = Reflex::FromString< bool >( value );
			return true;
		}

		return false;
	}

	void Interactable::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		if( selectionIsToggle )
			values.emplace_back( "SelectionIsToggle", "true" );
		if( unselectIfLostFocus )
			values.emplace_back( "UnselectIfLostFocus", "true" );
		if( isEnabled )
			values.emplace_back( "IsEnabled", "true" );
	}

	bool Interactable::IsFocussed() const
	{
		return isFocussed;
	}

	bool Interactable::IsSelected() const
	{
		return isSelected;
	}

	void Interactable::Select()
	{
		if( !isSelected && isEnabled )
		{
			isSelected = true;

			if( selectionChangedCallback )
				selectionChangedCallback( *this, true );
		}
	}

	void Interactable::Deselect()
	{
		if( isSelected )
		{
			isSelected = false;

			if( selectionChangedCallback )
				selectionChangedCallback( *this, false );
		}
	}
}