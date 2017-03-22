/**
 * bg_message.h
 * \addtogroup bg_message Message queue
 * @{
 * These functions manage bg_messages and queue used for signaling in WiFi stack.<br>
 * bg_message_sender struct keeps list of receivers and bg_messages sent from that sender is sent to all receivers.
 *
 */

#ifndef BG_MESSAGE_H_
#define BG_MESSAGE_H_
#include <stdint.h>
#include "bg_errorcodes.h"
#include "bg_config.h"

#define BG_MESSAGE_NOFREE_BIT 0x80000000 //<! if this is set, then data parameter of message is not freed
#define BG_MESSAGE_ID_MASK 0x7fffffff

//by default assume 1Khz clock
#ifndef BG_MESSAGE_HZ
#define BG_MESSAGE_HZ 1000
#endif
//timeout in 32KHz
#define BG_TIMEOUT_32KHZ(time) (((uint64_t)time)*BG_MESSAGE_HZ/32768L)
//timeout in milliseconds
#define BG_TIMEOUT_1KHZ(time) (((uint64_t)time)*BG_MESSAGE_HZ/1000L)
// ticks to milliseconds
#define BG_TICKS_TO_MS_32KHZ(ticks)   (((uint64_t)ticks) * 1000L / BG_MESSAGE_HZ)

/**
 * Define time value size bgmessage uses
 * 64bit timer does not need to worry about wraparound
 * Bluegecko has native 32bit sleep timer, so it uses 32bit messages
 */
#if BG_MESSAGE_TIME_BITS==32
typedef uint32_t bg_message_time_t;
#define BG_MESSAGE_NO_WAIT      UINT32_MAX
#define BG_MESSAGE_OVERFLOW     (1 << 31)
#else
//default fallback to 64 bits
typedef uint64_t bg_message_time_t;
#define BG_MESSAGE_NO_WAIT      UINT64_MAX
#define BG_MESSAGE_OVERFLOW     (1 << 63)
#endif

/**
 *
 * Prototype for function to receive bg_messages<br>
 * <br>
 * <br>
 * <b>receiver_ctx</b>, data for receiver data <br>
 * <b>sender</b>, sender struct (context)<br>
 * <b>msg</b>, bg_message id<br>
 * <b>msg_data</b>, bg_message data<br>
 *
 * If function return 0 then bg_message will be forwarded for next receiver function. This is valid for most cases.
 * Nonzero return value prevents other receivers from handling bg_message.
 */
typedef int8_t(*bg_message_receiver_func)(
        void *receiver_ctx,
        void *sender, 
		uint32_t msg,
        void *msg_data);

/**
 * Message sender struct
 * Pointer for this struct is a parameter in bg_message receiver function
 */
typedef struct 
{
	 struct bg_message_receiver_node_t *receivers;//!< list of receivers
}bg_message_sender;

/**
 * Message comparison function. The first argument is the bg_message, the second is supplied by the caller.
 * Must return 0 if the arguments match, 1 otherwise.
 */
typedef int (*bg_message_comparison_func)(void*, void*);

/**
 * Reset internal tick counter. This is used in unit testing.
 */
void bg_message_reset_ticks();

/**
 * Message will be queued and delivered next time bg_message_run is called
 * NOTE: topmost bit of msg must not be set
 */
errorcode_t bg_message_send(bg_message_sender *sender,uint32_t msg,void*data);

/**
 * Queue Message, parameter is 32bits of data which is not automatically freed
 *
 */
static inline errorcode_t bg_message_send_nofree(bg_message_sender *sender,uint32_t msg,void*data)
{
    return bg_message_send(sender,msg|BG_MESSAGE_NOFREE_BIT,data);
}


/**
 * Send bg_message directly to receiver without queueing.
 */
int8_t bg_message_call(bg_message_sender *sender,uint32_t msg,void*data);

/**
 * Message will be queued and delivered after timeout but at the latest when timeout + slack expires
 * NOTE: topmost bit of msg must not be set
 */
errorcode_t bg_message_send_later_lazy(bg_message_sender *sender, uint32_t msg, void *data, bg_message_time_t timeout, bg_message_time_t slack);

/**
 * Message will be queued and delivered when timeout expires
 * NOTE: topmost bit of msg must not be set
 */
static inline errorcode_t bg_message_send_later(bg_message_sender *sender, uint32_t msg, void *data, bg_message_time_t timeout)
{
    return bg_message_send_later_lazy(sender, msg, data, timeout, 0);
}

/**
 * Queue Message, parameter is 32bits of data which is not automatically freed
 *
 */
static inline errorcode_t bg_message_send_later_nofree(bg_message_sender *sender,uint32_t msg,void*data,bg_message_time_t timeout)
{
    return bg_message_send_later_lazy(sender, msg|BG_MESSAGE_NOFREE_BIT, data, timeout, 0);
}


/**
 * Cancel queued bg_message
 */
void bg_message_cancel(const bg_message_sender *sender,uint32_t msg);

/**
 * Cancel queued bg_message that matches the comparison function.
 */
void bg_message_cancel_matching(const bg_message_sender *sender, uint32_t msgid, void* data, bg_message_comparison_func comparison);

/**
 * Remove all receivers from list
 */
void bg_message_remove_receivers(bg_message_sender *sender);
/**
 * Remove receiver from list
 */
void bg_message_remove_receiver(bg_message_sender *sender,bg_message_receiver_func receiver_func,const void *receiver_ctx);
/**
 * Add receiver to first in list. receiver_func will be called with incoming bg_messages from sender. receiver_ctx is sent to receiver_func 
 */
errorcode_t bg_message_add_receiver(bg_message_sender *sender,bg_message_receiver_func receiver_func, void *receiver_ctx);
/**
 * Add receiver data (context) for node in receiver list. 
 */
errorcode_t bg_message_set_receiver_data(const bg_message_sender *object,bg_message_receiver_func receiver_func, void *receiver_ctx);

/**
 * send queued bg_messages.<br>
 * return time for next bg_message or BG_MESSAGE_NO_WAIT if no messages waiting
 */
bg_message_time_t bg_message_run(bg_message_time_t ticks);

/**
 * 
 *  Initialize message system
 */
void bg_message_init();


/**
 * check if there is messages queued for immediate processing
 * @param ticks current_time
 * @return nonzero if messages pending
 */
int bg_message_queue_waiting(bg_message_time_t ticks);

/**
 * Returns time until next message delivery
 * @param ticks current time
 * @returns time until next queued message, BG_MESSAGE_NO_WAIT if no messages waiting
 */
bg_message_time_t bg_message_queue_wait_time(bg_message_time_t time);

/**
 * Message callback pointer
 * TODO: move this struct definition back inside bg_message.c
 */
struct bg_message_receiver_node_t
{
    struct bg_message_receiver_node_t  *next;
    bg_message_receiver_func            receiver_func;
    void                            *receiver_data;
};
#endif /* MESSAGE_H_ */
/**@}*/
