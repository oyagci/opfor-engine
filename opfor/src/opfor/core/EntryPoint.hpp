#pragma once

#ifdef OP4_PLATFORM_LINUX

opfor::UniquePtr<opfor::Application> opfor::CreateApplication();

int main()
{
    auto app = opfor::CreateApplication();
    app->Run();
    return 0;
}

#elif defined(OP4_PLATFORM_WINDOWS)

opfor::UniquePtr<opfor::Application> opfor::CreateApplication();

int main()
{
    auto app = opfor::CreateApplication();
    app->Run();
    return 0;
}

#else
#error "Unsupported Platflorm!"
#endif
