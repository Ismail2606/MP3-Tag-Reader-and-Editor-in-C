#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int create_backup(const char *filename) {
    char backup_name[512];
    snprintf(backup_name, sizeof(backup_name), "backup.mp3");

    FILE *src = fopen(filename, "rb");
    if (!src) {
        perror("Error opening source file");
        return 0;
    }

    FILE *dst = fopen(backup_name, "wb");
    if (!dst) {
        perror("Error creating backup file");
        fclose(src);
        return 0;
    }

    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dst);
    }

    fclose(src);
    fclose(dst);
    printf("Backup created: %s\n", backup_name);
    return 1;
}

void edit_metadata(char *tag, char *data, const char *filename)
{
    // --- Step 1: Backup before editing ---
    if (!create_backup(filename)) {
        printf("Backup failed. Edit aborted.\n");
        return;
    }

    FILE *fp = fopen(filename, "r+b");
    if (!fp) {
        perror("File error");
        return;
    }

    char id3_header[3];
    fread(id3_header, 1, 3, fp);
    if (strncmp(id3_header, "ID3", 3) != 0) {
        printf("Not a valid ID3 tag.\n");
        fclose(fp);
        return;
    }

    fseek(fp, 10, SEEK_SET); // Skip ID3 header

    char frame_id[5];
    unsigned char size_bytes[4];

    while (fread(frame_id, 1, 4, fp) == 4)
    {
        frame_id[4] = '\0';

        if (fread(size_bytes, 1, 4, fp) != 4) break;
        int size = (size_bytes[0]<<24) | (size_bytes[1]<<16) |
                   (size_bytes[2]<<8)  | size_bytes[3];

        fseek(fp, 2, SEEK_CUR); // skip flags
        long frame_pos = ftell(fp);

        if ((strcmp(tag, "-t")==0 && strcmp(frame_id,"TIT2")==0) ||
            (strcmp(tag, "-a")==0 && strcmp(frame_id,"TPE1")==0) ||
            (strcmp(tag, "-A")==0 && strcmp(frame_id,"TALB")==0) ||
            (strcmp(tag, "-y")==0 && strcmp(frame_id,"TDRC")==0) ||
            (strcmp(tag, "-m")==0 && strcmp(frame_id,"TCON")==0) ||
            (strcmp(tag, "-c")==0 && strcmp(frame_id,"COMM")==0))
        {
            int payload_size = size - 1;   // exclude encoding byte
            int new_len = strlen(data);

            if (new_len > payload_size) {
                printf("Error: new text too long (max %d chars)\n", payload_size);
                fclose(fp);
                return;
            }

            // --- Step 2: Save original text ---
            char *backup_text = malloc(payload_size + 1);
            fseek(fp, frame_pos + 1, SEEK_SET); // skip encoding byte
            fread(backup_text, 1, payload_size, fp);
            backup_text[payload_size] = '\0';

            printf("Original %s: %s\n", frame_id, backup_text);

            // --- Step 3: Write new text ---
            fseek(fp, frame_pos + 1, SEEK_SET);
            fwrite(data, 1, new_len, fp);

            // Pad leftover
            for (int i = new_len; i < payload_size; i++)
                fputc(0, fp);

            printf("Updated %s to: %s\n", frame_id, data);

            free(backup_text);
            fclose(fp);
            return;
        }

        fseek(fp, size, SEEK_CUR); // skip next frame
    }

    printf("Tag not found or unsupported.\n");
    fclose(fp);
}
