#include "Precompiled.h"
#include "SceneNode.h"
#include "Object.h"
#include "TransformComponent.h"
#include "Logging.h"

namespace Reflex::Core
{
	SceneNode::SceneNode( const Reflex::Object& owner )
		: m_owningObject( owner )
		, m_parent()
		, m_children()
	{

	}

	SceneNode::SceneNode( const SceneNode& other )
		: m_owningObject( other.m_owningObject )
		, m_parent( other.m_parent )
		, m_children( other.m_children )
	{

	}

	SceneNode::~SceneNode()
	{
		if( m_parent )
			m_parent.GetTransform()->DetachChild( GetObject() );
	}

	void SceneNode::AttachChild( const Reflex::Object& child )
	{
		auto transform = child.GetTransform();

		if( transform->m_parent )
			transform->m_parent.GetTransform()->DetachChild( child );

		transform->m_parent = GetObject();
		transform->IncrementZOrder();
		transform->SetLayer( GetObject().GetTransform()->GetLayer() + 1 );
		m_children.push_back( child );
	}

	Reflex::Object SceneNode::DetachChild( const Reflex::Object& node )
	{
		// Find and detach the node'
		for( unsigned i = 0U; i < m_children.size(); ++i )
		{
			if( node == m_children[i] )
			{
				if( m_children[i] )
					if( const auto transform = m_children[i].GetTransform() )
						transform->m_parent = Reflex::Object();

				m_children.erase( m_children.begin() + i );
				return node;
			}
		}

		LOG_CRIT( "Node not found" );
		return Reflex::Object();
	}

	sf::Transform SceneNode::GetWorldTransform() const
	{
		TODO( "Cache the world transform" );
		sf::Transform worldTransform;

		for( auto node = GetObject(); node.IsValid(); node = node.GetTransform()->m_parent )
			worldTransform = node.GetTransform()->getTransform() * worldTransform;

		return worldTransform;
	}

	sf::Vector2f SceneNode::GetWorldPosition() const
	{
		return GetWorldTransform() * sf::Vector2f();
	}

	sf::Vector2f SceneNode::GetWorldTranslation() const
	{
		sf::Vector2f worldTranslation;

		for( auto node = GetObject(); node.IsValid(); node = node.GetTransform()->m_parent )
		{
			worldTranslation.x += node.GetTransform()->getPosition().x;
			worldTranslation.y += node.GetTransform()->getPosition().y;
		}

		return worldTranslation;
	}

	float SceneNode::GetWorldRotation() const
	{
		float worldRotation = 0.0f;

		for( auto node = GetObject(); node.IsValid(); node = node.GetTransform()->m_parent )
			worldRotation += node.GetTransform()->getRotation();

		return worldRotation;
	}

	sf::Vector2f SceneNode::GetWorldScale() const
	{
		sf::Vector2f worldScale( 1.0f, 1.0f );

		for( auto node = GetObject(); node.IsValid(); node = node.GetTransform()->m_parent )
		{
			worldScale.x *= node.GetTransform()->getScale().x;
			worldScale.y *= node.GetTransform()->getScale().y;
		}

		return worldScale;
	}

	unsigned SceneNode::GetChildrenCount() const
	{
		return ( unsigned )m_children.size();
	}

	Reflex::Object SceneNode::GetChild( const unsigned index ) const
	{
		if( index >= GetChildrenCount() )
		{
			LOG_CRIT( "Tried to get a child with an invalid index: " + index );
			return Reflex::Object();
		}

		return m_children[index];
	}

	Reflex::Object SceneNode::GetParent() const
	{
		return m_parent;
	}

	Reflex::Object SceneNode::GetObject() const
	{
		return m_owningObject;
	}

}