/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   sign_attach.c
 * Author: Lucky138
 *
 * Created on September 10, 2018, 11:35 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100
#define FW_LIMIT 2


int parse_CONFIG_FILE(FILE *input_f)
{
    int rc = EXIT_SUCCESS;
    int i = 0;
    int j = 0;
    int ch = 0;
    int len = 0;
    char buffer[100];
    const char *server_buffer;
    const char *file_buffer;
    const char *user_buffer;
    while (1) {
        ch = getc(input_f);
        if (ch != EOF && j < 2) {
            buffer[i] = ch;
            i++;
            if (ch == '\n') {
                i = 0;
                len = strlen(buffer);
                buffer[len] = '\0';
                if (j == 0) {
                    server_buffer = buffer;
                    printf("Server: %s len:%d\n", server_buffer, len);
                } else if (j == 1){
                    file_buffer = buffer;
                    printf("File: %s len:%d\n", file_buffer, len);
                }
                j++;
                //buffer[len - 1]= '\0';
                memset(buffer, '\0',len );
            }
        } else if(ch != EOF && j == 2){
            buffer[i] = ch;
            i++;
            if (ch == '\n') {
                printf("2Print correct format! %d\n", j);
                rc = EXIT_FAILURE;
                break;
            }
        } else {
            if( j == 2){
                len = strlen(buffer);
                buffer[len] = '\0';
                user_buffer = buffer;
                printf("User: %s len:%d\n", user_buffer, len);
            }
            printf("Print correct format! %d\n", j);
            rc = EXIT_FAILURE;
            break;
        }
                
    }
    
    
    return rc;
}
typedef struct input_data {
    const char *char_array_list[FW_LIMIT];
    int fw_count;
    const char *dir;
}INPUT_DATA;


int char_array(const char *input, INPUT_DATA *input_data){
    int rc = EXIT_SUCCESS; 
    char *ptr;
    char *saveptr;
    char *directory_FW;
    int string_size = 0;
    int limit_size = 0;
    
    
    string_size = strlen(input);
    if (string_size != 0) {
        
        ptr = strtok_r((char *)input, " \t", &saveptr);
        if (ptr == NULL) {
            printf("No delimiter character found!\n");
        } else {
            if (input_data->fw_count < FW_LIMIT){
                printf("ptr: %s:%d\n", ptr, strlen(ptr));
                limit_size = strlen(input_data->dir) + strlen(ptr);
                directory_FW = malloc(limit_size);
                sprintf(directory_FW,"%s%s",input_data->dir, ptr);
                printf("1. %d:%d -- FW_directory: %s\n", limit_size, strlen(directory_FW), directory_FW);
                
                input_data->char_array_list[input_data->fw_count] = directory_FW;
                //free(directory_FW);
                
                input_data->fw_count++;
                
                
                ptr = strtok_r(NULL, " \t", &saveptr);
                printf("ptr: %s:%d\n", ptr, strlen(ptr));
                
                directory_FW = malloc(strlen(input_data->dir) + strlen(ptr));
                sprintf(directory_FW,"%s%s",input_data->dir, ptr);
                input_data->char_array_list[input_data->fw_count] = directory_FW;
                input_data->fw_count++;
            } else {
                printf("FW limit of %d\n", FW_LIMIT );
            }
        }
    }
    
  
    return rc;
}

int print_char_array(INPUT_DATA *input_data)
{
    int rc = EXIT_SUCCESS; 
    int i = 0;
    int len = 0;
    while (i < input_data->fw_count) {
        len = strlen(input_data->char_array_list[i]);
        printf("FW %d -- %d: %s\n", i,len, input_data->char_array_list[i]);
        i++;
    }
    
    //concatinating all FW
    //printf("2. size: %d\n", sizeof(input_data->char_array_list));
    char *FWS = malloc(sizeof(input_data->char_array_list));
    int j = 0;
    while (j < input_data->fw_count) {
        strncat(FWS,input_data->char_array_list[j], strlen(input_data->char_array_list[j]));
        j++;
    }
    printf("FWS: %s", FWS);
    
    
    return rc;
}
/*
 * 
 */
int main(int argc, char *argv[]) {
    
    int rc = EXIT_SUCCESS;
    INPUT_DATA *input_data = (INPUT_DATA *)malloc(sizeof(*input_data));
    FILE *input_f;
    if (input_data) {
        memset(input_data, '\0', sizeof(INPUT_DATA));
        input_data->fw_count = 0;
        input_data->dir = "D:\\sample\\directory\\";
        //printf("%s", input_data->dir);
        if (argc == 2) {

            //input_f = fopen((const char *)argv[1],"r");
            //if (input_f != NULL){
                printf("valid!\n");
                //rc = parse_CONFIG_FILE(input_f);
                rc = char_array((const char *)argv[1], input_data);
                if (rc != EXIT_SUCCESS) {
                    printf("Error!\n");
                    rc = EXIT_FAILURE;
                } else {
                    rc = print_char_array(input_data);
                }
          //  } else {
          //      printf("invalid!\n");
          //  }


        } else {
            printf("Input out of bounds!\n");
        }
    } else {
        printf("Error in malloc allocation!");
    }
    
    
    return rc;
    
    
}

