#ifndef BG_CONFIG_H_
#define BG_CONFIG_H_

#define MESSAGE_QUEUE_SZ    32
#define MESSAGE_QUEUE_MASK  31

//BGStack configuration
#define BG_STACK_CONNECTIONS 4

//dummy sizes, needs to be removed after classic bt is completely separated from stack
#define BGSTACK_HCI_DATA_SIZE 1
#define BGSTACK_HCI_SMALL_DATA_SIZE 1
#define BGSTACK_MAX_INCOMING_ACL BG_STACK_CONNECTIONS
#define BGSTACK_EIR_SIZE 0
#define BGSTACK_L2CAP_MTU_IN_DEFAULT    672
#define BGSTACK_MAX_LOCAL_NAME_SIZE     2
#define BGSTACK_MAX_REMOTE_NAME_SIZE    2
#define BGSTACK_ATT_MTU_MAX 58

#define BG_MESSAGE_HZ 32768L

#define BG_EVENT_HANDLERS_MAX 5
#define BG_EVENT_MESSAGE 0x12345678 //message sent when event arrives

//BG Endpoint configuration
#define BG_ENDPOINT_MSG_BASE 0x10000

//use endpoint events
#define BG_ENDPOINT_EVENTS

//enable debugging if not disabled
#ifndef NDEBUG
#define BG_DEBUG
#endif

//LE only config
//bgbuf config
#define BGBUF_DATA_SIZE              (69)
#define BGBUF_HEADER_RESERVE         (9)
#define BGBUF_TAILER_RESERVE         (0)
#define BGBUF_GENERAL_DATA_SIZE      (BGBUF_DATA_SIZE-BGBUF_HEADER_RESERVE-BGBUF_TAILER_RESERVE)
#define HCI_HDR_SIZE 0

//TODO: this should by one byte in bluegecko
typedef uint8_t bgbuf_size_t;
#define BGBUF_IN_RESERVE       4        /*BGBUFS reserved for incoming HCI*/
//bgbuf config ends

#define BG_POOL_HCI_SZ      0
#define BG_POOL_HCI_NUM     0

//is dynamically allocated at boot
//#define BG_POOL_ACL_NUM     BG_STACK_CONNECTIONS
#define BG_POOL_ACL_SZ      sizeof(struct acl_link)

#define BG_POOL_BGBUF_NUM   30
#define BG_POOL_BGBUF_SZ    (sizeof(bgbuf)+BGBUF_DATA_SIZE+HCI_HDR_SIZE)


#define MAX_HANDLER_NODES       20
#define ALLOC_POOL_MSG_SZ    sizeof( struct message_listener_node_t)


#define POOLS (ALLOC_POOL1_SZ*ALLOC_POOL1_NUM+ALLOC_POOL2_SZ*ALLOC_POOL2_NUM+ALLOC_POOL3_SZ*ALLOC_POOL3_NUM)
#define BUFS (BGBUF_SIZE*(BGBUF_OUT_NUM+BGBUF_IN_NUM))
#define UBTS (BG_POOL_ACL_NUM*BG_POOL_ACL_SZ+MAX_L2CAP_CONNS*ALLOC_POOL_L2CAP_SZ+MAX_RFCOMM_CHAN*ALLOC_POOL_RFCOMM_SZ+MAX_HANDLER_NODES*ALLOC_POOL_MSG_SZ+BG_POOL_HCI_SZ*BG_POOL_HCI_NUM)


// Following should contain all items that are  not multiples of four
#define ALLOC_ITEMS (BG_POOL_HCI_NUM+MAX_L2CAP_CONNS+MAX_RFCOMM_CHAN+MAX_HANDLER_NODES+BGBUF_OUT_NUM+BGBUF_IN_NUM)
// This is upper limit of waste (about 120 bytes too much, if we need sqeeze this in we will need to
#define ALLOC_WASTE ((ALLOC_ITEMS)*3)
#define ALLOC_HEAP_SIZE (POOLS+BUFS+UBTS+ALLOC_WASTE)
#define STACK_SIZE 0x400

#if FEATURE_DEBUG && 0
#if FEATURE_STDLIB
#define DEBUGP(...) {fprintf(stderr,"%s:%d:%s\t",__FILE__,__LINE__,__FUNCTION__);fprintf(stderr,__VA_ARGS__);fflush(stderr);}
#define DEBUGPR(...) {fprintf(stderr,__VA_ARGS__);}

#else
#define DEBUGP(...) {tfp_printf("%s:%d:%s\t",__FILE__,__LINE__,__FUNCTION__);tfp_printf(__VA_ARGS__);}
#define DEBUGPR(...) {tfp_printf(__VA_ARGS__);}
#endif
#else
#define DEBUGP(...)
#define DEBUGPR(...)
#endif


#endif /* CONFIG_H_ */

