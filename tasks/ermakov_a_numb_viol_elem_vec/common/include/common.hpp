#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace  ermakov_a_numb_viol_elem_vec{

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace ermakov_a_numb_viol_elem_vec
