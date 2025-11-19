#include "ermakov_a_numb_viol_elem_vec/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <vector>

#include "ermakov_a_numb_viol_elem_vec/common/include/common.hpp"
#include "util/include/util.hpp"

namespace ermakov_a_numb_viol_elem_vec {

ErmakovANumbViolElemVecMPI::ErmakovANumbViolElemVecMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool ErmakovANumbViolElemVecMPI::ValidationImpl() {
  return true;
}

bool ErmakovANumbViolElemVecMPI::PreProcessingImpl() {
  return true;
}

bool ErmakovANumbViolElemVecMPI::RunImpl() {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const std::vector<int> &vec = GetInput();
  int n = vec.size();

  if (n == 0) {
    int zero = 0;
    MPI_Bcast(&zero, 1, MPI_INT, 0, MPI_COMM_WORLD);
    GetOutput() = 0;
    return true;
  }

  int base = n / size;
  int rem = n % size;

  std::vector<int> counts(size);
  std::vector<int> displs(size);
  int shift = 0;
  for (int i = 0; i < size; ++i) {
    counts[i] = base;
    if (i < rem) {
      counts[i] += 1;
    }
    displs[i] = shift;
    shift += counts[i];
  }

  int local_n = counts[rank];
  std::vector<int> local_vec(local_n);

  if (rank == 0) {
    for (int i = 0; i < local_n; i++) {
      local_vec[i] = vec[i];
    }
    for (int p = 1; p < size; p++) {
      if (counts[p] > 0) {
        MPI_Send(vec.data() + displs[p], counts[p], MPI_INT, p, 0, MPI_COMM_WORLD);
      }
    }
  } else {
    if (local_n > 0) {
      MPI_Recv(local_vec.data(), local_n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  int local_viol = 0;
  for (int i = 0; i + 1 < local_n; i++) {
    if (local_vec[i] > local_vec[i + 1]) {
      local_viol += 1;
    }
  }

  int left_last = 0;
  int my_last = 0;
  if (local_n > 0) {
    my_last = local_vec[local_n - 1];
  }

  if (rank > 0) {
    MPI_Recv(&left_last, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  if (rank < size - 1) {
    MPI_Send(&my_last, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
  }

  int border = 0;
  if (local_n > 0 && rank > 0) {
    if (left_last > local_vec[0]) {
      border = 1;
    }
  }

  int total_viol = local_viol + border;
  int result = 0;

  MPI_Reduce(&total_viol, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(&result, 1, MPI_INT, 0, MPI_COMM_WORLD);

  GetOutput() = result;
  return true;
}

bool ErmakovANumbViolElemVecMPI::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_numb_viol_elem_vec
