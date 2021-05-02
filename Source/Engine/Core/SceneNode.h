#pragma once

#include "Precompiled.h"
#include "Object.h"

namespace Reflex::Core
{
	class SceneNode : public sf::Transformable
	{
	public:
		SceneNode( const Reflex::Object& owner );
		SceneNode( const SceneNode& other );
		~SceneNode();

		void AttachChild( const Reflex::Object& child );
		Reflex::Object DetachChild( const Reflex::Object& node );

		sf::Transform GetWorldTransform() const;
		sf::Vector2f GetWorldPosition() const; 

		sf::Vector2f GetWorldTranslation() const;
		float GetWorldRotation() const;
		sf::Vector2f GetWorldScale() const;

		template< typename Func >
		void ForEachChild( Func function )
		{
			std::for_each( m_children.begin(), m_children.end(), function );
		}

		unsigned GetChildrenCount() const;
		Reflex::Object GetChild( const unsigned index ) const;
		Reflex::Object GetParent() const;
		Reflex::Object GetObject() const;

	protected:
		Reflex::Object m_owningObject;
		Reflex::Object m_parent;
		std::vector< Reflex::Object > m_children;
	};
}