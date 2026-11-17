#pragma once

#include "command.hpp"
#include "status.hpp"

namespace shell
{
    Status execute_pipeline(const Pipeline& pipeline);
}
