#include "ermakov_a_quick_sort_betcher/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

#include "ermakov_a_quick_sort_betcher/common/include/common.hpp"

namespace ermakov_a_quick_sort_betcher {

ErmakovAQuickSortBetcherTestTaskSEQ::ErmakovAQuickSortBetcherTestTaskSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::ValidationImpl() {
  return true;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::PreProcessingImpl() {
  GetOutput() = GetInput();
  return true;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::IsPowerOfTwo(std::size_t n) {
  return (n > 0) && ((n & (n - 1)) == 0);
}

void ErmakovAQuickSortBetcherTestTaskSEQ::QuickSort(std::vector<int> &arr, int left, int right) {
  if (left >= right) {
    return;
  }
  int pivot = arr[left + (right - left) / 2];
  int i = left, j = right;
  while (i <= j) {
    while (arr[i] < pivot) {
      i++;
    }
    while (arr[j] > pivot) {
      j--;
    }
    if (i <= j) {
      std::swap(arr[i++], arr[j--]);
    }
  }
  if (left < j) {
    QuickSort(arr, left, j);
  }
  if (i < right) {
    QuickSort(arr, i, right);
  }
}

void ErmakovAQuickSortBetcherTestTaskSEQ::DoBatcherSort() {
  auto &data = GetOutput();
  int n = static_cast<int>(data.size());

  for (int p = 1; p < n; p <<= 1) {
    for (int k = p; k > 0; k >>= 1) {
      for (int j = k % p; j <= n - 1 - k; j += 2 * k) {
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

bool ErmakovAQuickSortBetcherTestTaskSEQ::RunImpl() {
  if (GetOutput().empty()) {
    return true;
  }
  int n = static_cast<int>(GetOutput().size());

  if (IsPowerOfTwo(n)) {
    int mid = n / 2;
    if (mid > 0) {
      QuickSort(GetOutput(), 0, mid - 1);
      QuickSort(GetOutput(), mid, n - 1);
    }
    DoBatcherSort();
  } else {
    QuickSort(GetOutput(), 0, n - 1);
  }
  return true;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_quick_sort_betcher
