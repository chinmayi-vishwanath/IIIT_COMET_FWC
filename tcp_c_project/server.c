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

/* ── Client registry ──────────────────────────────────────────── */

typedef struct {
    int    fd;                    /* socket file descriptor, -1 = slot free */
    char   name[MAX_NAME_LEN];
    struct sockaddr_in addr;
} Client;

static Client clients[MAX_CLIENTS];
static pthread_mutex_t registry_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ── Helpers ──────────────────────────────────────────────────── */

static long now_ts(void) {
    return (long)time(NULL);
}

static int registry_add(int fd, const struct sockaddr_in *addr) {
    pthread_mutex_lock(&registry_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == -1) {
            clients[i].fd   = fd;
            clients[i].addr = *addr;
            snprintf(clients[i].name, MAX_NAME_LEN, "user%d", fd);
            pthread_mutex_unlock(&registry_mutex);
            return i;
        }
    }
    pthread_mutex_unlock(&registry_mutex);
    return -1;
}

static void registry_remove(int slot) {
    pthread_mutex_lock(&registry_mutex);
    clients[slot].fd = -1;
    memset(clients[slot].name, 0, MAX_NAME_LEN);
    pthread_mutex_unlock(&registry_mutex);
}

static int name_taken(const char *name, int skip_slot) {
    pthread_mutex_lock(&registry_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (i == skip_slot) continue;
        if (clients[i].fd != -1 &&
            strncmp(clients[i].name, name, MAX_NAME_LEN) == 0) {
            pthread_mutex_unlock(&registry_mutex);
            return 1;
        }
    }
    pthread_mutex_unlock(&registry_mutex);
    return 0;
}

static int send_msg(int fd, const Message *m) {
    ssize_t n = send(fd, m, sizeof(Message), MSG_NOSIGNAL);
    return (n == (ssize_t)sizeof(Message)) ? 0 : -1;
}

static void broadcast(const Message *m, int exclude_fd) {
    pthread_mutex_lock(&registry_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd != -1 && clients[i].fd != exclude_fd) {
            send_msg(clients[i].fd, m);
        }
    }
    pthread_mutex_unlock(&registry_mutex);
}

static void build_user_list(char *buf, size_t bufsz) {
    buf[0] = '\0';
    pthread_mutex_lock(&registry_mutex);
    int first = 1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd != -1) {
            size_t rem = bufsz - strlen(buf);
            if (rem < 3) break;
            if (!first && rem > 2) { strncat(buf, ", ", rem - 1); }
            rem = bufsz - strlen(buf);
            strncat(buf, clients[i].name, rem - 1);
            first = 0;
        }
    }
    pthread_mutex_unlock(&registry_mutex);
}

/* ── Per-client thread ────────────────────────────────────────── */

typedef struct { int fd; int slot; } ThreadArg;

static void *client_thread(void *arg) {
    ThreadArg ta = *(ThreadArg *)arg;
    free(arg);
    int fd = ta.fd;
    int slot = ta.slot;

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clients[slot].addr.sin_addr, ip, sizeof(ip));
    printf("[+] New connection  fd=%d  ip=%s\n", fd, ip);

    /* ── 1. Username handshake ──────────────────────────────── */
    Message m;
    while (1) {
        ssize_t n = recv(fd, &m, sizeof(Message), 0);
        if (n <= 0) goto disconnect;

        if (m.type == MSG_JOIN) {
            m.sender[MAX_NAME_LEN - 1] = '\0';
            if (strlen(m.sender) == 0)
                snprintf(m.sender, MAX_NAME_LEN, "user%d", fd);

            if (name_taken(m.sender, slot)) {
                Message resp = {.type = MSG_NAME_TAKEN, .timestamp = now_ts()};
                snprintf(resp.text, MAX_MSG_LEN,
                         "Name '%s' is taken. Send another /join <name>.", m.sender);
                strcpy(resp.sender, "Server");
                send_msg(fd, &resp);
            } else {
                pthread_mutex_lock(&registry_mutex);
                strncpy(clients[slot].name, m.sender, MAX_NAME_LEN - 1);
                clients[slot].name[MAX_NAME_LEN - 1] = '\0';
                pthread_mutex_unlock(&registry_mutex);

                Message resp = {.type = MSG_NAME_OK, .timestamp = now_ts()};
                strcpy(resp.sender, "Server");
                snprintf(resp.text, MAX_MSG_LEN, "Welcome, %s!", m.sender);
                send_msg(fd, &resp);
                break;
            }
        }
    }

    /* ── 2. Announce arrival ────────────────────────────────── */
    {
        Message ann = {.type = MSG_JOIN, .timestamp = now_ts()};
        strcpy(ann.sender, "Server");
        snprintf(ann.text, MAX_MSG_LEN, "%s joined the chat.", clients[slot].name);
        broadcast(&ann, fd);
        printf("    %s joined.\n", clients[slot].name);
    }

    /* ── 3. Main recv loop ──────────────────────────────────── */
    while (1) {
        ssize_t n = recv(fd, &m, sizeof(Message), 0);
        if (n <= 0) break;

        m.timestamp = now_ts();
        strncpy(m.sender, clients[slot].name, MAX_NAME_LEN - 1);
        m.sender[MAX_NAME_LEN - 1] = '\0';

        if (m.type == MSG_TEXT && m.text[0] == '/') {

            if (strncmp(m.text, "/quit", 5) == 0) break;

            if (strncmp(m.text, "/list", 5) == 0) {
                Message resp = {.type = MSG_LIST, .timestamp = now_ts()};
                strcpy(resp.sender, "Server");
                strcpy(resp.text, "Online: ");
                build_user_list(resp.text + 8, MAX_MSG_LEN - 8);
                send_msg(fd, &resp);
                continue;
            }

            if (strncmp(m.text, "/nick ", 6) == 0) {
                char newname[MAX_NAME_LEN];
                strncpy(newname, m.text + 6, MAX_NAME_LEN - 1);
                newname[MAX_NAME_LEN - 1] = '\0';
                newname[strcspn(newname, "\r\n")] = '\0';

                Message resp = {.timestamp = now_ts()};
                strcpy(resp.sender, "Server");

                if (strlen(newname) == 0 || name_taken(newname, slot)) {
                    resp.type = MSG_SERVER;
                    snprintf(resp.text, MAX_MSG_LEN,
                             "Name '%s' is unavailable.", newname);
                    send_msg(fd, &resp);
                } else {
                    char oldname[MAX_NAME_LEN];
                    strncpy(oldname, clients[slot].name, MAX_NAME_LEN - 1);
                    oldname[MAX_NAME_LEN - 1] = '\0';

                    pthread_mutex_lock(&registry_mutex);
                    strncpy(clients[slot].name, newname, MAX_NAME_LEN - 1);
                    clients[slot].name[MAX_NAME_LEN - 1] = '\0';
                    pthread_mutex_unlock(&registry_mutex);

                    resp.type = MSG_SERVER;
                    snprintf(resp.text, MAX_MSG_LEN,
                             "%s is now known as %s.", oldname, newname);
                    send_msg(fd, &resp);
                    broadcast(&resp, fd);
                    printf("    %s renamed to %s\n", oldname, newname);
                }
                continue;
            }

            Message resp = {.type = MSG_SERVER, .timestamp = now_ts()};
            strcpy(resp.sender, "Server");
            snprintf(resp.text, MAX_MSG_LEN,
                     "Unknown command. Try /list, /nick <name>, /quit.");
            send_msg(fd, &resp);
            continue;
        }

        if (m.type == MSG_TEXT) {
            printf("    [%s]: %s\n", m.sender, m.text);
            broadcast(&m, fd);
        }
    }

disconnect:
    {
        Message bye = {.type = MSG_LEAVE, .timestamp = now_ts()};
        strcpy(bye.sender, "Server");
        snprintf(bye.text, MAX_MSG_LEN, "%s left the chat.", clients[slot].name);
        printf("[-] %s disconnected  fd=%d\n", clients[slot].name, fd);
        registry_remove(slot);
        broadcast(&bye, -1);
    }
    close(fd);
    return NULL;
}

/* ── Main ─────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : SERVER_PORT;

    signal(SIGPIPE, SIG_IGN);

    for (int i = 0; i < MAX_CLIENTS; i++) clients[i].fd = -1;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port        = htons(port)
    };

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 16) < 0) {
        perror("listen");
        return 1;
    }

    printf("==============================================\n");
    printf("  TCP Chat Server  |  port %d  |  max %d users\n",
           port, MAX_CLIENTS);
    printf("==============================================\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);

        int cfd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
        if (cfd < 0) {
            perror("accept");
            continue;
        }

        int slot = registry_add(cfd, &client_addr);
        if (slot < 0) {
            Message full = {.type = MSG_SERVER, .timestamp = now_ts()};
            strcpy(full.sender, "Server");
            snprintf(full.text, MAX_MSG_LEN, "Server is full (%d/%d).",
                     MAX_CLIENTS, MAX_CLIENTS);
            send_msg(cfd, &full);
            close(cfd);
            continue;
        }

        ThreadArg *ta = malloc(sizeof(ThreadArg));
        if (!ta) {
            perror("malloc");
            close(cfd);
            registry_remove(slot);
            continue;
        }

        ta->fd = cfd;
        ta->slot = slot;

        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&tid, &attr, client_thread, ta);
        pthread_attr_destroy(&attr);
    }

    close(server_fd);
    return 0;
}
