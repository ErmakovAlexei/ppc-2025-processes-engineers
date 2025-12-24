#pragma once

#include <cstddef>

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

  void DoStdSort();
  void DoBatcherSort();
  bool IsPowerOfTwo(std::size_t n);
};

}  // namespace ermakov_a_quick_sort_betcher
