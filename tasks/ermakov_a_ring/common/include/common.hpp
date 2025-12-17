#pragma once

#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace ermakov_a_ring {

struct RingTaskData {
  int source;
  int dest;
  std::vector<int> data;
};

using InType = RingTaskData;
using OutType = std::vector<int>;
using TestType = std::tuple<int, int, std::vector<int>>;

using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace ermakov_a_ring
