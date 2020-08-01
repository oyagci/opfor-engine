#pragma once

#ifdef OP4_PLATFORM_LINUX

opfor::UniquePtr<opfor::Application> opfor::CreateApplication();

int main()
{
	auto app = opfor::CreateApplication();
	app->Run();
}

#else
# error "Unsupported Platflorm!"
#endif
