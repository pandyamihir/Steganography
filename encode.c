/* Header file inclusion */
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/*checking the operation type(encode / decode / none)*/
OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0) // check encoding operation
    {
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0) // check decoding operation
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

/* read & validate the encoding arguments */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0) // check it is ".bmp file or not"
    {
        encInfo->src_image_fname = argv[2]; // storing file
    }
    else
    {
        printf("Invalid file name\n");
        return e_failure;
    }

    if ((strcmp(strstr(argv[3], "."), ".txt") == 0) || (strcmp(strstr(argv[3], "."), ".c") == 0) || (strcmp(strstr(argv[3], "."), ".csv") == 0) || (strcmp(strstr(argv[3], "."), ".sh") == 0)) // check the valid file
    {
        encInfo->secret_fname = argv[3]; // storing file

        char *pstr = strstr(argv[3], ".");
        strcpy(encInfo->extn_secret_file, pstr);

        encInfo->size_secret_file_extn = strlen(encInfo->extn_secret_file); // store the secret file extn
    }
    else
    {
        printf("Invalid file name\n");
        return e_failure;
    }

    if (argv[4] != NULL) // for the output file
    {
        if (strcmp(strstr(argv[4], "."), ".bmp") == 0) // check it is ".bmp file or not"
        {
            encInfo->stego_image_fname = argv[4]; // storing file
        }
        else
        {
            printf("Invalid file name\n");
        }
    }
    else // if output file not mention, will create here
    {
        FILE *fp = fopen("output.bmp", "w"); // open the file if optional file is not entered by user
        printf("INFO : Output file not mentioned. Creating output.bmp as default.\n");
        encInfo->stego_image_fname = "output.bmp"; // store the default file
    }
}

/* function to get image size for bmp*/
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* Get The File Size*/
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

/* Open Files*/
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");

    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/*encoding*/
Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success) // open files
    {
        printf("INFO : Opening required files\n");
        printf("INFO : Opened %s\n", encInfo->src_image_fname);
        printf("INFO : Opened %s\n", encInfo->secret_fname);
        printf("INFO : Opened %s\n", encInfo->stego_image_fname);
        printf("INFO : Open files are done successfully.\n");

        printf("INFO : ###### Encoding Procedure Started ######\n");

        if (check_capacity(encInfo) == e_success) // check capacity
        {
            printf("INFO : Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);
            printf("INFO : Done. Found OK\n");

            rewind(encInfo->fptr_src_image);

            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) // copy bmp header 
            {
                printf("INFO : Copying Image Header\n");
                printf("INFO : Done\n");

                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success) // magic string
                {
                    printf("INFO : Encoding Magic String Signature\n");
                    printf("INFO : Done\n");

                    if (encode_secret_file_extn_size(encInfo->size_secret_file_extn, encInfo) == e_success) // secret file extension size
                    {
                        printf("INFO : Encoding %s Extension Size\n", encInfo->secret_fname);
                        printf("INFO : Done\n");

                        if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success) // secret file extension
                        {
                            printf("INFO : Encoding %s Extension\n", encInfo->secret_fname);
                            printf("INFO : Done\n");

                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success) // secret file size
                            {
                                printf("INFO : Encode %s Size\n", encInfo->secret_fname);
                                printf("INFO : Done\n");

                                if (encode_secret_file_data(encInfo) == e_success) // secret file data
                                {
                                    printf("INFO : Encode %s Data\n", encInfo->secret_fname);
                                    printf("INFO : Done\n");

                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) // copy remaining data
                                    {
                                        printf("INFO : Copy Remaining Image Data is done successfully.\n");
                                        printf("INFO : Done\n");
                                        printf("INFO : ###### Encoding Done SuccessFully ######\n");
                                    }
                                    else
                                    {
                                        printf("Error : Copy Remaining Image Data is not done successfully.\n");
                                        printf("###### Encoding is not done successfully. ######\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Error : Encode Secret File Data is not done successfully.\n");
                                    printf("###### Encoding is not done successfully. ######\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Error : Encode Secret File Size is not done successfully.\n");
                                printf("###### Encoding is not done successfully. ######\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Error : Encode Secret File Extension is not done successfully.\n");
                            printf("###### Encoding is not done successfully. ######\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Error : Encode Secret File Extension Size is not done successfully.\n");
                        printf("###### Encoding is not done successfully. ######\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Error : Encode Magic String is not done successfully.\n");
                    printf("###### Encoding is not done successfully. ######\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Error : Copy bmp header is not done successfully.\n");
                printf("###### Encoding is not done successfully. ######\n");
                return e_failure;
            }
        }
        else
        {
            printf("Error : Check capacity is not done successfully.\n");
            printf("###### Encoding is not done successfully. ######\n");
            return e_failure;
        }
    }
    else
    {
        printf("Error : Open files are not done successfully.\n");
        printf("###### Encoding is not done successfully. ######\n");
        return e_failure;
    }
}

/*checking capacity*/
Status check_capacity(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success) // open files
    {
        // Get image capacity and secret file size
        encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
        encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

        // Calculate required capacity
        int required_capacity = 54 + 16 + ((strlen(encInfo->extn_secret_file)) * 8) + 32 + 32 + ((encInfo->size_secret_file - 1) * 8);

        // Check if the image has enough capacity
        if (encInfo->image_capacity > required_capacity)
        {
            return e_success;
        }
        else
        {
            printf("Check capacity failed. Image does not have enough space.\n\n");
            return e_failure;
        }
    }
    else
    {
        printf("Failed to open files.\n");
        return e_failure;
    }
}

/*copy header bmp*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char ptr[54];

    fread(ptr, 54, 1, fptr_src_image); // read data from the src image

    fwrite(ptr, 54, 1, fptr_dest_image); // give data to destination image
    return e_success;
}

/* data to image encoding */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Iterate over each bit in the byte
    for (int i = 0; i < 8; i++)
    {
        // Set the least significant bit of the buffer to the corresponding bit of the data
        image_buffer[i] = ((data & (1 << (7 - i))) >> (7 - i)) | (image_buffer[i] & (~1));
    }
    return e_success; // Encoding successful
}

/* encode data to image */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
    // Iterate over each byte of data to be encoded
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from the source image file into the buffer
        fread(encInfo -> secret_data, 8, 1, fptr_src_image);
        
        // Encode the current byte of data into the least significant bit of the buffer
        encode_byte_to_lsb(data[i], encInfo -> secret_data);
        
        // Write the modified buffer to the stego image file
        fwrite(encInfo -> secret_data, 8, 1, fptr_stego_image);
    }
    return e_success; // Encoding successful
}

/*encode magic string*/
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    int i, j;
    int k = 0, n = 0;
    char buff[2 * 8];

    if (!(fread(buff, 2 * 8, 1, encInfo->fptr_src_image))) // reading data from source image
        return e_failure;

    for (i = 0; i < 2; i++)
    {
        for (j = 7; j >= 0; j--)
        {
            if ((magic_string[i] & (1 << j)))
                k = 1;

            buff[n] = (buff[n] & (-2)) | k; // clear the bit and doing or operation
            n++;
            k = 0;
        }
    }

    fwrite(buff, 16, 1, encInfo->fptr_stego_image); // write data to the stego image
    return e_success;
}

/* encode secret file extenxion */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    int i, j;
    int k = 0, n = 0;
    char buff[32];

    if (!(fread(buff, 32, 1, encInfo->fptr_src_image))) // reading data from source image
        return e_failure;

    for (i = 0; i < 4; i++)
    {
        for (j = 7; j >= 0; j--)
        {
            if ((file_extn[i] & (1 << j)))
                k = 1;

            buff[n] = (buff[n] & (-2)) | k; // clear the bit and doing or operation
            n++;
            k = 0;
        }
    }
    fwrite(buff, 32, 1, encInfo->fptr_stego_image); // write data to the stego image
    return e_success;
}

/*encode secret file extension size*/
Status encode_secret_file_extn_size(long file_size, EncodeInfo *encInfo)
{
    for (int j = 0; j < 32; j++)
    {
        char bit = (fgetc(encInfo->fptr_src_image)) & (~1); /* Actual buffer */

        // char image_byte = file_size;
                                        /* Arg 2 : file pointer */
        fputc(((file_size) & 1) | bit, encInfo->fptr_stego_image); /* Fetching LSB and updating */

        file_size = ((unsigned)file_size) >> 1;
    }

    return e_success;
}

/*encode secret file size*/
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    for (int i = 0; i < 32; i++)
    {
        int src_byte = fgetc(encInfo->fptr_src_image); 

        fputc((src_byte & 0xFE) | (file_size >> (31 - i)) & 1, encInfo->fptr_stego_image); 
    }
    return e_success;
}

/*encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // Move the file pointer to the start of the secret file
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    
    // Create a buffer to hold the secret file data
    char str[encInfo -> size_secret_file];
    
    // Read the entire secret file into the buffer
    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret); // read data from the secret data
    str[encInfo -> size_secret_file] = '\0';

    // Encode the secret file data into the source image and write to the stego image
    encode_data_to_image(str, strlen(str), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    
    return e_success; // Encoding successful
}

/*copy remaining image data*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    while (fread(&ch, 1, 1, fptr_src)) // read remaining data from the source image
    {
        fwrite(&ch, 1, 1, fptr_dest); // write remaining data to the destination image
    }
    return e_success;
}