#ifndef MEXCEPTION_H
#define MEXCEPTION_H

#include <string>
using namespace std;

namespace mathmod
{

enum ExceptionCode { NONE,
                     BADSIZE,
                     OUTOFRANGE,
                     ZEROPOINTER,
                     INCOMPATIBLE,
                     TESTERCLOSED
                   };

class MException
{
    protected:
        int _code;
        int _errno;
        string _descr;
    public:
        MException():
            _code(1),
            _errno(0)
        {}

        MException(int __code = NONE,
                   int __errno = 0,
                   const char *__descr = ""):
            _code(__code),
            _errno(__errno)
        {
            _descr = __descr;
            _descr.append ("\n");
        }

        inline int code() const { return _code; }
        inline int errno() const { return _errno; }
        inline const char * descr() const { return _descr.c_str (); }

        ~MException() {}
};


}

#endif // MEXCEPTION_H
