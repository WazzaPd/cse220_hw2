#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "hw2.h"

int same_strings(char* str1, char* str2){
    char* i1 = str1;
    char* i2 = str2;

    while (*i1 != '\0' || *i2 != '\0'){
        if(*i1 != *i2){
            return 0;
        }
        i1 ++;
        i2 ++;
    }

    if(*i1 != *i2){
        return 0;
    }

    return 1;
}

void print_packet(unsigned int packet[])
{
    //unsigned int * word = packet;
    unsigned int * iterator = packet;

    //binary: 1s in bits 0-9
    unsigned int length_mask = 1023;
    int length = *iterator & length_mask;

    //binary: 21st bit = 1
    unsigned int shifted_packet_type = (*iterator >> 10);
    char* packet_type;
    if(shifted_packet_type == 0){
        packet_type = "Read";
    } 
    //binary: 21st bit = 1, Write
    else if(shifted_packet_type == 1048576){
        packet_type = "Write";
    } else {
        return;
    }

    iterator++;

    unsigned int BE_mask = 15;
    int firstBE = *iterator & BE_mask;
    int lastBE = (*iterator >> 4) & BE_mask;
    unsigned int tag_mask = 255;
    int tag = (*iterator >> 8) & tag_mask;
    unsigned int requesterID_mask = 65535;
    int requesterID = (*iterator >> 16) & requesterID_mask;
    
    iterator++;

    int address_bits;
    address_bits = *iterator;

    printf("Packet Type: %s\n", packet_type);
    printf("Address: %d\n", address_bits);
    printf("Length: %d\n", length);
    printf("Requester ID: %d\n", requesterID);
    printf("Tag: %d\n", tag);
    printf("Last BE: %d\n", lastBE);
    printf("1st BE: %d\n", firstBE);
    printf("Data: ");
    if(same_strings(packet_type, "Write")){
        for(int i = 0; i<length; i++){
            iterator ++;
            int data = *iterator;
            printf("%d ", data);
        }
    }
    printf("\n");
}

void store_values(unsigned int packets[], char *memory)
{
    (void)packets;
    (void)memory;
}

unsigned int* create_completion(unsigned int packets[], const char *memory)
{
    (void)packets;
    (void)memory;
	return NULL;
}
