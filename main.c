#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void view_metadata(const char *filename);
void edit_metadata(char *edit, char *data, const char *filename);
void print_help();

int main(int argc, char *argv[])
{
    // Check operation type
    if (strcmp(argv[1], "-v") == 0)
    {
        // Validate for view
        if (argc != 3)
        {
            printf("Error: Invalid argument count for view\n");
            return 1;
        }
        view_metadata(argv[2]);  // argv[2] = filename
    }
    else if (strcmp(argv[1], "-e") == 0)
    {
        // Validate for edit
        if (argc != 5)
        {
            printf("Error: Invalid argument count for edit\n");
            return 1;
        }
        edit_metadata(argv[2], argv[3], argv[4]);
        // argv[2] = filename, argv[3] = frame_id, argv[4] = new_value
    }
    else if (strcmp(argv[1], "--help") == 0)
    {
        print_help();
    }

    return 0;
}
