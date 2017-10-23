#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#define MESSAGE_SIZE 512

void read_message (char *message);
void clear_message (void);
void write_message (char *message);
void write_messagef (char *format, ...);

void long_message (char *title, char **lines);

#endif
