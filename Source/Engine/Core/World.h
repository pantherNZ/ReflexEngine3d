#pragma once

#include "Precompiled.h"
#include "ResourceManager.h"
#include "Context.h"
#include "Systems/BaseSystem.h"
#include "Memory/ComponentAllocator.h"
#include "EventManager.h"
#include "TileMap.h"
#include "Objects/BaseObject.h"
#include "Components/Component.h"
#include "Box2DDebugDraw.h"

// Engine class
namespace Reflex 
{
	class Object;

	namespace Components
	{
		class Transform;
		class Camera;
	}
}

namespace Reflex::Core
{
	// World class
	class World : private sf::NonCopyable
	{
	public:
		explicit World( const Context& context, const sf::FloatRect& worldBounds, const sf::Vector2f& gravity = sf::Vector2f( 0.0f, 9.8f ) );
		~World();

		void Update( const float deltaTime );
		void ProcessEvent( const sf::Event& event );
		void Render();

		/* Object functions*/
		void CreateROFile( const std::string& name, const Object& object );
		Object CreateObject( const sf::Vector2f& position = {}, const float rotation = 0.0f, const sf::Vector2f& scale = sf::Vector2f( 1.0f, 1.0f ), const bool attachToRoot = true, const bool useTileMap = true );
		Object CreateObject( const std::string& objectFile, const sf::Vector2f& position = {}, const float rotation = 0.0f, const sf::Vector2f& scale = sf::Vector2f( 1.0f, 1.0f ), const bool attachToRoot = true, const bool useTileMap = true );

		void DestroyObject( const BaseObject& object );
		void DestroyAllObjects();

		bool IsValidObject( const BaseObject& object ) const;
		bool IsObjectFlagSet( const BaseObject& object, const ObjectFlags flag ) const;
		void SetObjectFlag( const BaseObject& object, const ObjectFlags flag );
		/*---------------*/

		/* Component functions*/
		template< class T, typename... Args >
		T* ObjectAddComponent( const BaseObject& object, Args&& ... args );

		// Adds an empty component (no args passed, non templated) - Used to add components from .ro files
		// The allocator must be already created (as in a component of the require type must already be registered or exist)
		Reflex::Components::BaseComponent* ObjectAddEmptyComponent( const Object& object, const size_t family );

		template< class T >
		T* ObjectGetComponent( const BaseObject& object ) const;

		Reflex::Components::BaseComponent* ObjectGetComponent( const BaseObject& object, const size_t family ) const;

		ComponentsMask ObjectGetComponentFlags( const BaseObject& object ) const;

		// Returns true if a component was removed
		template< class T >
		bool ObjectRemoveComponent( const BaseObject& object );

		bool ObjectRemoveComponent( const BaseObject& object, const size_t family );
		void ObjectRemoveAllComponents( const BaseObject& object );

		template< class T >
		bool ObjectHasComponent( const BaseObject& object ) const;

		bool ObjectHasComponent( const BaseObject& object, const size_t family ) const;

		// Returns true if the register resulted in a new component being allocated
		template< class T >
		bool RegisterComponent();
		/*---------------*/

		/* System functions*/
		template< class T, typename... Args >
		T* AddSystem( Args&& ... args );

		template< class T >
		T* GetSystem();

		template< class T >
		void RemoveSystem();
		/*---------------*/

		// Utility and helper functions
		float GetDeltaTime() const { return m_deltaTime; }

		sf::RenderWindow& GetWindow() { return m_context.window; }
		const sf::RenderWindow& GetWindow() const { return m_context.window; }
		TextureManager& GetTextureManager() { return m_context.textureManager; }
		FontManager& GetFontManager() { return m_context.fontManager; }
		EventManager& GetEventManager() { return eventManager; }
		TileMap& GetTileMap() { return m_tileMap; }
		const TileMap& GetTileMap() const { return m_tileMap; }
		b2World& GetBox2DWorld() { return *m_box2DWorld; }
		const b2World& GetBox2DWorld() const { return *m_box2DWorld; }

		sf::FloatRect GetBounds() const;
		Reflex::Handle< Reflex::Components::Transform > GetSceneRoot() const;

		void OnComponentAdded( const BaseObject& object );
		void OnComponentRemoved( const BaseObject& object );

		bool IsActiveCamera( const Reflex::Handle< Reflex::Components::Camera >& camera ) const;
		void SetActiveCamera( const Reflex::Handle< Reflex::Components::Camera >& camera );
		Reflex::Handle< Reflex::Components::Camera > GetActiveCamera() const;

		sf::Vector2f GetMousePosition() const;
		sf::Vector2f GetMousePosition( const Reflex::Handle< Reflex::Components::Camera >& camera ) const;
		sf::Vector2f RandomWindowPosition( const float margin = 0.0f ) const;
		sf::Vector2f GetWindowCentre() const { return Reflex::Vector2iToVector2f( GetWindow().getPosition() ) + GetWindowSize() / 2.0f; }
		sf::Vector2f GetWindowSize() const { return Reflex::Vector2uToVector2f( GetWindow().getSize() ); }

		std::vector< Object > GetObjects();

		float GetBox2DUnitToPixelScale() const { return m_box2DUnitToPixelScale; }
		float ToBox2DUnits( const float worldUnits ) const { return worldUnits / m_box2DUnitToPixelScale; }
		float ToWorldUnits( const float b2Units ) const { return b2Units * m_box2DUnitToPixelScale; }

		struct RayCastResult 
		{
			RayCastResult() { }
			RayCastResult( Object object, b2Fixture* fixture, const sf::Vector2f& point, const sf::Vector2f& normal, const float fraction );
			bool hit = false;
			b2Fixture* fixture = nullptr;
			const sf::Vector2f point;
			const sf::Vector2f normal;
			float fraction = 0.0f;
			operator bool() const;
			Object GetObject() const;

		private:
			BaseObject object;
		};

		RayCastResult RayCast( const sf::Vector2f& from, const sf::Vector2f& to );

	protected:
		void Setup();
		Object ObjectFromIndex( const unsigned index );

		bool IsObjectFlagSet( const std::uint32_t objectIndex, const ObjectFlags flag ) const;
		void SetObjectFlag( const std::uint32_t objectIndex, const ObjectFlags flag );

	private:
		World() = delete;

	protected:
		Context m_context;
		sf::View m_worldView;
		sf::FloatRect m_worldBounds;

		float m_deltaTime = 0.0f;

		// Box2d world, allocated on the heap because the b2World class is huge (103kb)
		std::unique_ptr< b2World > m_box2DWorld;
		float m_box2DUnitToPixelScale = 32;
		Box2DDebugDraw m_box2DDebugDraw;
		bool m_box2DUseDebugDraw = true;
		int m_box2DVelocityIterations = 8;
		int m_box2DPositionIterations = 3;

		// Handler for event system
		EventManager eventManager;

		// Tilemap which stores object handles in the world in an efficient spacial hash map
		TileMap m_tileMap;

		// Object data
		struct ObjectData
		{
			// Separate vectors are more efficient for fast lookup for individual data (less data to pull into cache), we rarely need info from more than 1 at the same time
			std::vector< std::bitset< ( size_t )ObjectFlags::NumFlags > > flags;
			std::vector< ComponentsMask > components;
			std::vector< unsigned > counters;
		};

		ObjectData m_objects;

		// Storage for all components
		std::vector< std::unique_ptr< ComponentAllocatorBase > > m_components;
		std::unordered_map< std::string, size_t > m_componentNameToIndex;
		std::queue< unsigned > m_freeList;

		// List of systems, indexed by their type, storage for all systems
		std::unordered_map< Type, std::unique_ptr< Reflex::Systems::BaseSystem > > m_systems;

		BaseObject m_sceneGraphRoot;
		BaseObject m_activeCamera;
	};

	// Template functions
	template< class T, typename... Args >
	T* World::ObjectAddComponent( const BaseObject& object, Args&& ... args )
	{
		const auto family = T::GetFamily();

		// Can't add multiple of the same component
		if( m_objects.components[object.GetIndex()].test( family ) )
		{
			LOG_CRIT( "You cannot add multiple of the same component!" );
			return {};
		}

		// Create allocator for this component type if it is new (usually this is done via the RegisterComponent< T > function)
		if( family >= m_components.size() )
		{
			assert( family == m_components.size() );
			m_components.push_back( std::unique_ptr< ComponentAllocatorBase >( new ComponentAllocator< T >( std::max( 128U, object.GetIndex() ) ) ) );
		}
		else
			m_components[family]->ExpandToFit( object.GetIndex() + 1 );

		// Allocate memory and construct, passing args through
		auto* allocator = static_cast< ComponentAllocator< T >* >( m_components[family].get() );
		auto newComponent = static_cast< T* >( allocator->Construct( object.GetIndex(), object, std::forward<Args>( args )... ) );
		m_objects.components[object.GetIndex()].set( family );

		const auto requiredComponents = T::GetRequiredComponents();

		// Automatically add required components
		for( unsigned i = 0; i < MaxComponents; ++i )
			if( requiredComponents.test( i ) && !m_objects.components[object.GetIndex()].test( i ) )
				ObjectAddEmptyComponent( object, i );

		newComponent->OnConstructionComplete();
		OnComponentAdded( object );

		return newComponent;
	}

	template< class T >
	T* World::ObjectGetComponent( const BaseObject& object ) const
	{
		return static_cast< T* >( ObjectGetComponent( object, ( size_t )T::GetFamily() ) );
	}

	template< class T >
	bool World::ObjectRemoveComponent( const BaseObject& object )
	{
		return ObjectRemoveComponent( object, ( size_t )T::GetFamily() );
	}

	template< class T >
	bool World::ObjectHasComponent( const BaseObject& object ) const
	{
		return ObjectHasComponent( object, ( size_t )T::GetFamily() );
	}

	template< class T >
	bool World::RegisterComponent()
	{
		const auto family = T::GetFamily();

		if( family >= m_components.size() )
		{
			assert( family == m_components.size() );
			m_componentNameToIndex[T::GetComponentName()] = m_components.size();
			m_components.push_back( std::unique_ptr< ComponentAllocatorBase >( new ComponentAllocator< T >( 128 ) ) );
			return true;
		}
		return false;
	}

	template< class T, typename... Args >
	T* World::AddSystem( Args&& ... args )
	{
		const auto type = Type( typeid( T ) );

		if( m_systems.find( type ) != m_systems.end() )
		{
			LOG_CRIT( "Trying to add a system that has already been added!" );
			return nullptr;
		}

		auto system = std::make_unique< T >( *this, std::forward< Args >( args )... );

		// Register components
		system->RegisterComponents();

		// Look for any existing objects that match what this system requires and add them to the system's list
		for( unsigned i = 0; i < m_objects.components.size(); ++i )
		{		
			if( IsObjectFlagSet( i, ObjectFlags::Deleted ) )
				continue;

			if( !system->ShouldAddObject( ObjectFromIndex( i ) ) )
				continue;

			system->AddComponent( ObjectFromIndex( i ) );
			system->OnComponentAdded( ObjectFromIndex( i ) );
		}

		auto result = m_systems.insert( std::make_pair( type, std::move( system ) ) );
		assert( result.second );

		result.first->second->OnSystemStartup();

		return ( T* )result.first->second.get();
	}

	template< class T >
	void World::RemoveSystem()
	{
		const auto systemType = Type( typeid( T ) );

		for( auto iter = m_systems.begin(); iter != m_systems.end(); ++iter )
		{
			if( systemType == iter->first )
			{
				iter->second->OnSystemShutdown();
				iter->second.release();
				m_systems.erase( iter );
				break;
			}
		}
	}

	template< class T >
	T* World::GetSystem()
	{
		const auto systemType = Type( typeid( T ) );

		for( auto iter = m_systems.begin(); iter != m_systems.end(); ++iter )
			if( systemType == iter->first )
				return ( T* )iter->second.get();

		return nullptr;
	}

	//template< typename Func >
	//void World::ForEachObject( Func function )
	//{
	//	for( unsigned i = 1; i < m_objects.counters.size(); ++i )
	//		function( Reflex::Object( *this, i, m_objects.counters[i] ) );
	//}
}