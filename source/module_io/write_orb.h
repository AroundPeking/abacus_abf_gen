#pragma once

#include "module_basis/module_ao/ORB_atomic_lm.h"
#include <string>
#include <vector>

namespace ModuleIO
{

void write_orb(const std::vector<std::vector<Numerical_Orbital_Lm>> &orb, const std::string &file_name);

}