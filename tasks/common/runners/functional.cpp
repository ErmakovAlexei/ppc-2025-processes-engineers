#include "runners/include/runners.hpp"
#include "util/include/util.hpp"

auto main(int argc, char **argv) -> int {
  if (ppc::util::IsUnderMpirun()) {
    return ppc::runners::Init(argc, argv);
  }
  return ppc::runners::SimpleInit(argc, argv);
}
