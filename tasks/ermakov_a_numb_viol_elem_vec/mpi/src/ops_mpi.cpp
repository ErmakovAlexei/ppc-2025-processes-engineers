#include "ermakov_a_numb_viol_elem_vec/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <vector>

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
  int rank = 0;
  int size = 0;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const auto &vec = GetInput();
  int n = static_cast<int>(vec.size());

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int chunk = (n + size - 1) / size;

  std::vector<int> local(chunk);

  const int *src = nullptr;
  std::vector<int> padded;

  if (rank == 0) {
    padded = vec;
    padded.resize(chunk * size, std::numeric_limits<int>::max());
    src = padded.data();
  }

  MPI_Scatter(src, chunk, MPI_INT, local.data(), chunk, MPI_INT, 0, MPI_COMM_WORLD);

  int local_count = 0;
  for (int i = 0; i + 1 < chunk; ++i) {
    local_count += (local[i] > local[i + 1]);
  }

  int global_count = 0;
  MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  MPI_Bcast(&global_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

  GetOutput() = global_count;
  return true;
}

bool ErmakovANumbViolElemVecMPI::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_numb_viol_elem_vec
