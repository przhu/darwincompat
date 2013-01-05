/*
 * This file is for compatibility only.
 * Not eligible for copyright.
 */

#include "xpccompat.h"

#ifndef __BLOCKS__
#error "this wrapper can only be compiled under __BLOCKS__ support"
#endif

/* <xpc/connection.h> */
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_set_event_handler_f(xpc_connection_t connection, 
        void *context, xpc_handler_f handler)
{
	xpc_connection_set_event_handler(connection, ^(xpc_object_t object) {
			handler(context, object);
		});
}

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_send_barrier_f(xpc_connection_t connection, 
        void *context, dispatch_function_t barrier)
{
	xpc_connection_send_barrier(connection, ^(void) {
			barrier(context);
		});
}

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
void
xpc_connection_send_message_with_reply_f(xpc_connection_t connection,
        xpc_object_t message, dispatch_queue_t replyq, void *context, 
	xpc_handler_f handler)
{
	xpc_connection_send_message_with_reply(connection, message, replyq,
		^(xpc_object_t object) {
			handler(context, object);
		});
}

/* <xpc/xpc.h> */
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
bool
xpc_array_apply_f(xpc_object_t xarray, void *context, 
	xpc_array_applier_f applier)
{
	return xpc_array_apply(xarray, ^(size_t index, xpc_object_t value) {
			return applier(context, index, value);
		});
}

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL_ALL
bool
xpc_dictionary_apply_f(xpc_object_t xdict, void *context, 
	xpc_dictionary_applier_f applier)
{
	return xpc_dictionary_apply(xdict, ^(const char *key, xpc_object_t value) {
			return applier(context, key, value);
		});
}

__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0)
XPC_EXPORT XPC_NONNULL2
void
xpc_set_event_stream_handler_f(const char *stream, dispatch_queue_t targetq, 
	void *context, xpc_handler_f handler)
{
	xpc_set_event_stream_handler(stream, targetq, ^(xpc_object_t object) {
			handler(context, object);
		});
}
/* END */
