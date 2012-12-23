/*
 * Copyright (c) 2012, PrZhu(przhu@gmx.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "time.h"

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

/* [CPT] */
int clock_getcpuclockid(pid_t pid, clockid_t *clock_id)
{
	clockid_t id = CLOCK_PROCESS_CPUTIME_ID;
	mach_port_t task;
	if (pid == 0) {
		task = mach_task_self();
	} else {
		int kret;
		kret = task_for_pid(mach_task_self(), pid, &task);
		if (kret != KERN_SUCCESS) {
			errno = EPERM; return -1;
		}
	}
	id |= ((uint64_t)task) << 32;
	*clock_id = id;
	return 0;
}
/* [TMR] */
int clock_getres(clockid_t clock_id, struct timespec *ts)
{
	clock_id &= 0xffffffff;
	ts->tv_sec = 0;
	switch (clock_id) {
	case CLOCK_REALTIME:
	case CLOCK_THREAD_CPUTIME_ID:
		ts->tv_nsec = NSEC_PER_USEC;
		break;
	case CLOCK_MONOTONIC:
	case CLOCK_PROCESS_CPUTIME_ID:
		ts->tv_nsec = 1;
		break;
	default:
		errno = EINVAL;
		return -1;
	}
	return 0;
}
/* [TMR] */
int clock_gettime(clockid_t clock_id, struct timespec *ts)
{
	uint32_t id; mach_port_t port;
	id = (uint32_t)(clock_id & 0xffffffff);
	port = (mach_port_t)(clock_id >> 32);
	switch (id) {
	case CLOCK_REALTIME:
		{
		struct timeval tv;
		(void)gettimeofday(&tv, NULL);
		ts->tv_sec = tv.tv_sec;
		ts->tv_nsec= tv.tv_usec * NSEC_PER_USEC;
		}
		break;
	case CLOCK_MONOTONIC:
		{
		uint64_t time;
		lldiv_t lld;
		time = mach_absolute_time();
		lld = lldiv(time, NSEC_PER_SEC);
		ts->tv_sec = lld.quot;
		ts->tv_nsec= lld.rem;
		}
		break;
	case CLOCK_PROCESS_CPUTIME_ID:
		{
		uint64_t time;
		lldiv_t lld;
		task_absolutetime_info_data_t info;
		int kret;
		natural_t count = TASK_ABSOLUTETIME_INFO_COUNT;
		if (port == 0) port = mach_task_self();
		kret = task_info(port, TASK_ABSOLUTETIME_INFO, 
			(thread_info_t)&info, &count);
		if (kret != KERN_SUCCESS) {
			errno = EINVAL;
			return -1;
		}
		time = info.total_user + info.total_system;
		lld = lldiv(time, NSEC_PER_SEC);
		ts->tv_sec = lld.quot;
		ts->tv_nsec = lld.rem;
		}
		break;
	case CLOCK_THREAD_CPUTIME_ID:
		{
		thread_basic_info_data_t info;
		int kret;
		natural_t count = THREAD_BASIC_INFO_COUNT;
		if (port == 0) port = pthread_mach_thread_np(pthread_self());
		kret = thread_info(port, THREAD_BASIC_INFO,
			(thread_info_t)&info, &count);
		if (kret != KERN_SUCCESS) {
			errno = EINVAL;
			return -1;
		}
		time_value_add(&info.user_time, &info.system_time);
		ts->tv_sec = info.user_time.seconds;
		ts->tv_nsec = info.user_time.microseconds * NSEC_PER_USEC;
		}
		break;
	default:
		errno = EINVAL;
		return -1;
	}
	return 0;
}
/* [TS] */
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *_rqtp,
	struct timespec *rmtp)
{
	struct timespec rqt;
	struct timespec *rqtp = &rqt;
	rqtp->tv_sec = _rqtp->tv_sec;
	rqtp->tv_nsec= _rqtp->tv_nsec;
	clock_id &= 0xffffffff;
	switch(clock_id) {
	case CLOCK_REALTIME:
		if (flags & TIMER_ABSTIME) {
			struct timespec ts;
			(void)clock_gettime(CLOCK_REALTIME, &ts);
			SUB_MACH_TIMESPEC(rqtp, &ts);
		} 
		return nanosleep(rqtp, rmtp);
	case CLOCK_MONOTONIC:
		if (~(flags & TIMER_ABSTIME)) {
			struct timespec ts;
			(void)clock_gettime(CLOCK_MONOTONIC, &ts);
			ADD_MACH_TIMESPEC(rqtp, &ts);
		}
		{
		int kret;
		kret = mach_wait_until(rqtp->tv_sec * NSEC_PER_SEC + rqtp->tv_nsec);
		if (kret == KERN_SUCCESS) {
			return 0;
		} else if (kret == KERN_ABORTED) {
			if (rmtp != NULL) {
				struct timespec ts;
				(void)clock_gettime(CLOCK_MONOTONIC, &ts);
				SUB_MACH_TIMESPEC(rqtp, &ts);
				rmtp->tv_sec = rqtp->tv_sec;
				rmtp->tv_nsec= rqtp->tv_nsec;
			}
			errno = EINTR;
		} else {
			errno = EINVAL;
		}
		return -1;
		}
	default:
		errno = ENOTSUP;
		return -1;
	}
}
/* [TMR] */
int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
	clock_id &= 0xffffffff;
	switch (clock_id) {
	case CLOCK_REALTIME:
		errno = EPERM;
		break;
	default:
		errno = EINVAL;
	}
	return -1;
}
/* [TCT] */
int pthread_getcpuclockid(pthread_t thread, clockid_t *clock_id)
{
	clockid_t id = CLOCK_THREAD_CPUTIME_ID;
	mach_port_t port = pthread_mach_thread_np(thread);
	if (port == MACH_PORT_NULL) {
		errno = ESRCH;
		return -1;
	} else {
		id |= ((uint64_t)port << 32);
		*clock_id = id;
		return 0;
	}
}
/* [TMR */
#include <dispatch/dispatch.h>
#include <libkern/OSAtomic.h>
#ifndef _DARWIN_COMPAT_TIMER_MAX
#define _DARWIN_COMPAT_TIMER_MAX 16
#endif
#define _DARWIN_COMPAT_TIMER_L (~0ull)
static OSSpinLock timers_lock = OS_SPINLOCK_INIT;
static timer_t timers[_DARWIN_COMPAT_TIMER_MAX];
static size_t timer_idx_get(timer_t timer_id, timer_t placeholder)
{
	size_t i;
	OSSpinLockLock(&timers_lock);
	for (i = 0; i < _DARWIN_COMPAT_TIMER_MAX 
		&& timers[i] != timer_id; ++i)
		;
	if (i < _DARWIN_COMPAT_TIMER_MAX)
		timers[i] = placeholder;
	OSSpinLockUnlock(&timers_lock);
	return i;
}

static timer_t assign_timer_id(size_t idx, timer_t new)
{
	timer_t old;
	OSSpinLockLock(&timers_lock);
	old = timers[idx];
	timers[idx] = new;
	OSSpinLockUnlock(&timers_lock);
	return old;
}

static int remove_timer_id(timer_t timer_id)
{
	size_t idx;
	if (timer_id == 0 || timer_id == _DARWIN_COMPAT_TIMER_L) {
		errno = EINVAL;
		return -1;
	}
	idx = timer_idx_get(timer_id, 0);
	if (idx >= _DARWIN_COMPAT_TIMER_MAX) {
		errno = EINVAL;
		return -1;
	}
	return 0;
}

static int check_timer_id(timer_t timer_id)
{
	size_t idx;
	if (timer_id == 0 || timer_id == _DARWIN_COMPAT_TIMER_L) {
		errno = EINVAL;
		return -1;
	}
	idx = timer_idx_get(timer_id, timer_id);
	if (idx >= _DARWIN_COMPAT_TIMER_MAX) {
		errno = EINVAL;
		return -1;
	}
	return 0;
}

struct timer_context {
	dispatch_source_t timer;
	clockid_t clock_id;
	uint64_t ointerval;
	int armed;
	long overrun;
	union sigval user_value;
	void(*user_notify_func)(union sigval);
};

static void timer_func(void *context)
{
	struct timer_context *tcxt = (struct timer_context *)context;
	if (context == NULL) return;
	dispatch_retain(tcxt->timer);
	tcxt->overrun = dispatch_source_get_data(tcxt->timer);
	dispatch_release(tcxt->timer);
	if (tcxt->user_notify_func) {
		(*tcxt->user_notify_func)(tcxt->user_value);
	}
}

int timer_create(clockid_t clock_id, struct sigevent *__restrict evp,
	timer_t *__restrict timerid)
{
	size_t idx;
	struct timer_context *tcxt;
	dispatch_source_t *dsp = (dispatch_source_t *)timerid;
	clock_id &= 0xffffffff;
	switch (clock_id) {
	case CLOCK_REALTIME:
	case CLOCK_MONOTONIC:
		break;
	case CLOCK_PROCESS_CPUTIME_ID:
	case CLOCK_THREAD_CPUTIME_ID:
		errno = ENOTSUP;
		return -1;
	default:
		errno = EINVAL;
		return -1;
	}
	switch (evp->sigev_notify) {
	case SIGEV_NONE:
	case SIGEV_THREAD:
		break;
	default:
		errno = EAGAIN;
		return -1;
	}

	idx = timer_idx_get(0, _DARWIN_COMPAT_TIMER_L);
	if (idx >= _DARWIN_COMPAT_TIMER_MAX) {
		errno = EAGAIN;
		return -1;
	}
	*dsp = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,
		0, 0, DISPATCH_TARGET_QUEUE_DEFAULT);
	if (*dsp == NULL) {
		(void)assign_timer_id(idx, 0);
		errno = EAGAIN;
		return -1;
	}
	tcxt = (struct timer_context*)calloc(1, sizeof(struct timer_context));
	tcxt->clock_id = clock_id;
	tcxt->timer    = *dsp;
	if (evp->sigev_notify == SIGEV_THREAD) {
		tcxt->user_value = evp->sigev_value;
		tcxt->user_notify_func = evp->sigev_notify_function;
	}
	dispatch_set_context(*dsp, tcxt);
	dispatch_set_finalizer_f(*dsp, free);
	dispatch_source_set_event_handler_f(*dsp, timer_func);
	(void)assign_timer_id(idx, (timer_t)*dsp);
	return 0;
}
int timer_delete(timer_t timer_id)
{
	dispatch_source_t ds;
	if (remove_timer_id(timer_id) == -1) {
		return -1;
	}
	ds = (dispatch_source_t)timer_id;
	dispatch_source_cancel(ds);
	dispatch_release(ds);
	return 0;
}
int timer_gettime(timer_t timer_id, struct itimerspec *itsp)
{
	dispatch_source_t ds;
	struct timer_context *tcxt;
	if (check_timer_id(timer_id) == -1) {
		return -1;
	}
	ds = (dispatch_source_t)timer_id;
	dispatch_retain(ds);
	tcxt = (struct timer_context *)dispatch_get_context(ds);
	dispatch_release(ds);
	itsp->it_interval.tv_sec = tcxt->ointerval / NSEC_PER_SEC;
	itsp->it_interval.tv_nsec= tcxt->ointerval % NSEC_PER_SEC;
/* TODO How To? */
	itsp->it_value.tv_sec = 0;
	itsp->it_value.tv_nsec= 0;
	return 0;
}
int timer_getoverrun(timer_t timer_id)
{
	long data;
	dispatch_source_t ds;
	if (check_timer_id(timer_id) == -1) {
		return -1;
	}
	ds = (dispatch_source_t)timer_id;
	dispatch_retain(ds);
	data = ((struct timer_context *)dispatch_get_context(ds))->overrun;
	dispatch_release(ds);
	if (DELAYTIMER_MAX < data)
		data = DELAYTIMER_MAX;
	return (int)data;
}
int timer_settime(timer_t timer_id, int flags, const struct itimerspec *__restrict itp,
	struct itimerspec *__restrict oitp)
{
	dispatch_source_t ds;
	struct timer_context *tcxt;
	dispatch_time_t start;
	uint64_t value;
	uint64_t interval;
	if (check_timer_id(timer_id) == -1) {
		return -1;
	}
	ds = (dispatch_source_t)timer_id;
	dispatch_retain(ds);
	tcxt = (struct timer_context *)dispatch_get_context(ds);
	value = itp->it_value.tv_sec
		* NSEC_PER_SEC + itp->it_value.tv_nsec;
	if (value == 0 && tcxt->armed) {
		dispatch_suspend(ds);
		tcxt->armed = 0;
		goto timer_settime_exit;
	} 
	switch (tcxt->clock_id) {
	case CLOCK_REALTIME:
		if (flags & TIMER_ABSTIME) {
			start = dispatch_walltime(&itp->it_value, 0);
		} else {
			start = dispatch_walltime(NULL, value);
		}
		break;
	case CLOCK_MONOTONIC:
		if (flags & TIMER_ABSTIME) {
			start = dispatch_time(value, 0);
		} else {
			start = dispatch_time(0, value);
		}
		break;
	default:
		errno = EINVAL;
		dispatch_release(ds);
		return -1;
	}
	interval = itp->it_interval.tv_sec * NSEC_PER_SEC 
		+ itp->it_interval.tv_nsec;	
	if (interval == 0) interval = UINT64_MAX;
	dispatch_source_set_timer(ds, start,
		interval, 0);
	if (!(tcxt->armed)) {
		tcxt->armed = 1;
		dispatch_resume(ds);
	}

timer_settime_exit:
	if (oitp != NULL) {
		oitp->it_interval.tv_sec = tcxt->ointerval / NSEC_PER_SEC;
		oitp->it_interval.tv_nsec= tcxt->ointerval % NSEC_PER_SEC;
		/* TODO How To? */
		oitp->it_value.tv_sec = 0;
		oitp->it_value.tv_nsec= 0;
	}
	tcxt->ointerval = interval;
	dispatch_release(ds);
	return 0;
}
/* TMR] */
