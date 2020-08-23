#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUMBER_OF_CHORDS 72
#define NUMBER_OF_STRINGS 4
#define IMPOSSIBLE_CHORD_LENGTH 4
#define MAX_CHORD_LENGTH 6
#define KEY_SIZE 3
#define INDEX_PER_NUMERAL 6
#define BUF_SIZE 64
#define NUMERAL_SIZE 9

char **populate_chords(char **chords);
int find_root_note(int note, char arg);
int find_key(char key[]);
int find_chord(char *argv, char *key);
void find_numeral(int key, int chord, char *numeral_array);
void print(int chord_to_print, char **chords);

int main(int argc, char *argv[]){
    char buf[BUF_SIZE];
    char key[KEY_SIZE];
    int key_index = 0;
    int bytes_read;
    int chord_to_print;
    char numeral[NUMERAL_SIZE];
    char *chords[NUMBER_OF_CHORDS];

    populate_chords(chords); 

    //Reject input with too many arguments
    if (argc > 2){  
        printf("%s\n", "Invalid Input");
        exit(0);
    }
   
    //If user provides no arguments prompt key and start loop 
    if (argc == 1){

       //Set key from user input
       printf("%s\n", "Enter key");
       scanf("%s", key);

       //Make third character null to truncate string
       key[2] = '\0'; 

       //If the key is not valid, reject
       if (key[0] != 'C' &&
           key[0] != 'D' &&
           key[0] != 'E' &&
           key[0] != 'F' &&
           key[0] != 'G' &&
           key[0] != 'A' &&
           key[0] != 'B') {
           printf("Invalid Input\n");
           exit(0);
       }

       //If the second character is invalid, reject
       else if (key[1] != '#' && //sharp
                key[1] != 'b' && //flat
                key[1] != '\0') {//natural
           printf("Invalid Input\n");
           exit(0);           
        }

       printf("%s%s\n", "You are in the key of ", key);

       key_index = find_key(key);

       //Start loop
       while ((bytes_read = read(0, buf, BUF_SIZE)) > 0){
           char arg[bytes_read-1];                //create empty char array 
           strncpy(arg, buf, bytes_read-1);       //Fill array with arg (- \n)
           chord_to_print = find_chord(arg, key); //Find chord index
           if (chord_to_print != -1){
               find_numeral(key_index, chord_to_print, numeral);
               printf("%s\n", numeral);
           }
           print(chord_to_print, chords);         //Print chord

           //Reset buffers
           fflush(stdout);
           memset(buf,'\0', bytes_read);
           memset(arg,'\0', bytes_read);

       } 

    //Otherwise print specified chord
    }else {
        chord_to_print = find_chord(argv[1], key);
        print(chord_to_print, chords);
    }
       
}


//Takes in an argument (eg. Cmaj7) and a key (eg. C)
//Returns an index of that chord in the chord list
int find_chord(char *arg, char *key){
    int note;
    int sharp = 0;
    int flat  = 0;
    int minor = 0;
    int seven = 0;
    int minor_seven = 0;
    int major_seven = 0;
    int six = 0;

    
    //Find root note
    note = find_root_note(note, *arg);

    //Find accidentals
    if (strlen(arg) >= 2){
        switch(*(arg + 1)){
            case '#':
                sharp = 1;
                break;
            case 'b':
                flat = 1;
                break;
        }
    }

    //Reject chords with too many characters
    if (strlen(arg) == (IMPOSSIBLE_CHORD_LENGTH + sharp + flat) || strlen(arg) >= (MAX_CHORD_LENGTH + sharp + flat)){
        //printf("%s\n", "Invalid Input");
        //exit(0);
        return -1;
    }

    //Find chords with single character symbols 
    if (strlen(arg) == (2 + sharp + flat)){
        switch(*(arg + 1 + sharp + flat)){
            case 'm':
                minor = 1;
                break;
            case '7':
                seven = 1;
                break;
            case '6':
                six = 1;
                break;
            default:
                //printf("%s\n", "Invalid Input");
                //exit(0);
                return -1;
        }
    }
    //Find chords with two character symbols
    if (strlen(arg) == (3 + sharp + flat)){
        if (*(arg + sharp + flat + 1) == 'm' && *(arg + sharp + flat + 2) == '7'){
            minor_seven = 1;
        }else{
            //printf("%s\n", "Invalid Input");
            //exit(0);
            return -1;
        }
    }
    //Find chords with four character symbols
    if (strlen(arg) == (5 + sharp + flat)){
        if (*(arg + sharp + flat + 1) == 'm' && 
            *(arg + sharp + flat + 2) == 'a' && 
            *(arg + sharp + flat + 3) == 'j' &&
            *(arg + sharp + flat + 4) == '7'
            ){
            major_seven = 1;
        }else{
            //printf("%s\n", "Invalid Input");
            //exit(0);
            return -1;
        }
    }

    //Adjust according to chord quality
    note = note 
           - (6*flat) 
           + (6*sharp) 
           + (5*six) 
           + (4*minor_seven) 
           + (3*major_seven) 
           + (2*seven) 
           + (1*minor);

    return note;
}

//Takes in an index integer and a list of chords
//Prints out chord at the index into a chord diagram
void print(int chord_to_print, char **chords){
    if (chord_to_print == -1){
        printf("Invalid Input\n\n");
        return;
    }
    for (int i = 0; i < NUMBER_OF_STRINGS; i++){
        switch(*(chords[chord_to_print]+i)){
            case '0':
                printf("%s\n", "--|--|--|--");
                break;
            case '1':
                printf("%s\n", "00|--|--|--");
                break;
            case '2':
                printf("%s\n", "--|00|--|--");
                break;
            case '3':
                printf("%s\n", "--|--|00|--");
                break;
            case '4':
                printf("%s\n", "--|--|--|00");
                break;
        }
    }
    printf("\n");
}

//Take in an integer and an argument (eg. Cmaj7)
//Return the root note of that argument (eg. C)
int find_root_note(int note, char arg){
    switch(arg){
        case 'C':
            note = 0;
            break;
        case 'D':
            note = 12;
            break;
        case 'E':
            note = 24;
            break;
        case 'F':
            note = 30;
            break;
        case 'G':
            note = 42;
            break;
        case 'A':
            note = 54;
            break;
        case 'B':
            note = 66;
            break;
        default:
            //printf("%s\n", "Invalid Input");
            //exit(0);
            return -1;
    }
    return note;

}

//Takes in a key string
//Returns the index of that key
int find_key(char key[]){

    int key_index = 0;

    //Find root note
    switch(*key){
        case 'C':
            key_index = 0;
            break;
        case 'D':
            key_index = 12;
            break;
        case 'E':
            key_index = 24;
            break;
        case 'F':
            key_index = 30;
            break;
        case 'G':
            key_index = 42;
            break;
        case 'A':
            key_index = 54;
            break;
        case 'B':
            key_index = 66;
            break;
        default:
            printf("%s\n", "Invalid Input");
            exit(0);
    }

    //raise or lower depending on accidental
    if (strlen(key) == 2){
        if (key[1] == '#'){
            key_index += 6;
        }

        if (key[1] == 'b'){
            key_index -= 6;
        }
    }
    return key_index;

}

//Takes in indexes for the key and the chord
//Return roman numeral representation of chord 
void find_numeral(int key, int chord, char *numeral){
    int diff = 0;

    //Find distance between chord and key
    if (key <= chord){
        diff = chord-key;
    }else{
        diff = NUMBER_OF_CHORDS - (key-chord);        
    }
    
    //Find numeral
    switch(diff/INDEX_PER_NUMERAL){

        case 0:
            strcpy(numeral, "I"); 
            break;
        case 1:
            strcpy(numeral, "I#/IIb"); 
            break;
        case 2:
            strcpy(numeral, "II"); 
            break;
        case 3:
            strcpy(numeral, "II#/IIIb"); 
            break;
        case 4:
            strcpy(numeral, "III"); 
            break;
        case 5:
            strcpy(numeral, "IV"); 
            break;
        case 6:
            strcpy(numeral, "IV#/Vb"); 
            break;
        case 7:
            strcpy(numeral, "V"); 
            break;
        case 8:
            strcpy(numeral, "V#/VIb"); 
            break;
        case 9:
            strcpy(numeral, "VI"); 
            break;
        case 10:
            strcpy(numeral, "VI#/VIIb"); 
            break;
        case 11:
            strcpy(numeral, "VII"); 
            break;
    }
    
    //Concatenate chord quality
    switch(chord%6){
        case 0:
            break;
        case 1:
            strcat(numeral, "m");
            break;
        case 2:
            strcat(numeral, "7");
            break;
        case 3:
            strcat(numeral, "maj7");
            break;
        case 4:
            strcat(numeral, "m7");
            break;
        case 5:
            strcat(numeral, "6");
            break;
    }

}

char **populate_chords(char **chords){

    //Populate chord array

    //C 0
    chords[0] = "3000"; //C
    chords[1] = "3330"; //Cm
    chords[2] = "1000"; //C7
    chords[3] = "2000"; //Cmaj7
    chords[4] = "3333"; //Cm7
    chords[5] = "0000"; //C6

    //C#/Db 1
    chords[6] = "4111";
    chords[7] = "3011";
    chords[8] = "2111";
    chords[9] = "3111";
    chords[10] = "4444";
    chords[11] = "1111";
    
    //D 3
    chords[12] = "0222";
    chords[13] = "0122";
    chords[14] = "3222";
    chords[15] = "4222";
    chords[16] = "3122";
    chords[17] = "2222";


    //D#/Eb 4
    chords[18] = "1333";
    chords[19] = "1233";
    chords[20] = "4333";
    chords[21] = "0333";
    chords[22] = "4233";
    chords[23] = "3333";
    
    //E/Fb 6
    chords[24] = "2041";
    chords[25] = "2340";
    chords[26] = "2021";
    chords[27] = "2031";
    chords[28] = "2020";
    chords[29] = "0201";

    //F/E# 7
    chords[30] = "0102";
    chords[31] = "3101";
    chords[32] = "0132";
    chords[33] = "3142";
    chords[34] = "3131";
    chords[35] = "3122";

    //F#/Gb 8
    chords[36] = "1213";
    chords[37] = "0212";
    chords[38] = "4243";
    chords[39] = "1110";
    chords[40] = "4242";
    chords[41] = "4233";

    //G 10
    chords[42] = "2320";
    chords[43] = "1320";
    chords[44] = "2120";
    chords[45] = "2220";
    chords[46] = "1120";
    chords[47] = "2020";

    //G#/Ab 11
    chords[48] = "3431";
    chords[49] = "2431";
    chords[50] = "3231";
    chords[51] = "3331";
    chords[52] = "2230";
    chords[53] = "3131";

    //A 13
    chords[54] = "0012";
    chords[55] = "0002";
    chords[56] = "0010";
    chords[57] = "0011";
    chords[58] = "0000";
    chords[59] = "4242";

    //A#/Bb 14
    chords[60] = "1123";
    chords[61] = "1113";
    chords[62] = "1121";
    chords[63] = "0123";
    chords[64] = "1111";
    chords[65] = "1120";

    //B 16
    chords[66] = "2234";
    chords[67] = "2224";
    chords[68] = "2232";
    chords[69] = "2233";
    chords[70] = "2222";
    chords[71] = "2231";

    return chords;

}
