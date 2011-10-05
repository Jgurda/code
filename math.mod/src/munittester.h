#ifndef MUNITTESTER_H
#define MUNITTESTER_H

#include <string>
#include <vector>
using namespace std;

#include <cstdio>
#include <stdarg.h>

#include "mexception.h"

namespace mathmod{

class UnitTester
{
    protected:
        string _name;
        UnitTester * _parent;
        FILE * _log;
        vector<UnitTester *>  children;
        int _errors;
        bool _closed;
        bool _started;
        void init(const string &__name, UnitTester *__parent);
    public:
        UnitTester(const string &__name, UnitTester *__parent = 0)
        { init (__name, __parent); }
        UnitTester(const char *__name, UnitTester *__parent = 0)
        { init (string(__name), __parent); }
        inline FILE * get_log() { return _log; }
        inline void set_log(FILE *log) { _log = log; }
        void add_children(UnitTester * ch);
        void rem_children(UnitTester * ch);
        void print(char * format, ...);
        void test(const char *testName, bool result = 0);
        inline void append_errors(int err) { _errors += err; }
        int close();
        ~UnitTester();
};

} // end of mathmod namespace

#endif // MUNITTESTER_H
