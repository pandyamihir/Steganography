#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure
} Status;

/* dStatus will be used in fn. return type */
typedef enum
{
    d_success,
    d_failure
} dStatus;

/* OperationType will be used in fn. return type */
typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif