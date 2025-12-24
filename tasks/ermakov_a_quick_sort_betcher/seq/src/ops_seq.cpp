#include "ermakov_a_quick_sort_betcher/seq/include/ops_seq.hpp"

#include <algorithm>
#include <limits>
#include <vector>

#include "ermakov_a_quick_sort_betcher/common/include/common.hpp"
#include "util/include/util.hpp"

namespace ermakov_a_quick_sort_betcher {

ErmakovAQuickSortBetcherTestTaskSEQ::ErmakovAQuickSortBetcherTestTaskSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::ValidationImpl() {
  return true;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::PreProcessingImpl() {
  input_size_ = static_cast<int>(GetInput().size());
  GetOutput() = GetInput();
  return true;
}

// Вспомогательная функция проверки степени двойки
bool ErmakovAQuickSortBetcherTestTaskSEQ::is_power_of_two(size_t n) {
  return (n > 0) && ((n & (n - 1)) == 0);
}

// Ветка Бэтчера
void ErmakovAQuickSortBetcherTestTaskSEQ::do_batcher_sort() {
  auto &data = GetOutput();
  int N = static_cast<int>(data.size());
  for (int p = 1; p < N; p <<= 1) {
    for (int k = p; k > 0; k >>= 1) {
      for (int j = k % p; j <= N - 1 - k; j += 2 * k) {
        for (int i = 0; i < k; ++i) {
          int idx1 = j + i;
          int idx2 = j + i + k;
          if ((idx1 / (p * 2)) == (idx2 / (p * 2))) {
            if (data[idx1] > data[idx2]) {
              std::swap(data[idx1], data[idx2]);
            }
          }
        }
      }
    }
  }
}

// Ветка Быстрой сортировки (через std::sort)
void ErmakovAQuickSortBetcherTestTaskSEQ::do_std_sort() {
  std::sort(GetOutput().begin(), GetOutput().end());
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::RunImpl() {
  if (GetOutput().empty()) {
    return true;
  }

  // Решаем, какой алгоритм использовать
  if (is_power_of_two(GetOutput().size()) && GetOutput().size() < 100000) {
    do_batcher_sort();
  } else {
    do_std_sort();
  }

  return true;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::PostProcessingImpl() {
  // Просто возвращаем результат
  return true;
}

}  // namespace ermakov_a_quick_sort_betcher
