#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#define DEBUG 1
#define BUFSIZE 9999
#define MAX_DIGITS 999
#define MAX_NUM_CHAR_DIRECTORIES_SIZE_NAME 200

struct person *headptr = NULL;
int highest_num_of_words = 1;

/* declare your struct for a person here */
struct person {
    char *name;
    int age;
    struct person *next;
};

static int compare_people(const struct person *a, const struct person *b) {
     return strcmp(a->name, b->name);
}

static struct person *insert_sorted(struct person *headptr, char *name, int age) {
    // Allocate heap space for a record
    struct person *ptr = malloc(sizeof(struct person));
    if (ptr == NULL) {
        abort();
    }
    ptr->name = malloc(2*strlen(name));
    strcpy(ptr->name, name);
    ptr->age = age;
    ptr->next = NULL;

    if (headptr == NULL || compare_people(ptr, headptr) < 0) {
        ptr->next = headptr;
        return ptr;
    } else {
        struct person *cur = headptr;
        while (cur->next != NULL && compare_people(ptr, cur->next) >= 0) {
            cur = cur->next;
        }
        ptr->next = cur->next;
        cur->next = ptr;
        return headptr;
    }
}

//--------------------------------------------------------------------------------------//
struct person *accept_word(char *word, int word_length, struct person *headptr)
{
    while (word[0] == '\n')
    {
        for(int i = 0; i < word_length; i++)
        {
            word[i] = word[i+1];
        }
        word_length--;
        }
        while (word[0] == '-')
        {
            for(int i = 0; i < word_length; i++)
            {
                word[i] = word[i+1];
            }
            word_length--;
        }
    if (word[0] != NULL && word[0] != '\n' && word_length >= 1) 
    {
        // printf("Got a word: |%s|\n", word);
        struct person *curr = headptr;
        while (curr != NULL) // finds if the word is already in the list, if so add to the counter by +1
        {
            if (strcmp(word, curr->name) == 0)
            {
                curr->age = curr->age + 1;
                if (curr->age > highest_num_of_words) highest_num_of_words = curr->age;
                free(word);
                return headptr;
            }
            curr = curr->next;
        }
        headptr = insert_sorted(headptr, word, 1);
        free(word);
        return headptr;
    }
    else free(word);
}

struct person *extract_insert(int fd)
{
    char buf[BUFSIZE-1];
    int pos, bytes, start, posnext;
    int word_length = 0, wordstart;
    char *word = NULL;
    if ((bytes = read(fd, buf, BUFSIZE)) > 0) 
    {
        // if (DEBUG) printf("Read %d bytes\n", bytes);
        // search input for a newline character
        start = 0;
        pos = 0;
        wordstart = 0;
        while (pos < bytes) 
        {
            if (buf[pos] == ' ' || buf[pos] == '!' || buf[pos] == '?' || buf[pos] == ',' || buf[pos] == '"' || buf[pos] == ':' || buf[pos] == ';' || buf[pos] == '0' || buf[pos] == '1' || buf[pos] == '2' || buf[pos] == '3' || buf[pos] == '4' || buf[pos] == '5' || buf[pos] == '6' || buf[pos] == '7' || buf[pos] == '8' || buf[pos] == '9' || buf[pos] == '.' || buf[pos] == '=')
            {
                word_length = pos - wordstart;
                if ( word_length < 1)
                {
                    wordstart++;
                    pos++;
                    continue;
                }
                word = malloc(word_length + 1);
                memcpy(word, buf + wordstart, word_length);
                word[word_length] = '\0';
                headptr = accept_word(word, word_length, headptr);
                wordstart = pos + 1;
                pos++;
                continue;
                while (buf[pos] == ('&'+1)) //I didn't know how to reference the apostrophe 
                {
                    //printf("we made it!\n");
                    if (!(('a' <= buf[pos + 1] && buf[pos + 1] <= 'z') || ('A' <= buf[pos + 1] && buf[pos + 1] <= 'Z')))
                    {
                        wordstart++;
                        pos++;
                    }
                }
                while (buf[pos] == ' ' || buf[pos] == '!' || buf[pos] == '?' || buf[pos] == ',' || buf[pos] == ':' || buf[pos] == ';' || buf[pos] == '0' || buf[pos] == '1' || buf[pos] == '2' || buf[pos] == '3' || buf[pos] == '4' || buf[pos] == '5' || buf[pos] == '6' || buf[pos] == '7' || buf[pos] == '8' || buf[pos] == '9' || buf[pos] == '-' || buf[pos] == '"' || buf[pos] == '.' || buf[pos] == '=')
                {
                    pos++;
                    wordstart++;
                    while (buf[pos] == ('&'+1)) //I didn't know how to reference the apostrophe 
                    {
                        if (!(('a' <= buf[pos + 1] && buf[pos + 1] <= 'z') || ('A' <= buf[pos + 1] && buf[pos + 1] <= 'Z')))
                        {
                            wordstart++;
                            pos++;
                        }
                    }
                }
            }
            if (buf[pos] == '&'+1)
            {
                if (!(('a' <= buf[pos + 1] && buf[pos + 1] <= 'z') || ('A' <= buf[pos + 1] && buf[pos + 1] <= 'Z')))
                {
                    if (pos > 0)
                    {                   
                        if ((('a' <= buf[pos - 1] && buf[pos - 1] <= 'z') || ('A' <= buf[pos - 1] && buf[pos - 1] <= 'Z')))
                        {
                            pos++;
                        }
                    }
                    word_length = pos - wordstart;
                    if ( word_length >= 1)
                    {
                        word = malloc(word_length + 1);
                        memcpy(word, buf + wordstart, word_length);
                        word[word_length] = '\0';
                        headptr = accept_word(word, word_length, headptr);
                        wordstart = pos + 1;
                    }
                    else 
                    {
                        wordstart = pos + 1;
                    }
                }
            }
            if (buf[pos] == '-')
            {
                //printf("this is the char before: %c\n", buf[pos-1]);
                if (buf[pos + 1]== '-')
                {
                    word_length = pos - wordstart;
                    // if ( word_length <= 1)
                    // {
                    //     wordstart++;
                    //     pos++;
                    //     continue;
                    // }
                    word = malloc(word_length + 1);
                    memcpy(word, buf + wordstart, word_length);
                    word[word_length] = '\0';
                    headptr = accept_word(word, word_length, headptr);
                    wordstart = pos + 1;
                    pos = pos + 1;
                    while (buf[pos] == ' ' || buf[pos] == '!' || buf[pos] == '?' || buf[pos] == ',' || buf[pos] == ':' || buf[pos] == ';' || buf[pos] == '0' || buf[pos] == '1' || buf[pos] == '2' || buf[pos] == '3' || buf[pos] == '4' || buf[pos] == '5' || buf[pos] == '6' || buf[pos] == '7' || buf[pos] == '8' || buf[pos] == '9' || buf[pos] == '-' || buf[pos] == ' ' )
                    {
                        pos++;
                        wordstart++;
                    }
                }

                if (!(('a' <= buf[pos + 1] && buf[pos + 1] <= 'z') || ('A' <= buf[pos + 1] && buf[pos + 1] <= 'Z')))
                {
                    word_length = pos - wordstart;
                    if ( word_length >= 1)
                    {

                        word = malloc(word_length + 1);
                        memcpy(word, buf + wordstart, word_length);
                        word[word_length] = '\0';
                        headptr = accept_word(word, word_length, headptr);
                        wordstart = pos + 1;
                    } 
                }
            }
            if (buf[pos] == '\n') 
            {   

                word_length = pos - wordstart;

                if ( word_length >= 1)
                {
                    word = malloc(word_length + 1);
                    memcpy(word, buf + wordstart, word_length);
                    word[word_length] = '\0';
                    headptr = accept_word(word, word_length, headptr);
                    wordstart = pos + 1;
                }

                while (buf[pos+1] == '\n') 
                {
                    pos++;
                    wordstart = pos;
                }
            }
            pos++;
        }
    }
    close(fd);
    return headptr;
}
// ========================================================================================================================================================================
int main(int argc, char **argv)
{
    char strFILE[200];
    int fd;
    if (argc > 1) 
    {   
        int why = strlen(argv[1]);
        char strDirName[why];
        for (int x = 0; x < why-1; x++)
        {
            strDirName[x] = argv[1][x];
        }
        DIR *dir = opendir(argv[1]);
        if(dir == NULL)
        {
            //printf("NOT A DIRECTORY\n");
        }
        else
        {
            struct dirent *entry;
            
            char sizeOfString[MAX_NUM_CHAR_DIRECTORIES_SIZE_NAME];
            sizeOfString[0] = '.';
            sizeOfString[1] = '/';
            int j = 2;
            int i = 0;
            while ( i < strlen(argv[1]) ) 
            {
                sizeOfString[j] = argv[1][i];
                i++;
                j++;
            }
            int startj = j;
            // printf("Start string: %s\n", sizeOfString);
            while ((entry = readdir(dir)) != NULL)
            {
                //printf("This is the name of the file: %s\n", entry->d_name);
                if (entry->d_name[strlen(entry->d_name)-1] == 't' && entry->d_name[strlen(entry->d_name)-2] == 'x' && entry->d_name[strlen(entry->d_name)-3] == 't')
                {

                    i = 0;
                    while (entry->d_name[i] != '\0' ) 
                    {
                        sizeOfString[j] = entry->d_name[i];
                        i++;
                        j++;
                        //printf("this is too much\n");
                    }
                    j = startj;
                    fd = open(sizeOfString, O_RDONLY);
                    headptr = extract_insert(fd);
                }
            }
            closedir(dir);

            int fdwrite = open ("answer.txt", O_RDWR | O_CREAT, 0644);
            struct person *current = headptr;
            struct person *temp = headptr;
            //if (headptr == NULL) printf("Head is NULL Still\n");
            for (int i = highest_num_of_words; i > 0; i--)
            {
                current = headptr;
                while (current != NULL)
                {
                    if (current->age == i)
                    {
                        // printf("The word: %s | Counter: %d\n", current->name, current->age);
                        write(fdwrite, current->name, strlen(current->name));
                        write(fdwrite, " ", 1);
                        char numCounterString[MAX_DIGITS];
                        sprintf(numCounterString, "%d", current->age);
                        // printf("currently writing this: %d", current->age);
                        write(fdwrite, numCounterString, strlen(numCounterString));
                        write(fdwrite, "\n", 1);
                    }
                    current = current->next;
                }
            }
            current = headptr;
            while (current != NULL) 
            {
                // printf("I am freeing: %s | counter: %d\n", current->name, current->age);
                current = current->next;
                free(temp->name);
                free(temp);
                temp = current;
            }
            close(fdwrite);
            return EXIT_SUCCESS;
        }

        fd = open(argv[1], O_RDONLY);
        if (fd < 0) 
        {
            perror(argv[1]);
            exit(EXIT_FAILURE);
        }
    } 
    else 
    {
        fd = STDIN_FILENO;
    }
    headptr = extract_insert(fd);
    int fdwrite = open ("answer.txt", O_RDWR | O_CREAT, 0644);
    struct person *current = headptr;
    struct person *temp = headptr;
    // if (headptr == NULL) printf("Head is NULL Still\n");
    for (int i = highest_num_of_words; i > 0; i--)
    {
        current = headptr;
        while (current != NULL)
        {
            if (current->age == i)
            {
                //printf("The word: %s | Counter: %d\n", current->name, current->age);
                write(fdwrite, current->name, strlen(current->name));
                write(fdwrite, " ", 1);
                char numCounterString[MAX_DIGITS];
                sprintf(numCounterString, "%d", current->age);
                // printf("currently writing this: %d", current->age);
                write(fdwrite, numCounterString, strlen(numCounterString));
                write(fdwrite, "\n", 1);
            }
            current = current->next;
        }
    }
    current = headptr;
    while (current != NULL) 
    {
        //printf("I am freeing: %s | counter: %d\n", current->name, current->age);
        current = current->next;
        free(temp->name);
        free(temp);
        temp = current;
    }
    //printf("This is the highest counter: %d\n", highest_num_of_words);
    close(fdwrite);
    return EXIT_SUCCESS;
}

