#include <mpi.h>

#include "util/include/perf_test_util.hpp"

auto ppc::util::GetTimeMPI() -> double {
  return MPI_Wtime();
}

auto ppc::util::GetMPIRank() -> int {
  int rank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  return rank;
}
