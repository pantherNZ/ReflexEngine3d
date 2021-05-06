#pragma once

#include "Systems/System.h"

namespace Reflex::Systems
{
	class PhysicsSystem : public System
	{
	public:
		using System::System;

		void RegisterComponents() final;
		void Update( const float deltaTime ) final;
	};
}