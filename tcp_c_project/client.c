#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include "protocol.h"

static int server_fd = -1;

static void print_msg(const Message *m) {
    char timebuf[16];
    time_t ts = (time_t)m->timestamp;
    struct tm *tm = localtime(&ts);
    strftime(timebuf, sizeof(timebuf), "%H:%M", tm);

    switch (m->type) {
        case MSG_TEXT:
            printf("\r\033[K[%s] \033[1m%s\033[0m: %s\n",
                   timebuf, m->sender, m->text);
            break;
        case MSG_JOIN:
            printf("\r\033[K\033[32m*** %s\033[0m\n", m->text);
            break;
        case MSG_LEAVE:
            printf("\r\033[K\033[33m*** %s\033[0m\n", m->text);
            break;
        case MSG_SERVER:
        case MSG_LIST:
            printf("\r\033[K\033[36m[Server] %s\033[0m\n", m->text);
            break;
        case MSG_NAME_OK:
            printf("\r\033[K\033[32m[Server] %s\033[0m\n", m->text);
            break;
        case MSG_NAME_TAKEN:
            printf("\r\033[K\033[31m[Server] %s\033[0m\n", m->text);
            break;
    }

    printf("> ");
    fflush(stdout);
}

static void *recv_thread(void *arg) {
    (void)arg;
    Message m;

    while (1) {
        ssize_t n = recv(server_fd, &m, sizeof(Message), 0);
        if (n <= 0) {
            printf("\r\033[K\033[31m[Disconnected from server]\033[0m\n");
            close(server_fd);
            server_fd = -1;
            exit(0);
        }
        print_msg(&m);
    }
    return NULL;
}

static int send_msg(const Message *m) {
    ssize_t n = send(server_fd, m, sizeof(Message), MSG_NOSIGNAL);
    return (n == (ssize_t)sizeof(Message)) ? 0 : -1;
}

int main(int argc, char *argv[]) {
    const char *host = (argc > 1) ? argv[1] : "127.0.0.1";
    int port = (argc > 2) ? atoi(argv[2]) : SERVER_PORT;

    signal(SIGPIPE, SIG_IGN);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_port   = htons(port)
    };

    if (inet_pton(AF_INET, host, &saddr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address: %s\n", host);
        return 1;
    }

    if (connect(server_fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("connect");
        return 1;
    }

    printf("Connected to %s:%d\n", host, port);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

    char name[MAX_NAME_LEN];
    printf("Enter your display name: ");
    fflush(stdout);

    if (!fgets(name, sizeof(name), stdin)) return 1;
    name[strcspn(name, "\r\n")] = '\0';
    if (strlen(name) == 0) strcpy(name, "anonymous");

    Message join = {.type = MSG_JOIN, .timestamp = (long)time(NULL)};
    strncpy(join.sender, name, MAX_NAME_LEN - 1);
    join.sender[MAX_NAME_LEN - 1] = '\0';
    snprintf(join.text, MAX_MSG_LEN, "%s wants to join.", name);
    send_msg(&join);

    while (1) {
        Message resp;
        ssize_t n = recv(server_fd, &resp, sizeof(Message), 0);
        if (n <= 0) {
            fprintf(stderr, "Server closed connection.\n");
            return 1;
        }

        if (resp.type == MSG_NAME_OK) {
            printf("\033[32m%s\033[0m\n", resp.text);
            break;
        } else if (resp.type == MSG_NAME_TAKEN) {
            printf("\033[31m%s\033[0m\n", resp.text);
            printf("Try another name: ");
            fflush(stdout);

            if (!fgets(name, sizeof(name), stdin)) return 1;
            name[strcspn(name, "\r\n")] = '\0';
            if (strlen(name) == 0) strcpy(name, "anonymous");

            strncpy(join.sender, name, MAX_NAME_LEN - 1);
            join.sender[MAX_NAME_LEN - 1] = '\0';
            send_msg(&join);
        }
    }

    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("Commands: /list  /nick <name>  /quit\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, recv_thread, NULL);
    pthread_attr_destroy(&attr);

    char line[MAX_MSG_LEN];
    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        Message m = {.timestamp = (long)time(NULL)};
        strncpy(m.sender, name, MAX_NAME_LEN - 1);
        m.sender[MAX_NAME_LEN - 1] = '\0';
        strncpy(m.text, line, MAX_MSG_LEN - 1);
        m.text[MAX_MSG_LEN - 1] = '\0';
        m.type = MSG_TEXT;

        if (strncmp(line, "/nick ", 6) == 0) {
            char newname[MAX_NAME_LEN];
            strncpy(newname, line + 6, MAX_NAME_LEN - 1);
            newname[MAX_NAME_LEN - 1] = '\0';
            send_msg(&m);
            strncpy(name, newname, MAX_NAME_LEN - 1);
            name[MAX_NAME_LEN - 1] = '\0';
            continue;
        }

        if (strcmp(line, "/quit") == 0) {
            send_msg(&m);
            break;
        }

        if (send_msg(&m) < 0) {
            fprintf(stderr, "Send failed.\n");
            break;
        }
    }

    printf("Goodbye!\n");
    if (server_fd != -1) close(server_fd);
    return 0;
}
