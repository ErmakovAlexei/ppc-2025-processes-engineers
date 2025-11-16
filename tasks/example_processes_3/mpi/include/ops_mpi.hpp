#pragma once

#include "example_processes_3/common/include/common.hpp"
#include "task/include/task.hpp"

namespace nesterov_a_test_task_processes_3 {

class NesterovATestTaskMPI : public BaseTask {
 public:
  static constexpr auto GetStaticTypeOfTask() -> ppc::task::TypeOfTask {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit NesterovATestTaskMPI(const InType &in);

 private:
  auto ValidationImpl() -> bool override;
  auto PreProcessingImpl() -> bool override;
  auto RunImpl() -> bool override;
  auto PostProcessingImpl() -> bool override;
};

}  // namespace nesterov_a_test_task_processes_3
