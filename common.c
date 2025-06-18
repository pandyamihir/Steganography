/* Header file inclusion */
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[], ...)
{
    /*variable declaration*/
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    if(argc == 1) // if only "./a.out" is there in command line
    {
        printf("%s : Encoding : %s -e <.bmp file> <.txt file> [output file]\n", argv[0], argv[0]);
        printf("%s : Decoding : %s -d <.bmp file> [output file]\n", argv[0], argv[0]);
    }

    if(argc>=4) // argument count should be greater than 4
    {
        int res = check_operation_type(argv); // function calling for the operation type(-e / -d)
        
        if(res==0) // Encoding operation
        {
            printf("INFO : Encode operation\n"); 
            read_and_validate_encode_args(argv, &encInfo); // calling read & validation function
            do_encoding(&encInfo); // calling do_encoding function
        }
    }
    else // Validation part for command line arguments
    {
        if(strcmp(argv[1], "-e")  == 0 && argc<4) 
        {
            printf("%s : Encoding : %s -e <.bmp file> <.txt file> [output file]\n", argv[0], argv[0]);
        }
        else
        {
            printf("INFO : Unsupported Operation\n");
        }
    }
    

    if(argc>=3) // argument count should be greater than 3
    {
        int res = check_operation_type(argv); // function calling for the operation type(-e / -d)

        if(res==1) // Decoding operation
        {
            printf("INFO : Decode operation\n");
            read_and_validate_decode_args(argv, &decInfo); // calling read & validation function
            do_decoding(&decInfo); // calling do_decoding function
        }
    }
    
    // else // Validation part for command line arguments
    // {
    //     if(strcmp(argv[1], "-d")  == 0) 
    //     {
    //         if(argc>=2)
    //         printf("%s : Decoding : %s -d <.bmp file> [output file]\n", argv[0], argv[0]);
    //     }
    //     else
    //     {
    //         printf("INFO : Unsupported Operation\n");
    //     }
    // }

    return 0;
}