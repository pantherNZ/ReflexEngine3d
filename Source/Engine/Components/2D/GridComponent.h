#pragma once

#include "Components/Component.h"
#include "Memory/VectorSet.h"
#include "Objects/Object.h"

namespace Reflex::Components
{
	// Class definition
	class Grid : public Component< Grid >
	{
	public:
		Grid( const Reflex::Object& owner );
		Grid( const Reflex::Object& owner, const unsigned width, const unsigned height, const float cellWidth, const float cellHeight );
		Grid( const Reflex::Object& owner, const sf::Vector2u gridSize, const sf::Vector2f cellSize );

		bool SetValue( const std::string& variable, const std::string& value ) override;
		void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const override;
		static std::string GetComponentName() { return "Grid"; }

		void AddToGrid( const Reflex::Object& handle, const unsigned x, const unsigned y );
		void AddToGrid( const Reflex::Object& handle, const sf::Vector2u index );
		Reflex::Object RemoveFromGrid( const unsigned x, const unsigned y );
		Reflex::Object RemoveFromGrid( const sf::Vector2u index );
		Reflex::Object GetCell( const unsigned x, const unsigned y ) const;
		Reflex::Object GetCell( const sf::Vector2u index ) const;
		Reflex::Object GetCell( const unsigned x, const unsigned y, const bool rotate ) const;
		Reflex::Object GetCell( const sf::Vector2u index, const bool rotate ) const;

		unsigned GetWidth() const;
		unsigned GetHeight() const;
		sf::Vector2u GetGridSize() const;
		unsigned GetTotalCells() const;

		sf::Vector2f GetCellSize() const;
		void SetCellSize( const sf::Vector2f cellSize );

		void SetGridIsCentred( const bool centreGrid );
		bool GetGridIsCentred() const;

		sf::Vector2f GetCellPositionRelative( const sf::Vector2u index ) const;
		sf::Vector2f GetCellPositionWorld( const sf::Vector2u index ) const;
		std::pair< bool, sf::Vector2u > GetCellIndex( const sf::Vector2f worldPosition, bool rotated = true ) const;
		std::pair< bool, sf::Vector2u > ConvertCellIndex( const sf::Vector2u index, const bool rotate ) const;

		void ForEachChild( std::function< void( const Reflex::Object& obj, const sf::Vector2u index ) > callback );

	protected:
		unsigned GetIndex( const sf::Vector2u coords ) const;
		const sf::Vector2u GetCoords( const unsigned index ) const;
		void UpdateGridPositions();

	private:
		Reflex::VectorSet< Reflex::Object > m_children;
		sf::Vector2u m_gridSize;
		sf::Vector2f m_cellSize;
		bool m_centreGrid = true;
	};
}