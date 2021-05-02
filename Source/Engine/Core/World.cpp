#include "Precompiled.h"

namespace Reflex::Core
{
	World::World( const Context& context, const sf::FloatRect& worldBounds, const sf::Vector2f& gravity )
		: m_context( context )
		, m_worldView( context.window.getDefaultView() )
		, m_worldBounds( worldBounds )
		, m_tileMap( 200, 20 )
		, m_box2DWorld( std::make_unique< b2World >( b2Vec2( gravity.x, gravity.y ) ) )
		, m_box2DDebugDraw( context.window, m_box2DUnitToPixelScale )
	{
		Reflex::box2DUnitToPixelScale = m_box2DUnitToPixelScale;
		Setup();
	}

	World::~World()
	{
		DestroyAllObjects();
	}

	void World::Setup()
	{
		RegisterComponent< Reflex::Components::Transform >();
		RegisterComponent< Reflex::Components::Interactable >();
		RegisterComponent< Reflex::Components::CircleShape >();
		RegisterComponent< Reflex::Components::RectangleShape >();
		RegisterComponent< Reflex::Components::ConvexShape >();
		RegisterComponent< Reflex::Components::Sprite >();
		RegisterComponent< Reflex::Components::Text >();
		RegisterComponent< Reflex::Components::Grid >();
		RegisterComponent< Reflex::Components::Camera >();
		RegisterComponent< Reflex::Components::Steering >();
		RegisterComponent< Reflex::Components::RigidBody >();
		RegisterComponent< Reflex::Components::CircleCollider >();

		AddSystem< Reflex::Systems::RenderSystem >();
		AddSystem< Reflex::Systems::InteractableSystem >();
		AddSystem< Reflex::Systems::MovementSystem >();
		AddSystem< Reflex::Systems::CameraSystem >();
		AddSystem< Reflex::Systems::SteeringSystem >();
		AddSystem< Reflex::Systems::PhysicsSystem >();

		m_sceneGraphRoot = CreateObject( sf::Vector2f( 0.0f, 0.0f ), 0.0f, sf::Vector2f( 1.0f, 1.0f ), false, false );

		m_box2DDebugDraw.SetFlags( -1 );
		m_box2DWorld->SetDebugDraw( &m_box2DDebugDraw );
		//m_box2DWorld.SetDestructionListener( &m_destructionListener );
		//m_box2DWorld.SetContactListener( this );
	}

	void World::Update( const float deltaTime )
	{
		PROFILE;
		m_deltaTime = deltaTime;
		m_box2DWorld->Step( deltaTime, m_box2DVelocityIterations, m_box2DPositionIterations );

		// Update systems
		for( auto& system : m_systems )
			system.second->Update( deltaTime );
	}

	void World::ProcessEvent( const sf::Event& event )
	{
		PROFILE;
		for( auto& system : m_systems )
			system.second->ProcessEvent( event );
	}

	void World::Render()
	{
		PROFILE;
		const auto camera = GetActiveCamera();
		GetWindow().setView( camera ? *camera : m_worldView );

		for( auto& system : m_systems )
		{
			system.second->RenderUI();
			GetWindow().draw( *system.second );
		}

		m_box2DWorld->DebugDraw();

		ImGui::Begin( "World Info" );
		
		if( ImGui::Checkbox( "Box2D Debug Draw", &m_box2DUseDebugDraw ) )
			m_box2DWorld->SetDebugDraw( m_box2DUseDebugDraw ? &m_box2DDebugDraw : nullptr );
		ImGui::InputInt( "Box2D Position Iterations", &m_box2DPositionIterations );
		ImGui::InputInt( "Box2D Velocity Iterations", &m_box2DVelocityIterations );

		ImGui::End();
	}

	Object World::CreateObject( const sf::Vector2f& position, const float rotation, const sf::Vector2f& scale, const bool attachToRoot /*= true*/, const bool useTileMap /*= true*/ )
	{
		unsigned index = 0;

		if( !m_freeList.empty() )
		{
			index = m_freeList.back();
			m_objects.flags[index].reset();
			m_freeList.pop();
		}
		else
		{
			m_objects.components.emplace_back();
			m_objects.flags.emplace_back();
			m_objects.counters.emplace_back();
			index = ( unsigned )m_objects.components.size() - 1;

			for( auto& allocator : m_components )
				allocator->ExpandToFit( index + 1 );
		}

		Object newObject = ObjectFromIndex( index );
		const auto transform = newObject.AddComponent< Reflex::Components::Transform >( position, rotation, scale, useTileMap );

		if( attachToRoot )
		{
			assert( GetSceneRoot().IsValid() );
			GetSceneRoot()->AttachChild( newObject );
		}

		SetObjectFlag( newObject, ObjectFlags::ConstructionComplete );

		return newObject;
	}

	Object World::CreateObject( const std::string& objectFile, const sf::Vector2f& position, const float rotation, const sf::Vector2f& scale, const bool attachToRoot /*= true*/, const bool useTileMap /*= true*/ )
	{
		auto newObject = CreateObject( position, rotation, scale, attachToRoot, useTileMap );
		
		const auto dot = objectFile.rfind( '.' );

		if( dot == std::string::npos )
			THROW( "Invalid object file name: " << objectFile );

		const auto fileType = objectFile.substr( dot );

		if( fileType.substr( 0, 3 ) != ".ro" )
			THROW( "Invalid object file name (must be .ro): " << objectFile );

		//if( fileType.length() > 3 )
		//	const auto version = std::atoi( &*fileType.rbegin() );

		std::ifstream input( objectFile );

		if( input.fail() )
			THROW( "Invalid object file name: " << objectFile );

		Json::CharReaderBuilder reader;
		Json::Value obj;
		std::string errs;

		if( !Json::parseFromStream( reader, input, &obj, &errs ) )
			THROW( "Invalid object file data: " << objectFile );

		const auto& components = obj["Components"];

		for( unsigned i = 0; i < components.getMemberNames().size(); ++i )
		{
			const auto componentName = components.getMemberNames()[i];
			const auto found = m_componentNameToIndex.find( componentName );

			if( found == m_componentNameToIndex.end() )
				THROW( "Invalid component name in file: " << objectFile << " , component: " << componentName );

			Reflex::Components::BaseComponent* component = nullptr;
				
			if( componentName == Reflex::Components::Transform::GetComponentName() )
				component = ObjectGetComponent( newObject, found->second );
			else
				component = ObjectAddEmptyComponent( newObject, found->second );
				
			const auto& componentData = components[componentName];
			for( unsigned j = 0; j < componentData.getMemberNames().size(); ++j )
			{
				const auto variable = componentData.getMemberNames()[j];
				const auto value = componentData[variable].asString();
				if( !component->SetValue( variable, value ) )
					THROW( "Invalid component variable name in file: " << objectFile << " , component: " << componentName << ", variable: " << variable );
			}
		}

		return newObject;
	}

	void World::CreateROFile( const std::string& name, const Object& object )
	{
		auto path = name;

		const auto dot = path.rfind( '.' );

		if( dot == std::string::npos )
			path = path.append( ".ro" );
		else if( path.substr( 0, 3 ) != ".ro" )
			THROW( "Invalid object file name (must be .ro): " << name );

		Json::Value jsonOut;
		Json::Value components;

		const auto flags = m_objects.components[object.GetIndex()];

		for( unsigned i = 0; i < flags.size(); ++i )
		{
			if( !flags.test( i ) )
				continue;

			Json::Value data;
			std::vector< std::pair< std::string, std::string > > values;
			ObjectGetComponent( object, i )->GetValues( values );

			for( const auto& [key, value] : values )
				data[key] = value;

			const auto componentName = std::find_if( m_componentNameToIndex.begin(), m_componentNameToIndex.end(), [&]( const auto& pair )
			{
				return pair.second == i;
			} );

			assert( componentName != m_componentNameToIndex.end() );
			components[componentName->first] = data;
		}

		jsonOut["Components"] = components;

		Json::StreamWriterBuilder builder;
		builder["commentStyle"] = "None";
		builder["indentation"] = "   ";

		std::ofstream outputFileStream( path );
		builder.newStreamWriter()->write( jsonOut, &outputFileStream );
	}

	Reflex::Components::BaseComponent* World::ObjectAddEmptyComponent( const Object& object, const size_t family )
	{
		assert( IsValidObject( object ) );
		assert( !m_objects.components[object.GetIndex()].test( family ) );
		assert( family < m_components.size() );

		m_components[family]->ExpandToFit( object.GetIndex() + 1 );

		auto* newComponent = static_cast< Reflex::Components::BaseComponent* >( m_components[family].get()->ConstructEmpty( object.GetIndex(), ( Object )object ) );
		m_objects.components[object.GetIndex()].set( family );
		newComponent->OnConstructionComplete();
		OnComponentAdded( object );
		return newComponent;
	}

	Reflex::Components::BaseComponent* World::ObjectGetComponent( const BaseObject& object, const size_t family ) const
	{
		assert( IsValidObject( object ) );
		if( !ObjectHasComponent( object, family ) )
			return nullptr;
		return static_cast< Reflex::Components::BaseComponent* >( m_components[family]->Get( object.GetIndex() ) );
	}

	bool World::ObjectRemoveComponent( const BaseObject& object, const size_t family )
	{
		assert( IsValidObject( object ) );

		if( !ObjectHasComponent( object, family ) )
			return false;

		auto* component = static_cast< Reflex::Components::BaseComponent* >( m_components[family]->Get( object.GetIndex() ) );
		OnComponentRemoved( object );
		component->OnDestructionBegin();
		m_objects.components[object.GetIndex()].reset( family );
		m_components[family].get()->Destroy( object.GetIndex() );
		return true;
	}

	bool World::ObjectHasComponent( const BaseObject& object, const size_t family ) const
	{
		return m_objects.components[object.GetIndex()].test( family );
	}

	void World::DestroyObject( const BaseObject& object )
	{
		if( IsObjectFlagSet( object, ObjectFlags::Deleted ) )
			return;

		ObjectRemoveAllComponents( object );
		m_objects.flags[object.GetIndex()] = 0;
		SetObjectFlag( object, ObjectFlags::Deleted );
		m_objects.counters[object.GetIndex()]++;
	}

	void World::DestroyAllObjects()
	{
		for( unsigned i = 0; i < m_objects.counters.size(); ++i )
			if( !IsObjectFlagSet( i, ObjectFlags::Deleted ) )
				DestroyObject( ObjectFromIndex( i ) );
	}

	bool World::IsValidObject( const BaseObject& object ) const
	{
		return object.GetIndex() < m_objects.counters.size() && object.GetCounter() == m_objects.counters[object.GetIndex()];
	}

	bool World::IsObjectFlagSet( const BaseObject& object, const ObjectFlags flag ) const
	{
		assert( IsValidObject( object ) );
		return IsObjectFlagSet( object.GetIndex(), flag );
	}

	bool World::IsObjectFlagSet( const std::uint32_t objectIndex, const ObjectFlags flag ) const
	{
		return m_objects.flags[objectIndex].test( (size_t )flag );
	}

	void World::SetObjectFlag( const BaseObject& object, const ObjectFlags flag )
	{
		assert( IsValidObject( object ) );
		SetObjectFlag( object.GetIndex(), flag );
	}

	void World::SetObjectFlag( const std::uint32_t objectIndex, const ObjectFlags flag )
	{
		m_objects.flags[objectIndex].set( (size_t )flag );
	}

	Reflex::ComponentsMask World::ObjectGetComponentFlags( const BaseObject& object ) const
	{
		assert( IsValidObject( object ) );
		return m_objects.components[object.GetIndex()];
	}

	void World::ObjectRemoveAllComponents( const BaseObject& object )
	{
		assert( IsValidObject( object ) );

		for( size_t i = 0; i < MaxComponents; ++i )
			if( m_objects.components[object.GetIndex()].test( i ) )
				ObjectRemoveComponent( object, i );
	}

	sf::FloatRect World::GetBounds() const
	{
		return m_worldBounds;
	}

	Reflex::Components::Transform::Handle World::GetSceneRoot() const
	{
		return Object( m_sceneGraphRoot ).GetTransform();
	}

	void World::OnComponentAdded( const BaseObject& base )
	{
		const auto object = Object( base );
		assert( IsValidObject( object ) );

		// Here we want to check if we should add this component to any systems
		for( const auto&[type, baseSystem] : m_systems )
		{
			if( !baseSystem->ShouldAddObject( object ) )
				continue;

			auto* system = static_cast< Reflex::Systems::System* >( baseSystem.get() );
			const auto found = Reflex::Find( system->m_releventObjects, object );
			if( found != system->m_releventObjects.end() )
				continue;

			system->AddComponent( object ); 
			system->OnComponentAdded( object );
		}
	}

	void World::OnComponentRemoved( const BaseObject& base )
	{
		const auto object = Object( base );

		for( const auto& [type, baseSystem] : m_systems )
		{
			if( !baseSystem->ShouldAddObject( object ) )
				continue;

			auto* system = static_cast< Reflex::Systems::System* >( baseSystem.get() );
			const auto found = Reflex::Find( system->m_releventObjects, object );
			if( found == system->m_releventObjects.end() )
				continue;

			const auto toRemove = *found;
			system->m_releventObjects.erase( found );
			system->OnComponentRemoved( toRemove );
		}
	}

	bool World::IsActiveCamera( const Reflex::Components::Camera::Handle& camera ) const
	{
		return camera && m_activeCamera == camera->GetObject();
	}

	void World::SetActiveCamera( const Reflex::Components::Camera::Handle& camera )
	{
		if( !camera )
		{
			LOG_CRIT( "World::SetActiveCamera called with an invalid camera component handle" );
			return;
		}

		m_activeCamera = camera->GetObject();
		GetWindow().setView( *GetActiveCamera() );
	}

	Reflex::Handle< Reflex::Components::Camera > World::GetActiveCamera() const
	{
		const auto object = Object( m_activeCamera );
		return object ? object.GetComponent< Reflex::Components::Camera >() : Reflex::Handle< Reflex::Components::Camera >();
	}

	sf::Vector2f World::GetMousePosition( const Reflex::Components::Camera::Handle& camera ) const
	{
		assert( camera.IsValid() );
		return GetWindow().mapPixelToCoords( sf::Mouse::getPosition( GetWindow() ), *camera );
	}

	sf::Vector2f World::GetMousePosition() const
	{
		return GetWindow().mapPixelToCoords( sf::Mouse::getPosition( GetWindow() ) );
	}

	Reflex::Object World::ObjectFromIndex( const unsigned index )
	{
		return Reflex::Object( *this, uint32_t( index ), uint32_t( m_objects.counters[index] ) );
	}

	sf::Vector2f World::RandomWindowPosition( const float margin /*= 0.0f */ ) const
	{
		return sf::Vector2f(
			Reflex::RandomFloat( margin, GetWindow().getSize().x - margin * 2.0f ),
			Reflex::RandomFloat( margin, GetWindow().getSize().y - margin * 2.0f ) );
	}

	std::vector< Object > World::GetObjects()
	{
		std::vector< Object > output;
		output.reserve( m_objects.flags.size() );

		for( unsigned i = 1; i < m_objects.flags.size(); ++i )
			if( !IsObjectFlagSet( i, ObjectFlags::Deleted ) )
				output.push_back( ObjectFromIndex( i ) );

		return output;
	}

	World::RayCastResult World::RayCast( const sf::Vector2f& from, const sf::Vector2f& to )
	{
		class RayCastCallback : public b2RayCastCallback {
		public:
			std::vector< RayCastResult > foundBodies;

			float ReportFixture( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction ) final
			{
				const auto object = *(Object* )fixture->GetBody()->GetUserData();
				foundBodies.push_back( RayCastResult( object, fixture, Reflex::B2VecToVector2f( point ), Reflex::B2VecToVector2f( normal ), fraction ) );
				return true;
			}
		};

		RayCastCallback callback;
		GetBox2DWorld().RayCast( &callback, Reflex::Vector2fToB2Vec( from ), Reflex::Vector2fToB2Vec( to ) );

		const auto min = std::min_element( callback.foundBodies.begin(), callback.foundBodies.end(), [&from]( const auto& a, const auto& b )
		{
			return Reflex::GetDistanceSq( a.GetObject().GetTransform()->getPosition(), from ) < Reflex::GetDistanceSq( b.GetObject().GetTransform()->getPosition(), from );
		} );

		return min == callback.foundBodies.end() ? RayCastResult() : *min;
	}

	World::RayCastResult::RayCastResult( Object object, b2Fixture* fixture, const sf::Vector2f& point, const sf::Vector2f& normal, const float fraction )
		: hit( true )
		, object( object )
		, fixture( fixture )
		, point( point )
		, normal( normal )
		, fraction( fraction )
	{
	}

	World::RayCastResult::operator bool() const
	{
		return GetObject().IsValid();
	}

	Reflex::Object World::RayCastResult::GetObject() const
	{
		return ( Object )object;
	}

}