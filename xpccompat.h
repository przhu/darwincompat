/*
 * simple wrapper of XPCServices for no __BLOCKS__ compiler.
 * Not eligible for copyright.
 */

#ifndef _DARWIN_COMPAT_XPC_COMPAT_H
#define _DARWIN_COMPAT_XPC_COMPAT_H
#include <xpc/xpc.h>

/*
 * First, some API can be exposed but not...(in my version, 10.7.5)
 * see Apple's orginal header for documents
 */
#ifndef __BLOCKS__
__BEGIN_DECLS
/* <xpc/connection.h> */
#define XPC_ERROR_CONNECTION_INTERRUPTED (&_xpc_error_connection_interrupted)
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT
struct _xpc_dictionary_s _xpc_error_connection_interrupted;

#define XPC_ERROR_CONNECTION_INVALID (&_xpc_error_connection_invalid)
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT
struct _xpc_dictionary_s _xpc_error_connection_invalid;

#define XPC_ERROR_TERMINATION_IMMINENT (&_xpc_error_termination_imminent)
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT
struct _xpc_dictionary_s _xpc_error_termination_imminent;

#define XPC_CONNECTION_MACH_SERVICE_LISTENER (1 << 0)

#define XPC_CONNECTION_MACH_SERVICE_PRIVILEGED (1 << 1)

typedef void (*xpc_finalizer_t)(void *value);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_WARN_RESULT
xpc_connection_t
xpc_connection_create(const char *name, dispatch_queue_t targetq);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL1 XPC_WARN_RESULT
xpc_connection_t
xpc_connection_create_mach_service(const char *name, dispatch_queue_t targetq,
	uint64_t flags);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL1 XPC_WARN_RESULT XPC_MALLOC
xpc_connection_t
xpc_connection_create_from_endpoint(xpc_endpoint_t endpoint);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL1
void
xpc_connection_set_target_queue(xpc_connection_t connection,
	dispatch_queue_t targetq);
/*
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_set_event_handler(xpc_connection_t connection, 
	xpc_handler_t handler);
 */

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_suspend(xpc_connection_t connection);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_resume(xpc_connection_t connection);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_send_message(xpc_connection_t connection, xpc_object_t message);

/*
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_send_barrier(xpc_connection_t connection, 
	dispatch_block_t barrier);
 */

/*
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_send_message_with_reply(xpc_connection_t connection,
	xpc_object_t message, dispatch_queue_t replyq, xpc_handler_t handler);
 */

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL XPC_WARN_RESULT
xpc_object_t
xpc_connection_send_message_with_reply_sync(xpc_connection_t connection,
	xpc_object_t message);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_cancel(xpc_connection_t connection);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL XPC_WARN_RESULT
const char *
xpc_connection_get_name(xpc_connection_t connection);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL XPC_WARN_RESULT
uid_t
xpc_connection_get_euid(xpc_connection_t connection);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL XPC_WARN_RESULT
gid_t
xpc_connection_get_egid(xpc_connection_t connection);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL XPC_WARN_RESULT
pid_t
xpc_connection_get_pid(xpc_connection_t connection);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL XPC_WARN_RESULT
au_asid_t
xpc_connection_get_asid(xpc_connection_t connection);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL1
void
xpc_connection_set_context(xpc_connection_t connection, void *context);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL1 XPC_WARN_RESULT
void *
xpc_connection_get_context(xpc_connection_t connection);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL1
void
xpc_connection_set_finalizer_f(xpc_connection_t connection, xpc_finalizer_t finalizer);

__END_DECLS
#endif /* #ifndef __BLOCKS__ */

__BEGIN_DECLS
/*
 * Apple doc'ed...
 */
typedef xpc_finalizer_t xpc_finalizer_f;

/* types for supporting no __BLOCKS__ compiler */
typedef void (*xpc_handler_f)(void *context, xpc_object_t object);
typedef bool (*xpc_array_applier_f)(void *context, 
		size_t index, xpc_object_t value);
typedef bool (*xpc_dictionary_applier_f)(void *context, 
		const char *key, xpc_object_t value);

/* wrappers for supporting no __BLOCKS__ compiler */
/* <xpc/connection.h> */
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_set_event_handler_f(xpc_connection_t connection, 
        void *context, xpc_handler_f handler);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_send_barrier_f(xpc_connection_t connection, 
        void *context, dispatch_function_t barrier);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_send_message_with_reply_f(xpc_connection_t connection,
        xpc_object_t message, dispatch_queue_t replyq, void *context, 
	xpc_handler_f handler);

/* <xpc/xpc.h> */
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
bool
xpc_array_apply_f(xpc_object_t xarray, void *context, 
	xpc_array_applier_f applier);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
bool
xpc_dictionary_apply_f(xpc_object_t xdict, void *context, 
	xpc_dictionary_applier_f applier);

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL2
void
xpc_set_event_stream_handler_f(const char *stream, dispatch_queue_t targetq, 
	void *context, xpc_handler_f handler);
/* END */
__END_DECLS
#endif /* #ifndef _DARWIN_COMPAT_XPC_COMPAT_H */
