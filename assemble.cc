#include <stdio.h>
#include <string.h>

#define LIMIT 10000
#define LINE_SIZE 128

//For part 1
void assemble(char[]);
int findOrigin(FILE*);
int firstPass(FILE*, int[], int);
void printLabels(int[]);
void remove_spaces(char[]);
void toUpperCase(char[]);
int hexToInt(char*);
//For part 2
int getAdd(char[]);
int getAnd(char[]);
int getTrap(char[]);
int getNot(char[]);
int getLd(char[], int[], int);
int getLdr(char[]);
int getSt(char[], int[], int);
int getStr(char[]);
int getBr(char[], int[], int);
int getIt(int, char[]);
int secondPass(FILE*, int[], int);

void assemble(char filename[])
{
    //Open the file for reading
    FILE *infile = fopen(filename , "r" );

    if (infile != NULL)
    {
        //Create labels array and set all elements to -1.  
        int labels[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

        int lc = findOrigin(infile);

        if (lc > -1)
        {
            //Read in label values
            if (!firstPass(infile, labels, lc))
            {
                //Show the labels.
                printLabels(labels);

                //The following is for part 2
                //secondPass(infile, labels, lc);
            }
        }

        //Close the file
        fclose(infile);

    }
    else
    {
        printf("Can't open input file.\n");
    }

}


int findOrigin(FILE *infile)
{
    //Each trip through the while loop will read the next line of infile
    //into the line char array as a null terminated string.
    char line[LINE_SIZE];
    line[0] = 0;
    //The variable lineCount keeps track of how many lines have been read.
    //It is used to guard against infinite loops.  Don't remove anything
    //that has to do with linecount.
    int lineCount = 0; //

    //For getting out of the while loop.
    int done = 0;

    //For getting rid of the trailing newline
    char c;

    if(infile == NULL)
    {
        printf("Can't open input file. \n");
        return -1;
    }
    //Read lines until EOF reached or LIMIT lines read.  Limit prevent infinite loop.
    while (!done && lineCount < LIMIT && fscanf(infile, "%[^\n]s", line) != EOF)
    {
        lineCount++;

        fscanf(infile, "%c", &c);  //Get rid of extra newline.

        //removing spaces from line
        remove_spaces(line);
        toUpperCase(line);

        //At this point, line contains the next line of the ASM file.
        //Put your code here for seeing if line is an origin.
        //Options:
        //  1. line is an origin (save value, set done = 1).
        //  2. line is a blank line(skip)
        //  3. line is a comment(skip)
        //  4. line is anything else (print error, set done = 1)
        if(line[0] != 0 && line[0] != ';')
        {
            if(strncmp(line, ".ORIG", 5) != 0)
            {
                printf("ERROR 1: Missing origin directive. Origin must be first line in program.\n");
                return -1;
            }
            else
            {
                int value;
                if(line[5] == 'X')
                {
                    sscanf(&line[6], "%x", &value);
                }
                else
                {
                    sscanf(&line[5], "%d", &value);
                }
                if(value > 0xFFFF || value < 0)
                {
                    printf("ERROR 2: Bad origin address. Address too big for 16 bit.\n");
                    return 2;
                }
                return value;
            }
        }
        //Set the line to empty string for the next pass.
        line[0] = 0;
    }


    //At this point you must decide if an origin was found or not.
    //How you do this is up to you.
    //If a good origin was found, check the size.  
    //      if it is too big, print an error and return -1.
    //      if it is not too big, return the value.
    //If a good origin was NOT found, print the error message and return -1.
    printf("ERROR 1: Missing origin directive. Origin directive must be first line in program.\n");
    return -1;

}

int firstPass(FILE *infile, int labels[], int lc)
{
    //Create a while loop similar to findOrigin.
    //You can rewind if you want to but you shouldn't have to.

    //Read a line.
        //If the line is a comment, a blank line or the .orig directive, don<92>t do anything.
        //If the line is a label on a line by itself, save the lc to the labels array at the appropriate posit
        //  For example, if L3, store the lc value in labels[3].
        //If the line is a label followed by .fill, save the lc in labels AND increment lc.
        //If the line is .end, return 0 for done with no error.
        //If the end of file is reached before .end is found print the error and return -1.
        //If the line is one of the allowed instructions (ADD, AND, NOT, LD, LDR, ST, STR, BR, and TRAP) incre
        //If the line is anything else print the unknown instruction error and return -1.
    rewind(infile);
    int lc2 = 0;
    int end = 0;
    char line[LINE_SIZE];
    char k;
    while(!end && fscanf(infile, "%[^\n]s", line) != EOF && lc2 < LIMIT)
    {
        remove_spaces(line);
        toUpperCase(line);
        fscanf(infile, "%c", &k);
        lc2++;
        if(line[0] != 0 && line[0] != ';' && strncmp(".ORIG", line, 5) != 0)
        {
            if(line[0] == 'L' && line[1] != 'D')
            {
                int v = line[1] - 48;
                labels[v] = lc;
                if(strlen(line) > 2)
                {
                    lc++;
                }
            }
            else if(strncmp(".END", line, 4) == 0)
            {
                end = 1;
                lc++;
            }
            else
            {
                lc++;
            }
        }
            line[0] = 0;

    }
    if(!end)
    {
        printf("ERROR 4: Missing end directive. \n");
        return -1;
    }
    return 0;
}





void printLabels(int labels[])
{
    printf("labels = {%d", labels[0]);
    for(int i = 1; i < 10; i++)
    {
        printf(", %d", labels[i]);
    }
    printf("}\n");
}

void remove_spaces(char line[])
{
    for(int i = 0;line[i] != 0; i++)
    {
    	if(line[i] ==  ' ' || line[i] ==  '\t')
        {
            for(int j = i; line[j] != 0; j++)
            {
                line[j] = line[j + 1];
            }
            i--;
        }
    }
}

void toUpperCase(char line[])
{
    for(int i = 0; line[i] != 0; i++)
    {
        if(line[i] >= 'a' && line[i] <= 'z')
        {
            line[i] -=32;
        }
    }
}

int hexToInt(char* hex)
{
    int res = 0;

    while((*hex) != '\0')
    {
        if(('0' <= (*hex)) && ((*hex) <= '9'))
            res = res*16 + (*hex) - '0';

        else if(('a' <= (*hex)) && ((*hex) <= 'f'))
            res = res*16 + (*hex) - 'a'+10;

        else if(('A' <= (*hex)) && ((*hex) <= 'F'))
            res = res*16 + (*hex) - 'A'+10;

        hex++;
    }
    return (res);
}
              

    