#ifndef PROTOCOL_PARSE_H
#define PROTOCOL_PARSE_H




//get message and length
typedef void (*protocol_parse_callback)(char* message, int length);

class protocol_parse
{
public:
    protocol_parse();
    virtual ~protocol_parse();
    void protocol_parse_in(char cell);
    void protocol_parse_set_callback(protocol_parse_callback callback_func);
    char* protocol_parse_to_message(int in_length, char* in_data, int *out_length);
    char* protocol_parse_last_error();

private:
    class protocol_parse_private;
    protocol_parse_private* m_protocol_parse_private;
};



#endif // PROTOCOL_PARSE_H
