#pragma once

#include "ermakov_a_quick_sort_betcher/common/include/common.hpp"
#include "task/include/task.hpp"

namespace ermakov_a_quick_sort_betcher {

class ErmakovAQuickSortBetcherTestTaskSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit ErmakovAQuickSortBetcherTestTaskSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  int input_size_ = 0;
  void do_std_sort();
  void do_batcher_sort();
  bool is_power_of_two(size_t n);
};

}  // namespace ermakov_a_quick_sort_betcher
