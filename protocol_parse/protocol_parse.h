#ifndef PROTOCOL_PARSE_H
#define PROTOCOL_PARSE_H




//get message and length
#if __cplusplus < 201402L && (defined _MSC_VER && _MSC_VER < 1800)
    #define null_protocol_parse_callback_function_ptr NULL
    typedef void (*protocol_parse_callback)(char* message, int length);
#else
    #include <iostream>
    #include <functional>
    #define null_protocol_parse_callback_function_ptr nullptr
    #define protocol_parse_callback std::function<void(char* msg, int len)>
#endif


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
