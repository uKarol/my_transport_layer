/*
 * cyclic_buffer.c
 *
 *  Created on: 23 cze 2021
 *      Author: Karol
 */

#include <stdint.h>
#include "cyclic_buffer.h"

uint16_t buffer_get_free(cyclic_buffer * buffer)
{
    if( buffer->head_ptr > buffer->tail_ptr )
    {
        return (buffer->max_length) - buffer->head_ptr + buffer->tail_ptr;
    }
    else{
        return buffer->tail_ptr - buffer->head_ptr;
    }

}

uint16_t buffer_get_busy(cyclic_buffer * buffer)
{
    return buffer->max_length - buffer_get_free(buffer) - 1;
}

buffer_state buffer_put(cyclic_buffer * buffer, uint8_t *data_in, uint16_t data_length )
{
    if( buffer->mode == BUFFER_NOT_ACTIVE ) return BUFFER_ERROR;

    else if( data_length < buffer_get_free(buffer)  )
    {
        for( int ctr = 0; ctr < data_length; ctr++ )
        {
            buffer->buffer[buffer->head_ptr] = data_in[ctr];
            buffer->head_ptr = (buffer->head_ptr + 1)%(buffer->max_length);
        }
        return BUFFER_OK;
    }
    else
    {
        return BUFFER_FULL;
    }

}

buffer_state buffer_get(cyclic_buffer *buffer, uint8_t *data_out, uint16_t data_length )
{
    if( buffer->mode == BUFFER_NOT_ACTIVE ) return BUFFER_ERROR;

    else if( data_length <= buffer_get_busy(buffer))
    {
        for( int ctr = 0; ctr < data_length; ctr++ )
        {
            buffer->tail_ptr = (buffer->tail_ptr + 1)%(buffer->max_length);
            data_out[ctr] = buffer->buffer[buffer->tail_ptr];
        }
        return BUFFER_OK;
    }
    else
    {
        return BUFFER_EMPTY;
    }
}
