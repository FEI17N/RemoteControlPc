#include "protocol_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE_BUFF_SIZE 1024
#define MAX_TO_MESSAGE_BUFF_SIZE 1024

const char PROTOCOL_PARSE_HEAD[] = {'<','f','x','_','p','r','o','t','o','c','o','l','_','h','e','a','d','>'};
const char PROTOCOL_PARSE_END[] = {'<','f','x','_','p','r','o','t','o','c','o','l','_','e','n','d','>'};
const char PROTOCOL_PARSE_OVERFIX = '\\';

class protocol_parse::protocol_parse_private
{
public:
    protocol_parse_private();
    virtual ~protocol_parse_private();

    char* PROTOCOL_PARSE_HE_TO;
    char* PROTOCOL_PARSE_HE_TO_BUFF;

    char* PROTOCOL_PARSE_HE;
    char* PROTOCOL_PARSE_HE_BUFF;

    char PROTOCOL_PARSE_BUFF[MAX_MESSAGE_BUFF_SIZE + 1];
    char PROTOCOL_PARSE_RESULT[MAX_MESSAGE_BUFF_SIZE + 1];

    char PROTOCOL_TO_MESSAGE_BUFF[MAX_TO_MESSAGE_BUFF_SIZE + 1];
    char PROTOCOL_TO_CONETENT_BUFF[MAX_TO_MESSAGE_BUFF_SIZE + 1];
    char PROTOCOL_PARSE_ERROR[64];


    protocol_parse_callback this_callback;

    typedef enum
    {
        p_false = 0,
        p_true = !p_false,
        p_error = -1,
    }protocol_parse_bool;

    typedef enum
    {
        p_uninitz,
        p_in,
        p_head,
        p_end,
        p_one,
        p_head_one,
        p_end_one,

        p_in_data,
    }protocol_parse_state;


    char verification_bit(char* data, int length)
    {
        char res = 0;
        for (int i = 0; i < length; ++i)
        {
            res ^= *(data + i);
        }

        return res;
    }

    void iniz_to_buffers()
    {
        int head_length = sizeof(PROTOCOL_PARSE_HEAD);
        int end_length = sizeof(PROTOCOL_PARSE_END);
        int longer_length = (head_length > end_length ? head_length : end_length);

        if (PROTOCOL_PARSE_HE_TO == 0)
        {
            PROTOCOL_PARSE_HE_TO = (char*)malloc(longer_length);
        }

        if (PROTOCOL_PARSE_HE_TO_BUFF == 0)
        {
            PROTOCOL_PARSE_HE_TO_BUFF = (char*)malloc(2 * longer_length);
        }

        memset(PROTOCOL_PARSE_HE_TO, 0, longer_length);
        memset(PROTOCOL_PARSE_HE_TO_BUFF, 0, 2 * longer_length);
        memset(PROTOCOL_TO_MESSAGE_BUFF, 0, MAX_TO_MESSAGE_BUFF_SIZE + 1);
        memset(PROTOCOL_TO_CONETENT_BUFF, 0, MAX_TO_MESSAGE_BUFF_SIZE + 1);
        return;
    }

    void free_resource()
    {
        if (PROTOCOL_PARSE_HE_TO != 0)
        {
            free(PROTOCOL_PARSE_HE_TO);
        }
        if (PROTOCOL_PARSE_HE_TO_BUFF != 0)
        {
            free(PROTOCOL_PARSE_HE_TO_BUFF);
        }

        if (PROTOCOL_PARSE_HE != 0)
        {
            free(PROTOCOL_PARSE_HE);
        }

        if (PROTOCOL_PARSE_HE_BUFF != 0)
        {
            free(PROTOCOL_PARSE_HE_BUFF);
        }
    }

    protocol_parse_bool is_head(char* buff)
    {
        int head_length = sizeof(PROTOCOL_PARSE_HEAD);
        for (int i = 0; i < head_length; ++i)
        {
            if (PROTOCOL_PARSE_HEAD[i] != *(buff + i))
            {
                return p_false;
            }
        }
        return p_true;
    }

    protocol_parse_bool is_end(char* buff)
    {
        int end_length = sizeof(PROTOCOL_PARSE_END);
        for (int i = 0; i < end_length; ++i)
        {
            if (PROTOCOL_PARSE_END[i] != *(buff + i))
            {
                return p_false;
            }
        }
        return p_true;
    }

    protocol_parse_state to_trans_head_end_buff(char in_char, int* flag, char* out_char, int *short_offset, char* PROTOCOL_PARSE_HE_A, char* PROTOCOL_PARSE_HE_A_BUFF)
    {
        int head_length = sizeof(PROTOCOL_PARSE_HEAD);
        int end_length = sizeof(PROTOCOL_PARSE_END);
        int longer_length = (head_length > end_length ? head_length : end_length);
        int shorter_length = (head_length < end_length ? head_length : end_length);

        protocol_parse_bool first_full = p_false;

        if ((*flag) <(longer_length -1))
        {
            (*flag) += 1;
            *(PROTOCOL_PARSE_HE_A + (*flag)) = in_char;

            if ((*flag) == (longer_length - 1))
            {
                first_full = p_true;
            }
            else
            {
                first_full = p_false;
            }

            if (((*flag) + 1) >= shorter_length)
            {
                *short_offset = ((*flag) + 1) - shorter_length;
            }

        }
        else
        {
            first_full = p_false;
            *out_char = *PROTOCOL_PARSE_HE_A;
            memmove(PROTOCOL_PARSE_HE_A, PROTOCOL_PARSE_HE_A + 1, longer_length - 1);
            *(PROTOCOL_PARSE_HE_A + (*flag)) = in_char;
        }

        int total_length = (*flag) + 1;
        if (total_length >= head_length || total_length >= end_length)
        {
            if (head_length > end_length)
            {
                if (is_end(PROTOCOL_PARSE_HE_A + *short_offset))
                {
                    memcpy(PROTOCOL_PARSE_HE_A_BUFF, PROTOCOL_PARSE_HE_A, *short_offset);
                    memmove(PROTOCOL_PARSE_HE_A, PROTOCOL_PARSE_HE_A + *short_offset + shorter_length, total_length - shorter_length - *short_offset);
                    (*flag) = total_length - shorter_length - *short_offset - 1;
                    return (total_length < longer_length || first_full == p_true) ? p_end : p_end_one;
                }
                else
                {
                    if (total_length >= head_length)
                    {
                        if (is_head(PROTOCOL_PARSE_HE_A))
                        {
                            memset(PROTOCOL_PARSE_HE_A, 0, longer_length);
                            (*flag) = -1;
                            return (total_length < longer_length || first_full == p_true) ? p_head : p_head_one;
                        }
                        else
                        {
                            if (total_length < longer_length || first_full == p_true)
                            {
                                return p_in;
                            }
                        }
                    }
                    else
                    {
                        return p_in;
                    }
                }

            }
            else // (head_length <= end_length)
            {


                if (is_head(PROTOCOL_PARSE_HE_A + *short_offset))
                {
                    memcpy(PROTOCOL_PARSE_HE_A_BUFF, PROTOCOL_PARSE_HE_A, *short_offset);
                    memmove(PROTOCOL_PARSE_HE_A, PROTOCOL_PARSE_HE_A + *short_offset + shorter_length, total_length - shorter_length - *short_offset);
                    (*flag) = total_length - shorter_length - *short_offset - 1;
                    return (total_length < longer_length || first_full == p_true) ? p_head : p_head_one;
                }
                else
                {
                    if (total_length >= end_length)
                    {
                        if (is_end(PROTOCOL_PARSE_HE_A))
                        {
                            memset(PROTOCOL_PARSE_HE_A, 0, longer_length);
                            (*flag) = -1;
                            return (total_length < longer_length || first_full == p_true) ? p_end : p_end_one;
                        }
                        else
                        {
                            if (total_length < longer_length || first_full == p_true)
                            {
                                return p_in;
                            }
                        }
                    }
                    else
                    {
                        return p_in;
                    }
                }

            }


        }
        else
        {
            return p_in;
        }

        return p_one;
    }

    protocol_parse_bool to_trans_message(int in_length, char* in_data, int* out_length)
    {
        iniz_to_buffers();

        int flag = -1;
        char out_char;
        int short_offset = 0;
        int start_index = -1;
        int head_length = sizeof(PROTOCOL_PARSE_HEAD);
        int end_length = sizeof(PROTOCOL_PARSE_END);
        //int shorter_length = (head_length < end_length ? head_length : end_length);
        int longer_length = (head_length > end_length ? head_length : end_length);
        protocol_parse_state state;

        for (int i = 0; i < in_length; ++i)
        {
            state = to_trans_head_end_buff(*(in_data+ i), &flag, &out_char, &short_offset, PROTOCOL_PARSE_HE_TO, PROTOCOL_PARSE_HE_TO_BUFF);
            switch (state) {
            case p_in:
                //nop
                break;
            case p_one:
            {
                if (start_index + 1 > MAX_TO_MESSAGE_BUFF_SIZE)
                {
                    return p_error;
                }
                *(PROTOCOL_TO_CONETENT_BUFF + start_index + 1) = out_char;
                start_index += 1;
            }
                break;
            case p_head:
            case p_head_one:
            {
                if (state == p_head_one)
                {
                    if (start_index + 1 > MAX_TO_MESSAGE_BUFF_SIZE)
                    {
                        return p_error;
                    }
                    *(PROTOCOL_TO_CONETENT_BUFF + start_index + 1) = out_char;
                    start_index += 1;
                }
                if (head_length < end_length)
                {
                    if (start_index + short_offset > MAX_TO_MESSAGE_BUFF_SIZE)
                    {
                        return p_error;
                    }
                    memcpy(PROTOCOL_TO_CONETENT_BUFF + start_index + 1, PROTOCOL_PARSE_HE_TO_BUFF, short_offset);
                    start_index += short_offset;

                }

                if (start_index + 2*head_length + 1> MAX_TO_MESSAGE_BUFF_SIZE)
                {
                    return p_error;
                }

                for (int is = 0; is < head_length; ++is)
                {
                    *(PROTOCOL_PARSE_HE_TO_BUFF + 2*is) = PROTOCOL_PARSE_OVERFIX;
                    *(PROTOCOL_PARSE_HE_TO_BUFF + 2*is + 1) = PROTOCOL_PARSE_HEAD[is];
                }
                memcpy(PROTOCOL_TO_CONETENT_BUFF + start_index + 1, PROTOCOL_PARSE_HE_TO_BUFF, 2*head_length);
                start_index += 2*head_length;
            }
                break;
            case p_end:
            case p_end_one:
                if (state == p_end_one)
                {
                    if (start_index + 1 > MAX_TO_MESSAGE_BUFF_SIZE)
                    {
                        return p_error;
                    }
                    *(PROTOCOL_TO_CONETENT_BUFF + start_index + 1) = out_char;
                    start_index += 1;
                }
                if (head_length > end_length)
                {
                    if (start_index + short_offset > MAX_TO_MESSAGE_BUFF_SIZE)
                    {
                        return p_error;
                    }
                    memcpy(PROTOCOL_TO_CONETENT_BUFF + start_index + 1, PROTOCOL_PARSE_HE_TO_BUFF, short_offset);
                    start_index += short_offset;

                }
                if (start_index + 2*end_length + 1 > MAX_TO_MESSAGE_BUFF_SIZE)
                {
                    return p_error;
                }
                for (int is = 0; is < end_length; ++is)
                {
                    *(PROTOCOL_PARSE_HE_TO_BUFF + 2*is) = PROTOCOL_PARSE_OVERFIX;
                    *(PROTOCOL_PARSE_HE_TO_BUFF + 2*is + 1) = PROTOCOL_PARSE_END[is];
                }
                memcpy(PROTOCOL_TO_CONETENT_BUFF + start_index + 1, PROTOCOL_PARSE_HE_TO_BUFF, 2*end_length);
                start_index += 2*end_length;

                break;
            default:
                break;
            }

            if (i == in_length - 1)
            {
                if (state == p_one || ((state == p_in && flag < longer_length) && flag > -1))
                {
                    if (start_index + flag + 1 > MAX_TO_MESSAGE_BUFF_SIZE)
                    {
                        return p_error;
                    }
                    memcpy(PROTOCOL_TO_CONETENT_BUFF + start_index + 1, PROTOCOL_PARSE_HE_TO, flag + 1);
                    start_index += (flag + 1);
                }

            }
        }

        *out_length = (start_index + 1);

        return p_true;
    }


    ////////////////////////////////////////////////////////////////////////////

    int protocol_parse_flag;
    char protocol_parse_out_char;
    int protocol_parse_short_offset;
    int protocol_parse_length;
    int protocol_parse_start_index;
    protocol_parse_state protocol_parse_state_value;
    void protocol_parse_in(char cell)
    {
        int head_length = sizeof(PROTOCOL_PARSE_HEAD);
        int end_length = sizeof(PROTOCOL_PARSE_END);
        //int shorter_length = (head_length < end_length ? head_length : end_length);
        int longer_length = (head_length > end_length ? head_length : end_length);

        if (PROTOCOL_PARSE_HE == 0)
        {
            PROTOCOL_PARSE_HE = (char*)malloc(longer_length);
            memset(PROTOCOL_PARSE_HE, 0, longer_length);
        }

        if (PROTOCOL_PARSE_HE_BUFF == 0)
        {
            PROTOCOL_PARSE_HE_BUFF = (char*)malloc(2 * longer_length);
            memset(PROTOCOL_PARSE_HE_BUFF, 0, longer_length);
        }

        protocol_parse_state state =
                to_trans_head_end_buff(
                    cell,
                    &protocol_parse_flag,
                    &protocol_parse_out_char,
                    &protocol_parse_short_offset,
                    PROTOCOL_PARSE_HE, PROTOCOL_PARSE_HE_BUFF);

        if (state == p_head || state == p_head_one)
        {
            protocol_parse_state_value = p_in_data;
            protocol_parse_start_index = 0;
            memset(PROTOCOL_PARSE_BUFF, 0, sizeof(PROTOCOL_PARSE_BUFF));
            memset(PROTOCOL_PARSE_RESULT, 0, sizeof(PROTOCOL_PARSE_RESULT));
        }
        else if (state == p_end || state == p_end_one)
        {
            if (state == p_end_one)
            {
                if ((protocol_parse_start_index + 1) > MAX_MESSAGE_BUFF_SIZE)
                {
                    protocol_parse_state_value = p_in_data;
                    protocol_parse_start_index = 0;
                    memset(PROTOCOL_PARSE_BUFF, 0, sizeof(PROTOCOL_PARSE_BUFF));
                    memset(PROTOCOL_PARSE_RESULT, 0, sizeof(PROTOCOL_PARSE_RESULT));
                    strcpy(PROTOCOL_PARSE_ERROR, "RECIVE_MESSAGE_OVER_LENGTH");
                    return;
                }
                else
                {
                    PROTOCOL_PARSE_BUFF[protocol_parse_start_index] = protocol_parse_out_char;
                    protocol_parse_start_index += 1;
                }
            }
            if (head_length > end_length)
            {
                if ((protocol_parse_start_index + protocol_parse_short_offset) > MAX_MESSAGE_BUFF_SIZE)
                {
                    protocol_parse_state_value = p_in_data;
                    protocol_parse_start_index = 0;
                    memset(PROTOCOL_PARSE_BUFF, 0, sizeof(PROTOCOL_PARSE_BUFF));
                    memset(PROTOCOL_PARSE_RESULT, 0, sizeof(PROTOCOL_PARSE_RESULT));
                    strcpy(PROTOCOL_PARSE_ERROR, "RECIVE_MESSAGE_OVER_LENGTH");
                    return;
                }
                else
                {
                    memcpy(PROTOCOL_PARSE_BUFF + protocol_parse_start_index, PROTOCOL_PARSE_HE_BUFF, protocol_parse_short_offset);
                    protocol_parse_start_index += protocol_parse_short_offset;
                }
            }

            if (protocol_parse_state_value == p_in_data)
            {
                if (protocol_parse_start_index < 2)
                {
                    protocol_parse_state_value = p_uninitz;
                    protocol_parse_start_index = 0;
                    memset(PROTOCOL_PARSE_BUFF, 0, sizeof(PROTOCOL_PARSE_BUFF));
                    memset(PROTOCOL_PARSE_RESULT, 0, sizeof(PROTOCOL_PARSE_RESULT));
                    strcpy(PROTOCOL_PARSE_ERROR, "RECIVE_MESSAGE_TOO_SHORT");
                    return;
                }

                char verify = verification_bit(PROTOCOL_PARSE_BUFF, protocol_parse_start_index - 1);
                if (verify != PROTOCOL_PARSE_BUFF[protocol_parse_start_index - 1])
                {
                    protocol_parse_state_value = p_uninitz;
                    protocol_parse_start_index = 0;
                    memset(PROTOCOL_PARSE_BUFF, 0, sizeof(PROTOCOL_PARSE_BUFF));
                    memset(PROTOCOL_PARSE_RESULT, 0, sizeof(PROTOCOL_PARSE_RESULT));
                    strcpy(PROTOCOL_PARSE_ERROR, "RECIVE_MESSAGE_VERIFY_ERROR");
                    return;
                }

                protocol_parse_bool over_fix = p_false;
                int result_index = 0;
                for (int i = 0; i < protocol_parse_start_index - 1; ++i)
                {
                    if (over_fix || PROTOCOL_PARSE_BUFF[i] != PROTOCOL_PARSE_OVERFIX)
                    {
                        PROTOCOL_PARSE_RESULT[result_index++] = PROTOCOL_PARSE_BUFF[i];
                        over_fix = p_false;
                    }
                    else if (!over_fix && PROTOCOL_PARSE_BUFF[protocol_parse_start_index] == PROTOCOL_PARSE_OVERFIX)
                    {
                        over_fix = p_true;
                    }
                }


                if (this_callback != null_protocol_parse_callback_function_ptr)
                {
#if __cplusplus < 201402L && (defined _MSC_VER && _MSC_VER < 1800)
                    (*this_callback)(PROTOCOL_PARSE_RESULT, result_index);
#else
                    this_callback(PROTOCOL_PARSE_RESULT, result_index);
#endif
                    protocol_parse_start_index = 0;
                    memset(PROTOCOL_PARSE_BUFF, 0, sizeof(PROTOCOL_PARSE_BUFF));
                    memset(PROTOCOL_PARSE_RESULT, 0, sizeof(PROTOCOL_PARSE_RESULT));
                    protocol_parse_state_value = p_uninitz;
                    return;
                }
            }
        }
        else if (state == p_one)
        {
            if ((protocol_parse_start_index + 1) > MAX_MESSAGE_BUFF_SIZE)
            {
                protocol_parse_state_value = p_in_data;
                protocol_parse_start_index = 0;
                memset(PROTOCOL_PARSE_BUFF, 0, sizeof(PROTOCOL_PARSE_BUFF));
                memset(PROTOCOL_PARSE_RESULT, 0, sizeof(PROTOCOL_PARSE_RESULT));
                strcpy(PROTOCOL_PARSE_ERROR, "RECIVE_MESSAGE_OVER_LENGTH");
                return;
            }
            else
            {
                PROTOCOL_PARSE_BUFF[protocol_parse_start_index] = protocol_parse_out_char;
                protocol_parse_start_index += 1;
                return;
            }
        }



    }

    void protocol_parse_set_callback(protocol_parse_callback callback_func)
    {
        this_callback = callback_func;
    }

    char* protocol_parse_to_message(int in_length, char* in_data, int *out_length)
    {
        int head_length = 0;
        int end_length = 0;
        int verification_length = 0;
        char verification_charater = 0xff;
        int total_length = 0;
        char* out_buff = 0;
        int content_length = 0;

        if (to_trans_message(in_length, in_data, &content_length) == p_error)
        {
            strcpy(PROTOCOL_PARSE_ERROR, "CONTENT_OVER_LENGTH");
            return 0;
        }

        head_length = sizeof(PROTOCOL_PARSE_HEAD);
        end_length = sizeof(PROTOCOL_PARSE_END);
        verification_charater = verification_bit(in_data, in_length);
        verification_length = sizeof(verification_charater);

        total_length = head_length + in_length + verification_length + end_length;
        if (total_length > MAX_MESSAGE_BUFF_SIZE)
        {
            strcpy(PROTOCOL_PARSE_ERROR, "MESSAGE_OVER_LENGTH");
            return 0;
        }

        out_buff = PROTOCOL_TO_MESSAGE_BUFF;

        if (out_buff == 0)
        {
            strcpy(PROTOCOL_PARSE_ERROR, "MALLOC_MEM_FAILURE");
            return 0;
        }

        memcpy(out_buff, PROTOCOL_PARSE_HEAD, head_length);
        memcpy(out_buff + head_length, PROTOCOL_TO_CONETENT_BUFF, content_length);
        memcpy(out_buff + head_length + content_length, &verification_charater, verification_length);
        memcpy(out_buff + head_length + content_length + verification_length, PROTOCOL_PARSE_END, end_length);
        *out_length = head_length + content_length + verification_length + end_length;
        return out_buff;
    }

    char* protocol_parse_last_error()
    {
        return PROTOCOL_PARSE_ERROR;
    }
};

protocol_parse::protocol_parse_private::protocol_parse_private()
{
    PROTOCOL_PARSE_HE_TO = 0;
    PROTOCOL_PARSE_HE_TO_BUFF = 0;

    PROTOCOL_PARSE_HE = 0;
    PROTOCOL_PARSE_HE_BUFF = 0;

    memset(PROTOCOL_PARSE_BUFF, 0, MAX_MESSAGE_BUFF_SIZE + 1);
    memset(PROTOCOL_PARSE_RESULT, 0, MAX_MESSAGE_BUFF_SIZE + 1);

    memset(PROTOCOL_TO_MESSAGE_BUFF, 0, MAX_TO_MESSAGE_BUFF_SIZE + 1);
    memset(PROTOCOL_TO_CONETENT_BUFF, 0, MAX_TO_MESSAGE_BUFF_SIZE + 1);

    memset(PROTOCOL_PARSE_ERROR, 0, 64);
    this_callback = 0;

    protocol_parse_flag = -1;
    protocol_parse_out_char = 0;
    protocol_parse_short_offset = 0;
    protocol_parse_length = 0;
    protocol_parse_start_index = 0;
    protocol_parse_state_value = p_uninitz;
}

protocol_parse::protocol_parse_private::~protocol_parse_private()
{
    free_resource();
}

protocol_parse::protocol_parse()
{
    m_protocol_parse_private = new protocol_parse_private;
}

protocol_parse::~protocol_parse()
{
    delete m_protocol_parse_private;
}


void protocol_parse::protocol_parse_in(char cell)
{
    m_protocol_parse_private->protocol_parse_in(cell);
}

void protocol_parse::protocol_parse_set_callback(protocol_parse_callback callback_func)
{
    m_protocol_parse_private->protocol_parse_set_callback(callback_func);
}

char* protocol_parse::protocol_parse_to_message(int in_length, char* in_data, int *out_length)
{
    return m_protocol_parse_private->protocol_parse_to_message(in_length, in_data, out_length);
}

char* protocol_parse::protocol_parse_last_error()
{
    return m_protocol_parse_private->protocol_parse_last_error();
}



