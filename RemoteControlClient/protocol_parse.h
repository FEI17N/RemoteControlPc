#ifndef PROTOCOL_PARSE_H
#define PROTOCOL_PARSE_H

//get message and length
typedef void (*protocol_parse_callback)(char* message, int length);

void protocol_parse_in(char cell);
void protocol_parse_set_callback(protocol_parse_callback callback_func);
char* protocol_parse_to_message(int in_length, char* in_data, int *out_length);
char* protocol_parse_last_error();
void free_resource();

#endif // PROTOCOL_PARSE_H
