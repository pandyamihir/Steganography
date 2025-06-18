#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

#define MAX_BUF_SIZE 8
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info - Input image information*/
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Secret File Info */
    char *secret_fname; 
    FILE *fptr_secret;
    int size_secret_file_extn;
    char extn_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE]; // secret message
    long size_stego_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;

/* Read and validate Decode args from argv */
dStatus read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
dStatus do_decoding(DecodeInfo *decInfo); // to performing encoding operation

/* Get File pointers for i/p and o/p files */
dStatus open_file(DecodeInfo *decInfo);

/* Get file size */
uint get_f_size(FILE *fptr);

/*Decode lsb to byte*/
dStatus decode_lsb_to_byte(char *S, unsigned char *string);

/* Decode Magic String */
dStatus decode_magic_string(DecodeInfo *decInfo);

/* Decode image to data */
dStatus decode_data_from_image(FILE *fptr_src_image, char *data, int size);

/* Decode secret file extension size */
dStatus decode_secret_file_extn_size(FILE *fptr_src_image, int size, DecodeInfo *decInfo);

/* Decode secret file extension */
dStatus decode_secret_file_extn(FILE *fptr_src_image, char *file_extn, DecodeInfo *decInfo);

/* Open and Merge file*/
dStatus open_and_merge_file(DecodeInfo *decInfo);

/* Decode secret file size */
dStatus decode_secret_file_size(FILE *fptr_src_image, int size, DecodeInfo *decInfo);

/* Decode secret file data*/
dStatus decode_secret_file_data(DecodeInfo *decInfo);

#endif