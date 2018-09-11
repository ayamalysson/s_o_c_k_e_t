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
/*
 * 
 */
int main(int argc, char *argv[]) {
    
    int rc = EXIT_SUCCESS;
    FILE *input_f;
    if (argc == 2) {
        
        input_f = fopen((const char *)argv[1],"r");
        if (input_f != NULL){
            printf("valid!\n");
            rc = parse_CONFIG_FILE(input_f);
        } else {
            printf("invalid!\n");
        }
                
                
    } else {
        printf("Input out of bounds!\n");
    }
    
    
    return rc;
    
    
   
}

