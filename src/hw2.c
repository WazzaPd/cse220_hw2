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
    unsigned int * iterator = packets;
    while( (*iterator >> 10) == 1048576){
        int length = (*iterator) & 1023;
        iterator ++;
        unsigned int BE_mask = 15;
        int firstBE = (*iterator) & BE_mask;
        int lastBE = (*iterator >> 4) & BE_mask;
        iterator ++;
        int address = (*iterator);
        for(int row =0 ; row < length; row ++){
            iterator++;
            if(row == 0){
                int firstBE_0 = (firstBE & 1);
                int firstBE_1 = (firstBE & 1 << 1);
                int firstBE_2 = (firstBE & 1 << 2);
                int firstBE_3 = (firstBE & 1 << 3);

                if(firstBE_0){
                    memory[address + (row*4)] = (*iterator) & 511;
                }
                if(firstBE_1){
                    memory[address + 1 + (row*4)] = (*iterator >> (8*1)) & 511;
                }
                if(firstBE_2){
                    memory[address + 2 + (row*4)] = (*iterator >> (8*2)) & 511;
                }
                if(firstBE_3){
                    memory[address + 3 + (row*4)] = (*iterator >> (8*3)) & 511;
                }
            } else if(row == length-1){
                int lastBE_0 = (lastBE & 1);
                int lastBE_1 = (lastBE & 1 << 1);
                int lastBE_2 = (lastBE & 1 << 2);
                int lastBE_3 = (lastBE & 1 << 3);

                if(lastBE_0){
                    memory[address + (row*4)] = (*iterator) & 511;
                }
                if(lastBE_1){
                    memory[address + 1 + (row*4)] = (*iterator >> (8*1)) & 511;
                }
                if(lastBE_2){
                    memory[address + 2 + (row*4)] = (*iterator >> (8*2)) & 511;
                }
                if(lastBE_3){
                    memory[address + 3 + (row*4)] = (*iterator >> (8*3)) & 511;
                }
            } else{
                for(int i = 0; i<4; i++){
                    memory[address + i + (row*4)] = (*iterator >> (8*i)) & 511;
                }
            }
        }
        iterator ++;
    }
}

//iterator pointing to data field
int given_iterator_populate_data_return_byteCount(int length, unsigned int * iterator, int firstBE, int lastBE, int address, const char *memory){
    char *byteIterator = (char *)iterator;
    int byteCount = 0;
    for(int row =0 ; row < length; row ++){
        if(row == 0){
            int firstBE_0 = (firstBE & 1);
            int firstBE_1 = (firstBE & 1 << 1);
            int firstBE_2 = (firstBE & 1 << 2);
            int firstBE_3 = (firstBE & 1 << 3);

            if(firstBE_0){
                *byteIterator = memory[address];
                byteIterator ++; 
                byteCount++;
            }
            if(firstBE_1){
                *byteIterator = memory[address+1];
                byteIterator++;
                byteCount++;
            }
            if(firstBE_2){
                *byteIterator = memory[address+2];
                byteIterator ++; 
                byteCount++;
            }
            if(firstBE_3){
                *byteIterator = memory[address+3];
                byteIterator ++; 
                byteCount ++;
            }
        } else if(row == length-1){
            int lastBE_0 = (lastBE & 1);
            int lastBE_1 = (lastBE & 1 << 1);
            int lastBE_2 = (lastBE & 1 << 2);
            int lastBE_3 = (lastBE & 1 << 3);

            if(lastBE_0){
                *byteIterator = memory[address + (row*4)];
                byteIterator ++; 
                byteCount++;
            }
            if(lastBE_1){
                *byteIterator = memory[address + 1 + (row*4)];
                byteIterator++;
                byteCount++;
            }
            if(lastBE_2){
                *byteIterator = memory[address + 2 + (row*4)];
                byteIterator ++; 
                byteCount++;
            }
            if(lastBE_3){
                *byteIterator = memory[address + 3 + (row*4)];
                byteIterator ++; 
                byteCount ++;
            }
        } else{
            for(int i = 0; i<4; i++){
                *byteIterator = memory[address + i + (row*4)];
                byteIterator++;
                byteCount++;
            }
        }
    }
    return byteCount;
}

// returns # of words used to create completion packet
int populate_completion_packet(int length, int firstBE, int lastBE, int* byte_count, int tag, int requesterID, int address, unsigned int* completion, int total_words_used_by_completion, const char *memory){

    //Assemble Completion
    unsigned int* completion_iterator = completion + total_words_used_by_completion;
    *completion_iterator = 37;
    *completion_iterator <<= 25;
    *completion_iterator |= length;
    completion_iterator++;
    *completion_iterator = 0;
    *completion_iterator |= (220 << 16);
    *(completion_iterator) |= *byte_count;
    completion_iterator ++;
    *completion_iterator = 0;
    *completion_iterator |= (requesterID<<16);
    *completion_iterator |= (tag<<8);
    int lower_address = address&127;
    *completion_iterator |= lower_address;
    completion_iterator++;

    int words_used = 3;

    int bytes_used = given_iterator_populate_data_return_byteCount(length, completion_iterator, firstBE, lastBE, address, memory);

    *byte_count = (*byte_count) - bytes_used;
    //calculate words_used
    if(bytes_used%4 == 0){
        words_used += bytes_used / 4;
    } else{
        words_used += (bytes_used/4) + 1;
    }

    return words_used;
}

unsigned int* create_completion(unsigned int packets[], const char *memory)
{
    print_packet(packets);
    int total_words_used_by_completion = 0;
    unsigned int * iterator = packets;
    int size = 0;
    unsigned int* completion = malloc(size);
    while ( (*iterator >> 10) == 0){
        int length = (*iterator) & 1023;
        iterator++;

        unsigned int BE_mask = 15;
        int firstBE = *iterator & BE_mask;
        int lastBE = (*iterator >> 4) & BE_mask;
        unsigned int tag_mask = 255;
        int tag = (*iterator >> 8) & tag_mask;
        unsigned int requesterID_mask = 65535;
        int requesterID = (*iterator >> 16) & requesterID_mask;
        iterator ++;
        int address = (*iterator);

        iterator ++;

        //Assemble Completion
        // populate the data field
                // address to last can = max 4k bytes
                // if multiple of 16kbytes inbetween address and last word, break up into two diff processes
                    // find distance from address (first byte of word) to last word to be accessed
                            // address_to_last % 4 == 0
                                // address_to_last / 4k = distance
                                // address_to_last / 4k +1 = distance
        
        int last_address_in_Bytes = address + (length*4);
        int distance_from_first_byte_to_last_byte = last_address_in_Bytes - address;

        //finishes just at the boundry or same size as when cut
        if(last_address_in_Bytes%16384 == 0 || last_address_in_Bytes%16384 == (address%16384) + distance_from_first_byte_to_last_byte){
            //size is not exact but good enough
            size += sizeof(unsigned int) * (length + 3);
            completion = realloc(completion, size);

            int* byte_count = malloc(sizeof(int)); 
            *byte_count = (length*4);
            if(length == 1){
                (*byte_count) --;
                for (int i = 0; i<4; i ++){
                    if(firstBE & 1 << i) (*byte_count) ++;
                }
            } else {
                (*byte_count) -= 8;
                for (int i = 0; i<4; i ++){
                    if(firstBE & 1 << i) (*byte_count) ++;
                    if(lastBE & 1 << i) (*byte_count) ++;
                }
            }

            total_words_used_by_completion += populate_completion_packet(length, firstBE, lastBE, byte_count, tag, requesterID, address, completion, total_words_used_by_completion, memory);
            free(byte_count);
        } else {
            //size is not exact but good enough
            size += sizeof(unsigned int) * (length + 6);
            completion = realloc(completion, size);

            int second_length_bytes = last_address_in_Bytes % 16384;
            int second_length_words = second_length_bytes/4;
            int second_address = last_address_in_Bytes - second_length_bytes;

            int first_length_bytes = second_address - address;
            int first_length_words = first_length_bytes/4;

            int* byte_count = malloc(sizeof(int)); 
            *byte_count = (first_length_bytes - 4) + (second_length_bytes - 4);
            for (int i = 0; i<4; i ++){
                if(firstBE & 1 << i) (*byte_count) ++;
                if(lastBE & 1 << i) (*byte_count) ++;
            }

            total_words_used_by_completion += populate_completion_packet(first_length_words, firstBE, 15, byte_count, tag, requesterID, address, completion, total_words_used_by_completion, memory);

            //if the length 1, treat lastBE as if it is firstBE
            if(second_length_words == 1){
                total_words_used_by_completion += populate_completion_packet(second_length_words, lastBE, 15, byte_count, tag, requesterID, second_address, completion, total_words_used_by_completion, memory);
            } else{
                total_words_used_by_completion += populate_completion_packet(second_length_words, 15, lastBE, byte_count, tag, requesterID, second_address, completion, total_words_used_by_completion, memory);
            }
            free(byte_count);
        }

    }
	return completion;
}
