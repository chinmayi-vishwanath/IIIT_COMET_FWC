#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_NAME_LEN   32
#define MAX_MSG_LEN    512
#define MAX_CLIENTS    64
#define SERVER_PORT    8080

/* Message types */
#define MSG_TEXT       1   /* Normal chat message          */
#define MSG_JOIN       2   /* Client joined notification   */
#define MSG_LEAVE      3   /* Client left notification     */
#define MSG_SERVER     4   /* Server announcement          */
#define MSG_LIST       5   /* /list command response       */
#define MSG_NAME_OK    6   /* Server accepted the username */
#define MSG_NAME_TAKEN 7   /* Username already in use      */

typedef struct {
    int   type;
    char  sender[MAX_NAME_LEN];
    char  text[MAX_MSG_LEN];
    long  timestamp;       /* Unix seconds */
} Message;

#endif /* PROTOCOL_H */
