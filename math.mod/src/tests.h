#ifndef TESTS_H
#define TESTS_H

#include "mathmod.h"
#include "munittester.h"
#include <iostream>
using namespace std;

using namespace mathmod;

void testUnitTester(UnitTester *parent)
{
    UnitTester m("self testing", parent);
    UnitTester t("self testing");
    t.set_log (fopen ("/dev/null", "w"));
    t.test ("true", 1);
    t.test ("false");
    UnitTester t2("inner test", &t);
    t2.test ("inner true", 1);
    t2.test ("inner false");
    m.test ("inner test", t2.close () == 1);
    m.test ("parent test", t.close () == 2);
}

void printMatrix(Matrix<double> m)
{
    FILE * _f = stderr;
    for (int i = 0; i < m.h (); i++){
        for (int j = 0; j < m.w (); j++)
            fprintf(_f, "%f ", m[i][j]);
        fprintf (_f, "\n");
    }
}

void testMatrix(UnitTester *parent)
{
    UnitTester m("matrix test", parent);
    Matrix<double> mm(3,5);
    mm[0][0] = 3;
    mm[0][1] = 5;
    mm[0][2] = 6.384;
    mm[0][3] = 7.1;
    mm[0][4] = 2.345;
    m.test ("mtr first test", mm[0][0] == 3);
    printMatrix (mm);
    printMatrix (mm.T ());
    Vector<double> vv(3);
    vv[0] = 3;
    vv[1] = 5;
    vv[2] = 1;
    Vector<double> vv2(3);
    vv2[0] = 5;
    vv2[1] = 3;
    vv2[2] = 1;
    fprintf (stderr, "r = %f\nsr = %f\n^ = %f\n", vv2.r (), vv.sr (), vv2^vv);
    printMatrix (vv);
    printMatrix (vv.T ());
    Matrix<double> mult = vv * mm;
    printMatrix (mult);
}

int maintest()
{
    UnitTester t("main test");
    testUnitTester (&t);
    testMatrix (&t);
    return t.close ();
}

#endif // TESTS_H
