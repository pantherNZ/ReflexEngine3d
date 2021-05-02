#pragma once

namespace Reflex::Core { class World; }
namespace Reflex { class Object; }

namespace Reflex::Systems
{
	// Abstract class for internal use. Custom systems should inherit from System
	class BaseSystem : private sf::NonCopyable, public sf::Drawable
	{
	public:
		friend class Reflex::Core::World;

		BaseSystem( Reflex::Core::World& world ) : m_world( world ) { }
		virtual ~BaseSystem() { }

		Reflex::Core::World& GetWorld() { return m_world; }
		const Reflex::Core::World& GetWorld() const { return m_world; }
		ComponentsMask GetRequiredComponents() const { return m_requiredComponents; }

	protected:
		virtual void RegisterComponents() = 0;
		virtual void Update( const float deltaTime ) { }
		virtual void ProcessEvent( const sf::Event& event ) { }
		virtual void Render( sf::RenderTarget& target, sf::RenderStates states ) const { }
		virtual void RenderUI() { }

		virtual void OnSystemStartup() { }
		virtual void OnSystemShutdown() { }

		virtual bool ShouldAddObject( const Reflex::Object& object ) const = 0;
		virtual void AddComponent( const Reflex::Object& object ) = 0;
		virtual void OnComponentAdded( const Reflex::Object& object ) { }
		virtual void OnComponentRemoved( const Reflex::Object& object ) { }

	private:
		void draw( sf::RenderTarget& target, sf::RenderStates states ) const final { Render( target, states ); }

	protected:
		ComponentsMask m_requiredComponents;

	private:
		Reflex::Core::World& m_world;
	};
}