#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void view_metadata(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("File error");
        return;
    }

    //Read and check ID3 header
    char id3_header[3];
    fread(id3_header, 1, 3, fp);
    if (strncmp(id3_header, "ID3", 3) != 0) {
        printf("Not a valid ID3 tag.\n");
        return;
    }

    fseek(fp, 0, SEEK_SET);
    fseek(fp, 10, SEEK_CUR); // Skip version(2) + flags(1) + size(4)

    int i=0;
    while (i<6)
    {
        i++;
        char tag[5] = {0};
        unsigned char size_bytes[4];

        // Read 4 bytes of tag
        if (fread(tag, 1, 4, fp) != 4)
            break;

        // Read 4 bytes of size
        if (fread(size_bytes, 1, 4, fp) != 4)
            break;

        // Convert size to integer (big endian)
        int size = (size_bytes[0] << 24) | (size_bytes[1] << 16) |
                   (size_bytes[2] << 8) | size_bytes[3];

        //Skip 3 bytes (flags + null char)
        fseek(fp, 3, SEEK_CUR);

        //Read (size - 1) bytes of data
        char *data = malloc(size);
        if (!data)
        {
            printf("Memory allocation failed\n");
            break;
        }

        if (fread(data, 1, size - 1, fp) != size - 1)
        {
            free(data);
            break;
        }

        data[size - 1] = '\0';

        //Print the tag and data
        printf("%s: %s\n", tag, data);

        free(data);
    }

    fclose(fp);
}
