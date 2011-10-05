#include "munittester.h"

namespace mathmod
{

void UnitTester::init (const string &__name, UnitTester *__parent)
{
    _name = __name;
    _parent = __parent;
    _log = stderr;
    _log = _parent ? _parent->get_log() : stderr;
    if (_parent) _parent->add_children (this);
    _errors = 0;
    _closed = 0;
    _started = 0;
}

void UnitTester::add_children (UnitTester *ch)
{
    if (_closed)
        throw MException(TESTERCLOSED);
    if(ch) children.push_back (ch);
}

void UnitTester::print (char *format...)
{
    if (_closed)
        throw MException(TESTERCLOSED);
    if (!_started){
        _started = 1;
        print ("*** %s start", _name.c_str ());
    }
    char buf[256];
    va_list args;
    va_start (args, format);
    vsprintf(buf, format, args);
    if (_parent) _parent->print ("| %s", buf);
    else fprintf (_log, "| %s\n", buf);
    va_end (args);
}

void UnitTester::test (const char *testName, bool result)
{
    if (_closed)
        throw MException(TESTERCLOSED);
    _errors += !result;
    print("%s: %s", testName, result?"passed (+)":"failed (-)");
}

void UnitTester::rem_children(UnitTester *ch)
{
    for (int i = 0; i < children.size (); i++){
        if (ch && ch == children[i]){
            append_errors (children[i]->close());
            children[i] = 0;
        }
    }
}

int UnitTester::close () {
    if (!_closed){
        for (int i = 0; i < children.size (); i++)
            if (children[i])
                append_errors (children[i]->close());
        print ("*** %s: %d errors", _name.c_str (), _errors);
        _closed = 1;
        if (_parent)
            _parent->rem_children(this);
    }
    return _errors;
}

UnitTester::~UnitTester () { close (); }

} // close namespace
