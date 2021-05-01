#pragma once

#include "Precompiled.h"

#include "Engine.h"

namespace Reflex
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while( true );
	}

	int Application::GetExitCode() const
	{
		return 0;
	}
}