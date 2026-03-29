#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <unistd.h>
#include <reent.h>
#include <stdint.h>

/*
 * AndraOS System Call Registers:
 * %rax: syscall number
 * %rdi: arg0
 * %rsi: arg1
 * %rdx: arg2
 */

_READ_WRITE_RETURN_TYPE _read (int file, void *ptr, size_t len) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(0LL), "D"((uintptr_t)file), "S"((uintptr_t)ptr), "d"((uintptr_t)len)
        : "memory"
    );
    return (_READ_WRITE_RETURN_TYPE)ret;
}

_READ_WRITE_RETURN_TYPE _write(int file, const void *ptr, size_t len) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(1LL), "D"((uintptr_t)file), "S"((uintptr_t)ptr), "d"((uintptr_t)len)
        : "memory"
    );
    return (_READ_WRITE_RETURN_TYPE)ret;
}

int _open(const char *pathname, int flags, ...) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(2LL), "D"((uintptr_t)pathname), "S"((uintptr_t)flags)
        : "memory"
    );
    return (int)ret;
}

int _close(int fd) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(3LL), "D"((uintptr_t)fd)
        : "memory"
    );
    return (int)ret;
}

_off_t _lseek(int fd, _off_t offset, int whence) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(8LL), "D"((uintptr_t)fd), "S"((uintptr_t)offset), "d"((uintptr_t)whence)
        : "memory"
    );
    return (_off_t)ret;
}

int _fstat(int fd, struct stat *st) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(5LL), "D"((uintptr_t)fd), "S"((uintptr_t)st)
        : "memory"
    );
    return (int)ret;
}


int _stat(const char *pathname, struct stat *st) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(4LL), "D"((uintptr_t)pathname), "S"((uintptr_t)st)
        : "memory"
    );
    return (int)ret;
}

void * _sbrk(ptrdiff_t incr) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(12LL), "D"((uintptr_t)incr)
        : "memory"
    );
    return (void *)(uintptr_t)ret;
}

int _isatty(int fd) {
    return 1;
}

int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

int _getpid(void) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(39LL)
        : "memory"
    );
    return (int)ret;
}

void _exit(int n) {
    __asm__ __volatile__ (
        "int $128"
        :
        : "a"(60LL), "D"((uintptr_t)n)
        : "memory"
    );
    while(1);
}

int _fork(void) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(57LL)
        : "memory"
    );
    return (int)ret;
}

int _execve(const char *pathname, char *const argv[], char *const envp[]) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(59LL), "D"((uintptr_t)pathname), "S"((uintptr_t)argv), "d"((uintptr_t)envp)
        : "memory"
    );
    return (int)ret;
}

int _wait(int *status) {
    long long ret;
    __asm__ __volatile__ (
        "int $128"
        : "=a"(ret)
        : "a"(61LL), "D"((uintptr_t)status)
        : "memory"
    );
    return (int)ret;
}

int _link(const char *oldpath, const char *newpath) {
    errno = EMLINK;
    return -1;
}

int _unlink(const char *pathname) {
    errno = ENOENT;
    return -1;
}

clock_t _times(struct tms *buf) {
    return (clock_t)-1;
}

int _gettimeofday(struct timeval *tv, void *tz) {
    return -1;
}

// === WRAPPERS BARE-METAL ===
_READ_WRITE_RETURN_TYPE read (int file, void *ptr, size_t len) { return _read(file, ptr, len); }
_READ_WRITE_RETURN_TYPE write(int file, const void *ptr, size_t len) { return _write(file, ptr, len); }
int open(const char *pathname, int flags, ...) { return _open(pathname, flags); }
int close(int fd) { return _close(fd); }
_off_t lseek(int fd, _off_t offset, int whence) { return _lseek(fd, offset, whence); }
int fstat(int fd, struct stat *st) { return _fstat(fd, st); }
int stat(const char *pathname, struct stat *st) { return _stat(pathname, st); }
void * sbrk(ptrdiff_t incr) { return _sbrk(incr); }
int isatty(int fd) { return _isatty(fd); }
int kill(int pid, int sig) { return _kill(pid, sig); }
int getpid(void) { return _getpid(); }
// void exit(int n) { _exit(n); } // Dihindari karena konflik dengan stdlib exit
int fork(void) { return _fork(); }
int execve(const char *pathname, char *const argv[], char *const envp[]) { return _execve(pathname, argv, envp); }
int wait(int *status) { return _wait(status); }
int link(const char *oldpath, const char *newpath) { return _link(oldpath, newpath); }
int unlink(const char *pathname) { return _unlink(pathname); }
clock_t times(struct tms *buf) { return _times(buf); }
int gettimeofday(struct timeval *tv, void *tz) { return _gettimeofday(tv, tz); }