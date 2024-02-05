#include "division_engine_core/context.h"
#include <iostream>

#include <division_engine/core/core_runner.hpp>

struct LifecycleManager 
{
    void draw(DivisionContext* context)
    {
    }

    void error(DivisionContext* context, int32_t errorCode, const char* errorMessage)
    {
        std::cerr << "Error code: " << errorCode 
                  << ". Error message: " << errorMessage << std::endl;
    }

    void cleanup(DivisionContext* context)
    {

    }
};

struct LifecycleManagerBuilder
{
    using managerType = LifecycleManager;

    managerType build(DivisionContext* context)
    {
        std::cout << "Hello from lifecycle builder" << std::endl;
        return LifecycleManager {};
    }
};

int main(int argc, char** argv)
{
    division_engine::core::CoreRunner coreRunner {
        std::string { "Hello division cpp" },
        {512, 512},
    };

    LifecycleManagerBuilder builder {};
    coreRunner.run(builder);
}