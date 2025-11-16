#pragma once

#include "example_threads/common/include/common.hpp"
#include "task/include/task.hpp"

namespace nesterov_a_test_task_threads {

class NesterovATestTaskTBB : public BaseTask {
 public:
  static constexpr auto GetStaticTypeOfTask() -> ppc::task::TypeOfTask {
    return ppc::task::TypeOfTask::kTBB;
  }
  explicit NesterovATestTaskTBB(const InType &in);

 private:
  auto ValidationImpl() -> bool override;
  auto PreProcessingImpl() -> bool override;
  auto RunImpl() -> bool override;
  auto PostProcessingImpl() -> bool override;
};

}  // namespace nesterov_a_test_task_threads
