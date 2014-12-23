#ifndef CPMF_PARALLEL_TP_SWITCH_HPP_
#define CPMF_PARALLEL_TP_SWITCH_HPP_

// for Cilk
#if defined TP_CILK
# include <cilk/cilk.h>
# define mk_task_group
# define cpmf_spawn(func) cilk_spawn func
# define cpmf_sync cilk_sync

// for MassiveThreads
#elif defined TP_MYTH
# include <mtbb/task_group.h>
# define mk_task_group mtbb::task_group __tg__
# define cpmf_spawn(func) __tg__.run([&] { func; })
# define cpmf_sync __tg__.wait()
#endif

#endif // CPMF_PARALLEL_TP_SWITCH_HPP_
