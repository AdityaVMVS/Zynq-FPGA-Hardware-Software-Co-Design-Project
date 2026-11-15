#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

static uint32_t decode(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

int main(int argc, char **argv)
{
    const char *path = argc > 1 ? argv[1] : "/dev/multiplier";
    unsigned char tx[8], rx[12];
    unsigned int failures = 0;
    int fd = open(path, O_RDWR);
    if (fd < 0) { perror(path); return 1; }
    for (uint32_t a = 0; a <= 16; ++a) {
        for (uint32_t b = 0; b <= 16; ++b) {
            for (unsigned int k = 0; k < 4; ++k) {
                tx[k] = (unsigned char)(a >> (8 * k));
                tx[k + 4] = (unsigned char)(b >> (8 * k));
            }
            ssize_t n = write(fd, tx, sizeof(tx));
            if (n != (ssize_t)sizeof(tx)) {
                if (n < 0) perror("write");
                else fprintf(stderr, "Short write: %zd bytes\n", n);
                close(fd); return 1;
            }
            n = read(fd, rx, sizeof(rx));
            if (n != (ssize_t)sizeof(rx)) {
                if (n < 0) perror("read");
                else fprintf(stderr, "Short read: %zd bytes\n", n);
                close(fd); return 1;
            }
            if (decode(rx) != a || decode(rx + 4) != b || decode(rx + 8) != a * b) {
                ++failures;
                fprintf(stderr, "%u * %u: read %u, %u, %u\n",
                        a, b, decode(rx), decode(rx + 4), decode(rx + 8));
            }
        }
    }
    if (close(fd) < 0) { perror("close"); return 1; }
    printf("289 cases: %u passed, %u failed\n", 289 - failures, failures);
    return failures ? 1 : 0;
}
