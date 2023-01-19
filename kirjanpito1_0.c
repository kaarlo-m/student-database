#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//The table's last member is a buffer. Its first name is set to point to NULL.

struct student {
    char *name_first;
    char *name_last;
    char student_number[7];
    unsigned int points[6];
};

int field_length(char *string, char separator, int field_number){
    //Returns the length of a field inside string, when fields are separated by a single-character separator.
    char *field_end;
    char *field_start=string;
    int length;
    int i;
    field_end=strchr(string, separator);
    for (i=1; i<field_number;i++){
        field_end=strchr(field_end+1,separator);
        field_start=strchr(field_start+1, separator);
    }
    length=field_end-field_start-1;
    return length;
}

int field_count(const char *string){
    //Returns how many fields of text are in a string, used for checking user input.
    int i=0;
    int istext=0;
    int fields=0;
    while (string[i]!='\0'){
            //Reading a character will set "istext" to 1, and spaces will set it to 0.
            if ((isspace(string[i])==0) && (istext==0)){
                //Moving from a space or the start of the string to a character counts the following characters as 1 field.
                istext=1;
                fields++;
            }
            else if ((isspace(string[i])!=0) && (istext==1)){
                istext=0;
            }
            i++;
    }
    return fields;
}


struct student *init_table(struct student *table){
    table = malloc(sizeof(struct student));
    table->name_first=NULL;
    return table;
}

struct student *clear_table(struct student *table){
    //Frees all dynamically allocated memory used by table.
    int i;
    for (i=0; table[i].name_first != NULL; i++){
        free(table[i].name_first);
        free(table[i].name_last);
    }
    free(table);
    return table;
}

struct student init_student(char *first, char *last, char *number){
    //Allocates memory for the students' names, assigns their names and sets their points from all rounds to 0.
    struct student new_student;
    //malloc(strlen(first)*sizeof(char)); would be optimal. A constant heap size, however, is easier to manage when switching elements around.
    new_student.name_first=malloc(50*sizeof(char));
    strcpy(new_student.name_first, first);
    new_student.name_last=malloc(50*sizeof(char));
    strcpy(new_student.name_last, last);
    strcpy(new_student.student_number, number);
    memset(new_student.points, 0, sizeof(unsigned int)*6);
    return new_student;
}

struct student *swap_elements(struct student *table, int a, int b){
    //Swaps two elements from the given table of students. Used in sorting.
    int k;
    struct student temp=init_student(table[b].name_first, table[b].name_last, table[b].student_number);
    for (k=0;k<=5;k++){
            temp.points[k]=table[b].points[k];
        }
    free(table[b].name_first);
    free(table[b].name_last);
    table[b]=table[a];

    table[a]=temp;

    return table;
}


struct student *add_student(struct student *table, char *first, char *last, char *number){
    int i=0;
    for (i=0; table[i].name_first!=NULL; i++);

    table=realloc(table, (i+2)*sizeof(struct student));
    if (table==NULL){
        printf("\tError: Memory allocation failed.\n");
        return table;
    }
    table[i]=init_student(first, last, number);
    table[i+1].name_first=NULL;

    return table;

    }

struct student *setpoints(char *number, struct student *table, int round, int grade){
    int i;
    for (i=0; table[i].name_first!=NULL;i++){
        //Searching for number in table. If it's found, the loop is broken.
        if (strcmp(table[i].student_number, number)==0)
            break;
    }

    if (table[i].name_first==NULL){
        //If the end of the table is reached, the student number was not found.
        printf("\tError: Student number %s not found.\n", number);
        return table;
    }
    table[i].points[round-1]=grade;
    printf("\tSuccess: Points were updated.\n");
    return table;
}

void print_table(struct student *table){
    int i;
    int k;
    int l;
    int max=0;
    int best=-1;
    int total=0;
    //A second table is used inside this function, so that the order of the table remains unchanged.
    struct student *sorted=NULL;
    sorted=init_table(sorted);
    for (l=0; table[l].name_first!=NULL; l++){
        sorted=add_student(sorted, table[l].name_first, table[l].name_last, table[l].student_number);
        for (k=1;k<=6;k++){
            sorted[l].points[k-1]=table[l].points[k-1];
        }
    }

    //Selection sort for descending point total. 
    for (l=0; sorted[l].name_first!=NULL; l++){
        for (i=l; sorted[i].name_first!=NULL; i++){
            for (k=0;k<6;k++){
                total=total+sorted[i].points[k];
            }
            if (total>=max){
                max=total;
                best=i;
            }
            total=0;
        }
        sorted=swap_elements(sorted, l, best);
        max=-1;
    }
    
    l=0;
    while (sorted[l].name_first!=NULL){
        printf("%s %s %s", sorted[l].student_number, sorted[l].name_last, sorted[l].name_first);
        total=0;
        for (k=0; k<6; k++){
            printf(" %d", sorted[l].points[k]);
            total=total+sorted[l].points[k];
        }
        printf(" %d\n", total);
        l++;
    }
    clear_table(sorted);
}

int save_table(const char *filename, struct student *table){
    int l=0;
    int k;
    FILE *f=fopen(filename, "w");
    //-1 is returned if opening the file fails.
    if (f==NULL){
        return -1;
    }
    while (table[l].name_first!=NULL){
        fprintf(f, "%s %s %s ", table[l].student_number, table[l].name_last, table[l].name_first);
        for (k=0; k<6; k++){
            fprintf(f, "%d ", table[l].points[k]);
        }
        fprintf(f, "\n");
        l++;
    }
    fclose(f);
    return 1;

}

struct student *get_table(const char *filename, struct student *table){
    int k=0;
    int j=0;
    int points[6];
    char number[7];
    FILE *f=fopen(filename, "r");
    if (f==NULL){
        printf("\tError: Could not read file \"%s\"\n", filename);

        return table;
    }
    char *name_first=malloc(50*sizeof(char)); 
    char *name_last=malloc(50*sizeof(char));
    table=clear_table(table);
    table=init_table(table);

    char *line = malloc(100*sizeof(char));
    while ((fgets(line,100, f))){
        
        sscanf(line, "%s %s %s %d %d %d %d %d %d", number, name_last, name_first, &points[0], &points[1], &points[2], &points[3], &points[4], &points[5]);
        
        table=add_student(table, name_first, name_last, number);
        
        for (k=0; k<6; k++){
            table[j].points[k]=points[k];
        }
        j++;

    }
    free(name_first);
    free(name_last);
    free(line);
    fclose(f);
    printf("\tSuccess: File \"%s\" was read.\n", filename);
    return table;
}

int main(){
    struct student *table=NULL;
    table=init_table(table);
    char command;
    int round;
    int points;
    int on=1;
    char number[7];
    char *name_first;
    char *name_last;
    char *filename;
    char *input=malloc(100*sizeof(char));
    /*The loop of inputs is ended when:
    1. The user types "Q" and the loop is to be broken (on==0).
    2. fgets() reads EOF without reading characters, thus returning NULL (this has not been tested).*/
    while((on==1)&&(fgets(input,100,stdin)!=NULL)){

        if (isspace(input[1])==0){
            printf("\tError: Invalid command.\n");
            continue;
        }
        
        command=input[0];
        switch(command){
            case 'A':
                if (field_count(input)<4){
                    printf("\tError: Too few arguments for command A. Nothing was appended to the table.\n");
                    break;
                }
                else if (field_count(input)>4){
                    printf("\tError: Too many arguments for command A. Nothing was appended to the table.\n");
                    break;
                }
                if (field_length(input, ' ', 2)>6){
                    printf("\tError: Student numbers must be less than 7 characters long. Nothing was appended to the table.\n");
                    break;
                }
                name_first=malloc(50*sizeof(char));
                name_last=malloc(50*sizeof(char));
                sscanf(input, "%c %s %s %s", &command, number, name_last, name_first);
                table=add_student(table, name_first, name_last, number);
                free(name_first);
                free(name_last);
                printf("\tSuccess: Information was added to the table.\n");
                
            break;

            case 'U':
                if (field_count(input)<4){
                    printf("\tError: Too few arguments for command A. No points were changed from the table.\n");
                    break;
                }
                else if (field_count(input)>4){
                    printf("\tError: Too many arguments for command A. No points were changed from the table.\n");
                    break;
                }
                if (field_length(input, ' ', 2)>6){
                    printf("\tError: Student numbers must be less than 7 characters long. No points were changed from the table.\n");
                    break;
                }
                
                sscanf(input, "%c %s %d %d", &command, number, &round, &points);
                table=setpoints(number, table, round, points);

            break;

            case 'L':
                if (field_count(input)>1){
                    printf("\tError: Too many arguments for command L. The table was not printed.\n");
                    break;
                }
                print_table(table);
                printf("\tSuccess: The table was printed.\n");

            break;

            case 'W':
                if (field_count(input)<2){
                    printf("\tError: Too few arguments for command W. The table was not saved.\n");
                    break;
                }
                else if (field_count(input)>2){
                    printf("\tError: Too many arguments for command W. The table was not saved.\n");
                    break;
                }
                filename=malloc(50*sizeof(char));
                sscanf(input, "%c %s", &command, filename);
                if (save_table(filename, table)==1){
                    printf("\tSuccess: The table was saved to file \"%s\"\n", filename);
                }
                else{
                    printf("\tError: The table was not saved to file\"%s\"\n", filename);
                }
                free(filename);

            break;

            case 'O':
                if (field_count(input)<2){
                        printf("\tError: Too few arguments for command O. The table was not saved.\n");
                        break;
                    }
                    else if (field_count(input)>2){
                        printf("\tError: Too many arguments for command O. The table was not saved.\n");
                        break;
                    }
                    filename=malloc(50*sizeof(char));
                    sscanf(input, "%c %s", &command, filename);
                    table=get_table(filename, table);
                    free(filename);
                    

            break;

            case 'Q':
            if (field_count(input)>1){
                    printf("\tError: Too many arguments for command Q. The program was not terminated.\n");
                    break;
                }

            on=0;
            break;

            default:
            printf("\tError: Invalid command.\n");
    }
    }
    clear_table(table);
    free(input);
    printf("\tThe program was terminated.\n");
}