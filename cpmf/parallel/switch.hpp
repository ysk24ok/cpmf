#ifndef CPMF_PARALLEL_SWITCH_HPP_
#define CPMF_PARALLEL_SWITCH_HPP_

#if defined TP_BASED
#include <cpmf/parallel/tp_based/tp_based.hpp>
using namespace cpmf::parallel::tp_based;

#elif defined FPSGD
#include <cpmf/parallel/fpsgd/fpsgd.hpp>
using namespace cpmf::parallel::fpsgd;

#endif

#endif // CPMF_PARALLEL_SWITCH_HPP_
