/* Header file inclusion */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Read and validate Decode args from argv */
dStatus read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0) // output file
    {
        decInfo -> src_image_fname = argv[2]; // store output bmp file as source image
    }
    else
    {
        printf("Invalid file name\n");
        return d_failure;
    }

    if (argv[3] != NULL) // ".txt file / optional"
    {
        if (strcmp(strstr(argv[3], "."), ".txt") == 0 || strcmp(strstr(argv[3], "."), ".csv") == 0 || strcmp(strstr(argv[3], "."), ".c") == 0 || strcmp(strstr(argv[3], "."), ".sh") == 0)
        {
            printf("Valid\n");
            decInfo -> stego_image_fname = argv[3]; // store optional file
        }
        else
        {
            printf("Invalid file name\n");
        }
    }
    else
    {
        FILE *fp = fopen("output", "w"); // if optional file doesn't entered by user it will open by default
        decInfo -> stego_image_fname = "output"; // store optional file
    }
}

/* Get File pointers for i/p and o/p files */
dStatus open_file(DecodeInfo *decInfo)
{
    // Open source image file
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    if (decInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);
        return d_failure;
    }

    return d_success;
}

/* Perform the decoding */
dStatus do_decoding(DecodeInfo *decInfo)
{
    if(open_file(decInfo) ==  d_success)
    {
        printf("INFO : ###### Decoding Procedure Started ######\n");
        printf("INFO : Opening required files\n");
        printf("INFO : Opened %s\n", decInfo -> src_image_fname);
        
        /* variable declaration */
        char magic_string[16] = {0};
        int extn_size = 0;
        char file_extn[32] = {0};
        int file_size = 0;

        fseek(decInfo -> fptr_src_image, 54, SEEK_SET);

        if(decode_magic_string(decInfo) == d_success)// magic string
        {
            printf("INFO : Decoding Magic String Signature\n");
            printf("INFO : Done\n");
            
            fseek(decInfo -> fptr_src_image,70,SEEK_SET);
            if(decode_secret_file_extn_size(decInfo -> fptr_src_image, extn_size, decInfo) == d_success) // secret file extension size
            {
                printf("INFO : Decoding Output File Extension Size\n");
                printf("INFO : Done\n");
                
                if(decode_secret_file_extn(decInfo->fptr_src_image, file_extn, decInfo) == d_success) // secret file extension size
                {
                    printf("INFO : Decoding Output File Extension\n");
                    printf("INFO : Done\n");

                    if(open_and_merge_file(decInfo) == d_success) // open and merge file
		            {
			            printf("INFO : Open and Merge file is done successfully.\n");
                        printf("INFO : Done\n");
                        
                        if(decode_secret_file_size(decInfo->fptr_src_image, file_size, decInfo) == d_success) // secret file size
                        {
                            printf("INFO : Decoding Secret File Size\n");
                            printf("INFO : Done\n");
                            
                            if(decode_secret_file_data(decInfo) == d_success) // secret file data
                            {
                                printf("INFO : Decode Scret File Data\n");
                                printf("INFO : Done\n");
                                printf("INFO : ###### Decoding Done Successfully ######\n");
                            }
                            else
                            {
                                printf("Error : Decode Secret File Data is not done successfully.\n");
                                printf("###### Decoding is not done successfully. ######\n");
                                return d_failure;
                            }
                        } 
                        else
                        {
                            printf("Error : Decode Secret File Size is not done successfully.\n");
                            printf("###### Decoding is not done successfully. ######\n");
                            return d_failure;
                        }
                    }
                    else
                    {
                        printf("Error : Open and Merge file is not done successfully.\n");
                        printf("###### Decoding is not done successfully. ######\n");
                        return d_failure;
                    }
                }
                else
                {
                    printf("Error : Decode Secret File Extension is not done successfully.\n");
                    printf("###### Decoding is not done successfully. ######\n");
                    return d_failure;
                }
            }
            else
            {
                printf("Error : Decode Secret File Extension Size is not done successfully.\n");
                printf("###### Decoding is not done successfully. ######\n");
                return d_failure;
            }
        }
        else
        {
            printf("Error : Decode Magic String is not done successfully.\n");
            printf("###### Decoding is not done successfully. ######\n");
            return d_failure;
        }
    }
    else
    {
        printf("Error : Open files are not done successfully.\n");
        printf("###### Decoding is not done successfully. ######\n");
        return d_failure;
    }
}

/* Decode Magic String */
dStatus decode_magic_string(DecodeInfo *decInfo)
{
    /* variable declaration */
    int k=0, i, j;
    char decode_ms[3];
    char user_ms[3];

    char arr[16];
    fread(arr, 16, 1, decInfo->fptr_src_image); // read data from the source image

    for(i = 0;i < 2;i++)
    {
        decode_ms[i] = 0;
        for(j = 7; j >= 0; j--)
        {
            /* bitwise operation */
            arr[k] &= 1;
            arr[k] <<= j;
            decode_ms[i] |= arr[k]; 
            k++;
        }
    }

    decode_ms[i] = '\0';

    /* read magic string from the user */
    printf("Enter the magic string : ");
    scanf("%s", user_ms);

    /* comparing magic string */
    if(strcmp(decode_ms, user_ms) == 0)
    {
        printf("INFO : Magic Strings Matched.\n");
        return d_success;
    }
    else
    {
        printf("INFO : Magic Strings Not Matched.\n");
        return d_failure;
    }
}

/* Decode secret file extension size */
dStatus decode_secret_file_extn_size(FILE *fptr_src_image, int size, DecodeInfo *decInfo)
{
    /* variable declaration */
    char arr[32];
    size = 0;

    fread(arr, 32, 1, fptr_src_image); // read data from the source image

    for (int i = 31; i > -1; i--)
    {
        size = (size << 1) | (arr[i] & 1); // bitwise operation
    }

    decInfo -> size_secret_file_extn = size; // store the extension size

    return d_success;
}

/* Decode secret file extension */
dStatus decode_secret_file_extn(FILE *fptr_src_image, char *file_extn, DecodeInfo *decInfo)
{
    /* variable declaration */
    int k=0, i, j;
    char extn_file[4];
    char arr[32];

    fread(arr, 32, 1, fptr_src_image); // read data from the source image

    for(i = 0; i < decInfo -> size_secret_file_extn; i++)
    {
        extn_file[i] = 0;
        for(j = 7; j >= 0; j--)
        {
            /* bitwise operation */
            arr[k] &= 1;
            arr[k] <<= j;
            extn_file[i] |= arr[k];
            k++;
        }
    }

    extn_file[i] = '\0';
    
    return d_success;
}

dStatus open_and_merge_file(DecodeInfo *decInfo)
{
    int fname_len = strlen(decInfo->stego_image_fname);
    int extn_len = strlen(decInfo->extn_file);

    // Allocate memory for the full file name
    char *full_fname = malloc(fname_len + extn_len + 1);

    if (full_fname == NULL)
    {
        printf("Memory allocation failed.\n");
        return d_failure;
    }

    // Build the full file name
    strcpy(full_fname, decInfo -> stego_image_fname); // copy file name to the other file
    strcat(full_fname, ".txt"); // concatinate operation

    printf("%s\n", full_fname);

    // Open the file
    decInfo -> fptr_stego_image = fopen(full_fname, "w");
    printf("INFO : Opened %s\n", full_fname);


    // Free the allocated memory for file name
    free(full_fname);

    if (decInfo -> fptr_stego_image == NULL)
    {
        printf("Error opening the file: %s\n", full_fname);
        return d_failure;
    }

    return d_success;
}

/* Decode secret file size */
dStatus decode_secret_file_size(FILE *fptr_src_image, int size, DecodeInfo *decInfo)
{
    /* variable declaration */
    int file_size = 0;

    for (int i = 0; i < 32; i++)
    {
        int byte = fgetc(fptr_src_image);

        // Extract the least significant bit
        int bit = byte & 1;

        // Shift the bit to its correct position and add it to file_size
        file_size = (file_size << 1) | bit;
    }

    decInfo -> size_stego_file = file_size; // store file size

    return d_success;
}

/* Decode lsb to byte */
dStatus decode_lsb_to_byte(char *S, unsigned char *string)  
{
    *S = 0;  // Initialize the byte to 0
    for(int i = 0; i < 8; i++)
    {
        *S |= ((string[i] & 1) << (7 - i));  // Extract the LSB from each byte and shift it into the correct position
    }

    return e_success;  // Return success
}

/* Decode secret file data */
dStatus decode_secret_file_data(DecodeInfo *decInfo)
{
    /* Decode secret file data */
    char string[8];  // Buffer to read 8 bytes at a time
    char s;  // Buffer to store decoded byte

    for(int i = 0; i < decInfo -> size_stego_file; i++)
    {
        fread(string, 8, 1, decInfo -> fptr_src_image);  // Read 8 bytes from the source image
        decode_lsb_to_byte(&s, string);  // Decode the LSB to a byte
        fwrite(&s, 1, 1, decInfo -> fptr_stego_image);  // Write the decoded byte to the stego image file
    }
    
    return e_success;  // Return success
}