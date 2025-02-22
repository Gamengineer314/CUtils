#ifndef THROW_H
#define THROW_H
#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>

void _throw_error(const char* err, ...);
void _throw(int code);
void _throw_msg(int code, const char* msg, ...);
void _throw_c(int code);
void _throw_c_msg(int code, const char* msg, ...);

// Throw a given error
#define THROW_ERR(err, ...) _throw_error("Error !\n" err "\n", ##__VA_ARGS__)

// Throw an error if func is not 0. errno is used for the error code.
#define THROW(func) if (func) _throw(errno);
// Throw an error with a given message if func is not 0. errno is used for the error code.
#define THROW_MSG(func, msg, ...) if (func) _throw_msg(errno, msg, ##__VA_ARGS__);

// Throw an error if func is not 0. func is used for the error code.
#define THROW_R(func) errno = func; if (errno) _throw(errno);
// Throw an error with a given message if func is not 0. func is used for the error code.
#define THROW_R_MSG(func, msg, ...) errno = func; if (errno) _throw_msg(errno, msg, ##__VA_ARGS__);

// Throw an error if func is not 0. func is used for the error code but is not converted to a string.
#define THROW_C(func) errno = func; if (errno) _throw_c(errno);
// Throw an error with a given message if func is not 0. func is used for the error code but is not converted to a string.
#define THROW_C_MSG(func, msg, ...) errno = func; if (errno) _throw_c_msg(errno, msg, ##__VA_ARGS__);

// Throw an error if func is 0. errno is used for the error code.
#define THROW_N(func) if (!func) _throw(errno);
// Throw an error with a given message if func is 0. errno is used for the error code.
#define THROW_N_MSG(func, msg, ...) if (!func) _throw_msg(errno, msg, ##__VA_ARGS__);

// Execute func and throw an error if err is not 0. errno is used for the error code.
#define THROW_P(func, err) func; if (err) _throw(errno)
// Execute func and throw an error with a given message if err is not 0. errno is used for the error code.
#define THROW_P_MSG(func, err, msg, ...) func; if (err) _throw_msg(errno, msg, ##__VA_ARGS__)

// Execute func and throw an error if err is not 0. err is used for the error code.
#define THROW_PR(func, err) func; if (err) _throw(err)
// Execute func and throw an error with a given message if err is not 0. err is used for the error code.
#define THROW_PR_MSG(func, err, msg, ...) func; if (err) _throw_msg(err, msg, ##__VA_ARGS__)

// Execute func and throw an error if err is not 0. err is used for the error code but is not converted to a string.
#define THROW_PC(func, err) func; if (err) _throw_c(err)
// Execute func and throw an error with a given message if err is not 0. err is used for the error code but is not converted to a string.
#define THROW_PC_MSG(func, err, msg, ...) func; if (err) _throw_c_msg(err, msg, ##__VA_ARGS__)

// Execute func and throw an error if err is 0. errno is used for the error code.
#define THROW_PN(func, err) func; if (!err) _throw(errno)
// Execute func and throw an error with a given message if err is 0. errno is used for the error code.
#define THROW_PN_MSG(func, err, msg, ...) func; if (!err) _throw_msg(errno, msg, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif