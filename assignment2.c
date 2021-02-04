/***********************************************************************
name:
	ok -- see if a word is in the online dictionary
description:	
	See CS 360 IO lecture for details.
***********************************************************************/

/* Includes and definitions */

/**********************************************************************
Search for given word in dictionary using file descriptor fd.
Return the line number the word was found on, negative of the last line searched
if not found, or the error number if an error occurs.
**********************************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int ok(char *dictionaryName, char *word, int length) {

    int fd = open(dictionaryName, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Error %d: cannot open file <%s>: %s\n", errno, dictionaryName, strerror(errno));
        return (errno);
    }

    int lastLine = lseek(fd, 0, SEEK_END); // get the total bytes of the file
    if (lastLine < 0) {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        close(fd);
        return (errno);
    }

    int totalLine = lastLine / length; // compute number of lines in the file
    if (totalLine == 0) {
        fprintf(stderr, "Error: %s is empty\n", dictionaryName);
        close(fd);
        return (-1);
    }

    lseek(fd, 0, SEEK_SET); // set the offset back to the beginning of the file

    char buffer[length];
    char copyWord[length];

    strcpy(copyWord, word); // copy the given word and truncate to the specified length
    copyWord[(length - 1)] = '\0';

    // binary search iterative algorithm
    int topLine = 1;
    int bottomLine = totalLine;
    int middleLine, currentOffset, compareResult, readResult;

    while (topLine <= bottomLine) {
        middleLine = (topLine + bottomLine) / 2;
        currentOffset = (middleLine * length) - length;

        lseek(fd, currentOffset, SEEK_SET);

        readResult = read(fd, buffer, length - 1);
        buffer[length - 1] = '\0';

        if (readResult < 0) {
            fprintf(stderr, "Error %d reading file: %s\n", errno, dictionaryName);
            return (errno);
        }
        if (readResult == 0) { // in case, there is only one line in the given file
            if (strcmp(buffer, copyWord) == 0)
                return middleLine;
            return middleLine * -1;
        }

        compareResult = strcmp(copyWord, buffer); // compare the given word with the buffer
        if (compareResult == 0 || compareResult == -32) // found the word
            return middleLine;
        else if (compareResult > 0)
            topLine = middleLine + 1;
        else
            bottomLine = middleLine - 1;
    }
    close(fd);
    return middleLine * (-1);
}