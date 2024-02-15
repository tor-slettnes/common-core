/// -*- c++ -*-
//==============================================================================
/// @file streambuffer.c++
/// @brief C++ stream buffer interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "streambuffer.h++"

#include <string.h>

namespace shared::io
{
    // buffer management
    int StreamBuffer::sync()
    {
        if (auto remaining = this->pptr() - this->pbase())
        {
            return this->xsputn(this->pbase(), remaining) ? 0 : traits_type::eof();
        }
        else
        {
            return 0;
        }
    }

    //==========================================================================
    // get area

    std::streamsize StreamBuffer::showmanyc()
    {
        std::streamsize howmany = this->egptr() - this->gptr();
        return howmany;
    }

    std::streamsize StreamBuffer::xsgetn(char_type *__s, std::streamsize __n)
    {
        std::streamsize total = 0;
        while (total < __n)
        {
            if (std::streamsize delta = std::min(__n, this->egptr() - this->gptr()))
            {
                memcpy(__s, this->gptr(), delta);
                this->gbump(delta);
                total += delta;
            }

            if ((total < __n) && (this->underflow() == traits_type::eof()))
            {
                break;
            }
        }
        return total;
    }

    StreamBuffer::int_type StreamBuffer::underflow()
    {
        std::streamsize size = 0;
        if (!this->__input_completed && this->read_some(&this->__input))
        {
            size = this->__input.size();
        }
        else
        {
            this->__input_completed = true;
        }

        this->setg(this->__input.data(),          // gbeg
                   this->__input.data(),          // gnext
                   this->__input.data() + size);  // gend

        return size ? *this->gptr() : traits_type::eof();
    }

    StreamBuffer::int_type StreamBuffer::pbackfail(int_type __c)
    {
        if (this->gptr() > this->eback())
        {
            this->gbump(-1);
            *this->gptr() = __c;

            return __c;
        }
        else
        {
            return traits_type::eof();
        }
    }

    //==========================================================================
    // put area

    std::streamsize StreamBuffer::xsputn(const char_type *__s, std::streamsize __n)
    {
        return this->write_some(std::string(__s, __n)) ? __n : 0;
    }

    StreamBuffer::int_type StreamBuffer::overflow(int_type __c)
    {
        if ((__c != traits_type::eof()) &&
            this->write_some(std::string(1, __c)))
        {
            return __c;
        }
        else
        {
            return traits_type::eof();
        }
    }

}  // namespace shared::io
