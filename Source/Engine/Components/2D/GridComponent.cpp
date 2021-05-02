#include "Precompiled.h"
#include "GridComponent.h"
#include "TransformComponent.h"
#include "Object.h"
#include "World.h"
#include "SceneNode.h"

namespace Reflex::Components
{
	Grid::Grid( const Reflex::Object& owner )
		: Component< Grid >( owner )
	{

	}

	Grid::Grid( const Reflex::Object& owner, const unsigned width, const unsigned height, const float cellWidth, const float cellHeight )
		: Component< Grid >( owner )
		, m_gridSize( width, height )
		, m_cellSize( cellWidth, cellHeight )
	{
		assert( width != 0U && height != 0U );
		m_children.resize( GetTotalCells() );
	}

	Grid::Grid( const Reflex::Object& owner, const sf::Vector2u gridSize, const sf::Vector2f cellSize )
		: Component< Grid >( owner )
		, m_gridSize( gridSize )
		, m_cellSize( cellSize )
	{
		assert( m_gridSize.x != 0U && m_gridSize.y != 0U );
		m_children.resize( GetTotalCells() );
	}

	bool Grid::SetValue( const std::string& variable, const std::string& value )
	{
		if( variable == "GridSize" )
		{
			m_gridSize = Reflex::FromString< sf::Vector2u >( value );
			return true;
		}
		else if( variable == "CellSize" )
		{
			m_cellSize = Reflex::FromString< sf::Vector2f >( value );
			return true;
		}
		else if( variable == "CentreGrid" )
		{
			m_centreGrid = Reflex::FromString< bool >( value );
			return true;
		}

		return false;
	}

	void Grid::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		values.emplace_back( "GridSize", Reflex::ToString( m_gridSize ) );
		values.emplace_back( "CellSize", Reflex::ToString( m_cellSize ) );
		values.emplace_back( "CentreGrid", Reflex::ToString( m_centreGrid ) );
	}

	void Grid::AddToGrid( const Reflex::Object& handle, const unsigned x, const unsigned y )
	{
		AddToGrid( handle, sf::Vector2u( x, y ) );
	}

	void Grid::AddToGrid( const Reflex::Object& handle, const sf::Vector2u index )
	{
		if( index.x >= GetWidth() || index.y >= GetHeight() )
		{
			LOG_CRIT( "Trying to add to grid with an invalid index of (" << index.x << ", " << index.y << ")" );
			return;
		}
			
		auto transform = handle.GetTransform();

		if( transform->m_parent )
			transform->m_parent.GetTransform()->DetachChild( handle );

		const auto insertIndex = GetIndex( index );
		m_children[insertIndex] = handle;
		transform->m_parent = GetObject();
		transform->IncrementZOrder();
		transform->SetLayer( GetObject().GetTransform()->GetLayer() + 1 );
		transform->setPosition( GetCellPositionRelative( index ) );
	}

	Reflex::Object Grid::RemoveFromGrid( const unsigned x, const unsigned y )
	{
		return RemoveFromGrid( sf::Vector2u( x, y ) );
	}

	Reflex::Object Grid::RemoveFromGrid( const sf::Vector2u index )
	{
		if( index.x >= GetWidth() || index.y >= GetHeight() )
		{
			LOG_CRIT( "Trying to remove from grid with an invalid index of (" << index.x << ", " << index.y << ")" );
			return Reflex::Object();
		}

		const auto insertIndex = GetIndex( index );
		const auto result = m_children[insertIndex];
		GetObject().GetWorld().GetSceneRoot()->AttachChild( result );
		m_children[insertIndex] = Object();
		return result;
	}

	Reflex::Object Grid::GetCell( const unsigned x, const unsigned y ) const
	{
		return GetCell( sf::Vector2u( x, y ) );
	}

	Reflex::Object Grid::GetCell( const sf::Vector2u index ) const
	{
		if( index.x >= GetWidth() || index.y >= GetHeight() )
		{
			LOG_CRIT( "Trying to access from grid with an invalid index of (" << index.x << ", " << index.y << ")" );
			return Reflex::Object();
		}

		const auto insertIndex = GetIndex( index );
		return m_children[insertIndex];
	}

	Reflex::Object Grid::GetCell( const unsigned x, const unsigned y, const bool rotate ) const
	{
		return GetCell( sf::Vector2u( x, y ), rotate );
	}

	Reflex::Object Grid::GetCell( const sf::Vector2u index, const bool rotate ) const
	{
		if( index.x >= GetWidth() || index.y >= GetHeight() )
		{
			LOG_CRIT( "Trying to access from grid with an invalid index of (" << index.x << ", " << index.y << ")" );
			return Reflex::Object();
		}

		const auto insertIndex = GetIndex( ConvertCellIndex( index, rotate ).second );
		return m_children[insertIndex];
	}

	unsigned Grid::GetWidth() const
	{
		return m_gridSize.x;
	}

	unsigned Grid::GetHeight() const
	{
		return m_gridSize.y;
	}

	sf::Vector2u Grid::GetGridSize() const
	{
		return m_gridSize;
	}

	unsigned Grid::GetTotalCells() const
	{
		return m_gridSize.x * m_gridSize.y;
	}

	sf::Vector2f Grid::GetCellSize() const
	{
		return m_cellSize;
	}

	void Grid::SetCellSize( const sf::Vector2f cellSize )
	{
		if( m_cellSize != cellSize )
		{
			m_cellSize = cellSize;
			UpdateGridPositions();
		}
	}

	void Grid::SetGridIsCentred( const bool centreGrid )
	{
		if( centreGrid != m_centreGrid )
		{
			m_centreGrid = centreGrid;
			UpdateGridPositions();
		}
	}

	bool Grid::GetGridIsCentred() const
	{
		return m_centreGrid;
	}

	sf::Vector2f Grid::GetCellPositionRelative( const sf::Vector2u index ) const
	{
		const auto topLeft = m_centreGrid ? sf::Vector2f( ( GetWidth() / -2.0f + 0.5f ) * m_cellSize.x, ( GetHeight() / -2.0f + 0.5f ) * m_cellSize.y ) : sf::Vector2f( 0.0f, 0.0f );
		const auto position = topLeft + sf::Vector2f( index.x * m_cellSize.x, index.y * m_cellSize.y );
		return position;
	}

	sf::Vector2f Grid::GetCellPositionWorld( const sf::Vector2u index ) const
	{
		const auto transform = GetObject().GetTransform();
		const auto gridCentre = transform->GetWorldPosition();
		const auto worldPosition = GetCellPositionRelative( index ) + gridCentre;
		return Reflex::RotateAroundPoint( worldPosition, gridCentre, transform->GetWorldRotation() );
	}

	std::pair< bool, sf::Vector2u > Grid::GetCellIndex( const sf::Vector2f worldPosition, bool rotated ) const
	{
		const auto transform = GetObject().GetTransform();
		const auto gridCentre = transform->GetWorldPosition();
		const auto correctedPosition = rotated ? Reflex::RotateAroundPoint( worldPosition, gridCentre, -transform->GetWorldRotation() ) : worldPosition;
		const auto localPosition = correctedPosition - ( GetCellPositionRelative( sf::Vector2u( 0U, 0U ) ) + gridCentre ); // Scale??
		const auto indexX = RoundToInt( localPosition.x / m_cellSize.x );
		const auto indexY = RoundToInt( localPosition.y / m_cellSize.y );
		const auto valid = indexX >= 0 && indexX < ( int )GetWidth() && indexY >= 0 && indexY < ( int )GetHeight();
		return std::make_pair( valid, sf::Vector2u( indexX, indexY ) );
	}

	std::pair< bool, sf::Vector2u > Grid::ConvertCellIndex( const sf::Vector2u index, const bool rotate ) const
	{
		const auto rotation = GetObject().GetTransform()->GetWorldRotation();
		const auto rotatedIndex = Reflex::RotateAroundPoint( Reflex::Vector2uToVector2i( index ), sf::Vector2i( 1, 1 ), rotate ? -rotation : rotation );
		const auto valid = rotatedIndex.x >= 0 && rotatedIndex.x < ( int )GetWidth() && rotatedIndex.y >= 0 && rotatedIndex.y < ( int )GetHeight();
		return std::make_pair( valid, Reflex::Vector2iToVector2u( rotatedIndex ) );
	}

	void Grid::ForEachChild( std::function< void( const Reflex::Object& obj, const sf::Vector2u index ) > callback )
	{
		for( unsigned i = 0U; i < m_children.size(); ++i )
			callback( m_children[i], GetCoords( i ) );
	}

	unsigned Grid::GetIndex( const sf::Vector2u coords ) const
	{
		return coords.y * GetWidth() + coords.x;
	}

	const sf::Vector2u Grid::GetCoords( const unsigned index ) const
	{
		return sf::Vector2u( index % GetWidth(), index / GetWidth() );
	}

	void Grid::UpdateGridPositions()
	{
		for( unsigned y = 0U; y < GetHeight(); ++y )
		{
			for( unsigned x = 0U; x < GetWidth(); ++x )
			{
				const auto index = sf::Vector2u( x, y );
				m_children[GetIndex( index )].GetTransform()->setPosition( GetCellPositionRelative( index ) );
			}
		}
	}
}