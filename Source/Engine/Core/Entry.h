#pragma once

extern Reflex::Application* Reflex::CreateApplication();

int main( int argc, char** argv )
{
	auto* app = Reflex::CreateApplication();
	app->Run();
	auto exit_code = app->GetExitCode();
	delete app;
	return exit_code;
}