#include <ReflexInclude.h>
#include <iostream>

class Demo : public Reflex::Application
{
public:
	Demo() : Reflex::Application()
	{

	}

	~Demo()
	{

	}
};

Reflex::Application* Reflex::CreateApplication()
{
	return new Demo();
}