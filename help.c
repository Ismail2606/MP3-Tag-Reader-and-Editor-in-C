#include <stdio.h>

void print_help()
{
    printf("  To view mp3 file contents -> ./a.out -v filename.mp3\n");
    printf("  To edit mp3 file tags     -> ./a.out -e -<tag> new_data filename.mp3\n");
    printf("     -t -> edit title\n");
    printf("     -a -> edit artist name\n");
    printf("     -A -> edit album name\n");
    printf("     -y -> edit year\n");
    printf("     -m -> edit content type\n");
    printf("     -c -> edit comment\n");
}