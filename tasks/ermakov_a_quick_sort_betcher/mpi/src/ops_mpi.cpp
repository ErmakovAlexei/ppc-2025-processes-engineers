#include "ermakov_a_quick_sort_betcher/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "ermakov_a_quick_sort_betcher/common/include/common.hpp"
#include "util/include/util.hpp"

namespace ermakov_a_quick_sort_betcher {

ErmakovAQuickSortBetcherTestTaskMPI::ErmakovAQuickSortBetcherTestTaskMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool ErmakovAQuickSortBetcherTestTaskMPI::ValidationImpl() {
  int is_init = 0;
  MPI_Initialized(&is_init);
  if (!is_init) {
    return true;
  }

  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  return size > 0;
}

bool ErmakovAQuickSortBetcherTestTaskMPI::PreProcessingImpl() {
  return true;
}

void ErmakovAQuickSortBetcherTestTaskMPI::compare_split_low(int partner) {
  int size = static_cast<int>(local_vec.size());
  MPI_Sendrecv(local_vec.data(), size, MPI_INT, partner, 0, remote_data.data(), size, MPI_INT, partner, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  int i = 0, j = 0;
  for (int k = 0; k < size; ++k) {
    if (j >= size || (i < size && local_vec[i] <= remote_data[j])) {
      temp_data[k] = local_vec[i++];
    } else {
      temp_data[k] = remote_data[j++];
    }
  }
  local_vec = temp_data;
}

void ErmakovAQuickSortBetcherTestTaskMPI::compare_split_high(int partner) {
  int size = static_cast<int>(local_vec.size());
  MPI_Sendrecv(local_vec.data(), size, MPI_INT, partner, 0, remote_data.data(), size, MPI_INT, partner, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  int i = size - 1, j = size - 1;
  for (int k = size - 1; k >= 0; --k) {
    if (j < 0 || (i >= 0 && local_vec[i] >= remote_data[j])) {
      temp_data[k] = local_vec[i--];
    } else {
      temp_data[k] = remote_data[j--];
    }
  }
  local_vec = temp_data;
}

bool ErmakovAQuickSortBetcherTestTaskMPI::RunImpl() {
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int total_n = 0;
  if (world_rank == 0) {
    total_n = static_cast<int>(GetInput().size());
  }
  MPI_Bcast(&total_n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (total_n == 0) {
    return true;
  }

  int elements_per_proc = (total_n + world_size - 1) / world_size;
  int n_prime = elements_per_proc * world_size;

  local_vec.assign(elements_per_proc, std::numeric_limits<int>::max());
  remote_data.resize(elements_per_proc);
  temp_data.resize(elements_per_proc);

  std::vector<int> full_vec;

  if (world_rank == 0) {
    full_vec.assign(n_prime, std::numeric_limits<int>::max());
    std::copy(GetInput().begin(), GetInput().end(), full_vec.begin());
  }

  MPI_Scatter(world_rank == 0 ? full_vec.data() : nullptr, elements_per_proc, MPI_INT, local_vec.data(),
              elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

  std::sort(local_vec.begin(), local_vec.end());

  int next_power_of_2 = 1;
  while (next_power_of_2 < world_size) {
    next_power_of_2 <<= 1;
  }

  for (int p = 1; p < next_power_of_2; p <<= 1) {
    for (int k = p; k > 0; k >>= 1) {
      for (int j = k % p; j <= next_power_of_2 - 1 - k; j += 2 * k) {
        for (int i = 0; i < k; ++i) {
          if ((j + i) / (p * 2) == (j + i + k) / (p * 2)) {
            int p1 = j + i;
            int p2 = j + i + k;

            if (world_rank == p1) {
              if (p2 < world_size) {
                compare_split_low(p2);
              }
            } else if (world_rank == p2) {
              compare_split_high(p1);
            }
          }
        }
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  }

  if (world_rank == 0) {
    full_vec.assign(n_prime, 0);
  }

  MPI_Gather(local_vec.data(), elements_per_proc, MPI_INT, world_rank == 0 ? full_vec.data() : nullptr,
             elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

  if (world_rank == 0) {
    full_vec.resize(total_n);
    GetOutput() = full_vec;
  }

  return true;
}

bool ErmakovAQuickSortBetcherTestTaskMPI::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_quick_sort_betcher
