#ifndef _DARWIN_COMPAT_TIME_H
#define _DARWIN_COMPAT_TIME_H
#include <mach/std_types.h>
#include <mach/clock_types.h>
#include <sys/types.h>
/*
 #include_next <time.h>
 */
#include <sys/time.h>
#include <sys/signal.h>

/* [TMR] */
typedef uint64_t clockid_t;
/* [TMR] */
typedef uintptr_t timer_t;
#define CLOCK_REALTIME  CALENDAR_CLOCK
/* [MON] */
#define CLOCK_MONOTONIC SYSTEM_CLOCK
#define TIMER_ABSTIME 1
/* [TMR|CPT] */
#define CLOCK_PROCESS_CPUTIME_ID 3
/* [TMR|TCT] */
#define CLOCK_THREAD_CPUTIME_ID 4
/* [TMR] */
#ifndef _STRUCT_ITIMERSPEC
#define _STRUCT_ITIMERSPEC struct itimerspec
_STRUCT_ITIMERSPEC {
	struct timespec  it_interval;
	struct timespec  it_value;
};
#endif
#ifndef DELAYTIMER_MAX
#define DELAYTIMER_MAX 2147483647
#endif

__BEGIN_DECLS
/* [CPT] */
int clock_getcpuclockid(pid_t, clockid_t *);
/* [TMR] */
int clock_getres(clockid_t, struct timespec *);
/* [TMR] */
int clock_gettime(clockid_t, struct timespec *);
/* [TS] */
int clock_nanosleep(clockid_t, int, const struct timespec *,
	struct timespec *);
/* [TMR] */
int clock_settime(clockid_t, const struct timespec *);
/* [TCT] */
int pthread_getcpuclockid(pthread_t thread, clockid_t *clock_id);
/* [TMR */
int timer_create(clockid_t, struct sigevent * __restrict,
	timer_t * __restrict);
int timer_delete(timer_t);
int timer_gettime(timer_t, struct itimerspec *);
int timer_getoverrun(timer_t);
int timer_settime(timer_t, int, const struct itimerspec * __restrict,
	struct itimerspec * __restrict);
/* TMR] */
__END_DECLS
#endif
