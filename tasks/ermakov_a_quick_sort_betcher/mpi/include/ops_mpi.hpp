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

  std::vector<int> local_vec;
  std::vector<int> remote_data;
  std::vector<int> temp_data;

  int world_size;
  int world_rank;

  void compare_split_low(int partner);
  void compare_split_high(int partner);
};

}  // namespace ermakov_a_quick_sort_betcher
