#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    //Program should accept only one command line argument
    if (argc != 2)
    {
        printf("Usage: Only one forensic image allowed.");
        return 1;
    }
    //Open memory card
    FILE *memory_card = fopen(argv[1], "r");
    if (memory_card == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }
    //Read memory card 512 bytes at a time until the end
    BYTE buffer[512];
    char *filename = malloc(8 * sizeof(char));
    //Initialize image_number with -1 because image numbering should start fromm 000 on up
    int image_number = -1;
    FILE *image = NULL;
    while ((fread(buffer, sizeof(BYTE), 512, memory_card)) != 0)
    {
        //Look for a JPEG
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            //With every JPEG found increase the number of images
            image_number++;
            //If JPEG found is the first one
            if (image_number == 0)
            {
                sprintf(filename, "%03i.jpg", image_number);
                image = fopen(filename, "w");
                fwrite(buffer, sizeof(BYTE), 512, image);
            }
            //if JPEG found is not the first one
            if (image_number > 0)
            {
                fclose(image);
                sprintf(filename, "%03i.jpg", image_number);
                image = fopen(filename, "w");
                fwrite(buffer, sizeof(BYTE), 512, image);
            }
        }
        //If already found JPEG
        else if (image != NULL)
        {
            fwrite(buffer, sizeof(BYTE), 512, image);
        }
    }
    //Free allocated memory
    free(filename);
    // Close files
    fclose(memory_card);
    fclose(image);
}