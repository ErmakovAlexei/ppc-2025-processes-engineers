#pragma once

#include <vector>

#include "ermakov_a_quick_sort_betcher/common/include/common.hpp"
#include "task/include/task.hpp"

namespace ermakov_a_quick_sort_betcher {

class ErmakovAQuickSortBetcherTestTaskMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit ErmakovAQuickSortBetcherTestTaskMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  std::vector<int> local_vec_;
  std::vector<int> remote_data_;
  std::vector<int> temp_data_;

  int world_size_{0};
  int world_rank_{0};

  void CompareSplitLow(int partner);
  void CompareSplitHigh(int partner);
  void RunBatcherNetwork(int next_power_of_two);
  void RunBatcherStep(int phase, int step, int next_power_of_two);
};

}  // namespace ermakov_a_quick_sort_betcher
