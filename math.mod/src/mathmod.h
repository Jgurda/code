#ifndef MATHMOD_H
#define MATHMOD_H

#include <vector>
#include <map>
using namespace std;
#include <cmath>

#include "mexception.h"

namespace mathmod
{

#define likely(x) __builtin_expect (!!(x), 1)
#define unlikely(x) __builtin_expect (!!(x), 0)

template<class D>
class Data: public vector<D>
{
    public:
        D& operator [] (int i){
            if (i >= this->size ())
                throw MException(OUTOFRANGE);
            return *(this->begin () + i);
        }

        const D& operator [] (int i) const {
            if (i >= this->size ())
                throw MException(OUTOFRANGE);
            return *(this->begin () + i);
        }
};


template<class D>
class DataCursor
{
    protected:
        Data<D> *_data;
        int _begin;
        int _end;
        int _step;
        int _len;
        void check (const DataCursor<D>& cur) const
        {
            if (cur.len() != len())
                throw MException(INCOMPATIBLE);
        }
    public:
        DataCursor(Data<D> *__data,
                        int __begin,
                        int __end,
                        int __step = 1):
            _data(__data),
            _begin(__begin),
            _end(__end),
            _step(__step)
        {
            if (_step <= 0) throw MException(BADSIZE);
            _len = (_end - 1 - _begin) / _step + 1;
        }

        int len() const { return _len; }

        Data<D> cp() const {
            Data<D> buf;
            for (int i = 0; i < _len ; i++)
                buf.push_back ((*this)[i]);
            return buf;
        }

        const D operator [] (int i) const {
            if (i > len ()) throw MException(OUTOFRANGE);
            return _data->at (_begin + i * _step);
        }

        D& operator [] (int i) {
            if (i > len ()) throw MException(OUTOFRANGE);
            return _data->at (_begin + i * _step);
        }

        DataCursor<D>& operator = (const DataCursor<D> &cur)
        {
            check (cur);
            for (int i = 0; i < len (); i++)
                this->operator [] (i) = cur[i];
            return *this;
        }

        DataCursor<D>& operator = (const Data<D> &cur)
        {
            if (cur.size () != len())
                throw MException(INCOMPATIBLE);
            for (int i = 0; i < len (); i++)
                this->operator [] (i) = cur[i];
            return *this;
        }

        bool operator == (const DataCursor<D> &cur) const
        {
            if (cur.len () != len ()) return 0;
            for (int i = 0; i < len(); i++)
                if (cur[i] != (*this)[i]) return 0;
            return 1;
        }

        bool operator != (const DataCursor<D> &cur) const
        {
            return ! this->operator == (cur);
        }

        DataCursor<D>& operator += (const DataCursor<D> &cur)
        {
            check (cur);
            for (int i = 0; i < len (); i++)
                this->operator [] (i) += cur[i];
            return *this;
        }

        DataCursor<D>& operator -= (const DataCursor<D> &cur)
        {
            check (cur);
            for (int i = 0; i < len (); i++)
                this->operator [] (i) -= cur[i];
            return *this;
        }

        DataCursor<D>& operator *= (const D &b)
        {
            for (int i = 0; i < len (); i++)
                this->operator [] (i) *= b;
            return *this;
        }

        DataCursor<D>& operator /= (const D &b)
        {
            for (int i = 0; i < len (); i++)
                this->operator [] (i) /= b;
            return *this;
        }

        D operator ^ (const DataCursor<D> &cur) const
        {
            check (cur);
            D summ = 0;
            for (int i = 0; i < len (); i++)
                summ += (*this)[i] * cur[i];
            return summ;
        }
};

template<class D>
class Matrix
{
    protected:
        Data<D> data;
        int _w, _h;

        vector<DataCursor<D> *> rows;
        vector<DataCursor<D> *> cols;

        void reinit(int __h, int __w)
        {
            clear();
            _w = __w;
            _h = __h;
            init();
        }

        void clear()
        {
            data.clear ();
            for (int i = 0; i < rows.size (); i++)
                delete rows[i];
            rows.clear ();
            for (int i = 0; i < cols.size (); i++)
                delete cols[i];
            cols.clear ();
        }

        void init()
        {
            for (int i = 0; i < _w*_h; i++)
                data.push_back (0);
            for (int i = 0; i < _h; i++)
                rows.push_back (new DataCursor<D>(&data, _w * i, _w * (i + 1)));
            for (int i = 0; i < _w; i++)
                cols.push_back (new DataCursor<D>(&data, i, _w*_h-_w+i+1, _w));
        }

        bool _transpose;

    public:
        Matrix(int __h = 1, int __w = 1):
            _w(__w),
            _h(__h),
            _transpose(0)
        {
            init();
        }

        Matrix(const Matrix& b):
            _w( b.w () ),
            _h( b.h () ),
            _transpose(0)
        {
            init();
            for (int i = 0; i < _h; i++)
                (*this)[i] = b[i];
        }

        inline void setTranspose(bool t){ _transpose = t; }

        Matrix<D> T() const{
            Matrix<D> r(*this);
            r.setTranspose (1);
            return r;
        }

        /*
        DataCursor<D>& operator [] (int i){
            return DataCursor<D>(&data, w() * i, w() * (i + 1));
        }

        const DataCursor<D>& operator [] (int i) const{
            return DataCursor<D>(const_cast<Data<D> *>(&data), w() * i, w() * (i + 1));
        }

        DataCursor<D>& col (int i){
            return DataCursor<D>(&data, i, i + w() * (h() - 1), w());
        }

        const DataCursor<D>& col (int i) const{
            return DataCursor<D>(const_cast<Data<D> *>(&data), i, i + w() * (h() - 1), w());
        }
        */

        DataCursor<D>& operator [] (int i){ return *(!_transpose?rows[i]:cols[i]); }

        const DataCursor<D>& operator [] (int i) const{
            return *(const_cast<const DataCursor<D> *>(!_transpose?rows[i]:cols[i]));
        }

        DataCursor<D>& col (int i){ return *(!_transpose?cols[i]:rows[i]); }

        const DataCursor<D>& col (int i) const{
            return *(const_cast<const DataCursor<D> *>(!_transpose?cols[i]:rows[i]));
        }


        inline D& at(int i){ return data[i]; }
        inline const D& at(int i) const{ return data[i]; }

        inline int w() const { return !_transpose?_w:_h; }
        inline int h() const { return !_transpose?_h:_w; }

        // math
        Matrix<D> operator + (const Matrix<D> &b) const
        {
            if (_h != b.h ()) throw MException(INCOMPATIBLE);
            Matrix<D> buf(_h, _w);
            for (int i = 0; i < _h; i++)
                buf[i] = (*this)[i] + b[i];
            return buf;
        }

        Matrix<D> operator - (const Matrix<D> &b) const
        {
            if (_h != b.h ()) throw MException(INCOMPATIBLE);
            Matrix<D> buf(_h, _w);
            for (int i = 0; i < _h; i++)
                buf[i] = (*this)[i] - b[i];
            return buf;
        }

        Matrix<D> operator * (const Matrix<D> &b) const
        {
            if (w() != b.h ()) throw MException(INCOMPATIBLE);
            Matrix<D> buf (h(), b.w());
            for (int i = 0; i < b.w(); i++)
                for (int j = 0; j < h(); j++)
                    buf[j][i] = (*this)[j] ^ b.col (i);
            return buf;
        }

        Matrix<D> operator * (const D &b) const
        {
            Matrix<D> buf = (*this);
            for (int i = 0; i < _h; i++)
                buf[i] *= b;
            return buf;
        }
        Matrix<D> operator / (const D &b) const
        {
            Matrix<D> buf = (*this);
            for (int i = 0; i < _h; i++)
                buf[i] /= b;
            return buf;
        }

        // comparison
        bool operator == (const Matrix<D> &b) const
        {
            if (h() != b.h ())
                return 0;
            for (int i = 0; i < h; i++)
                if ((*this)[i] != b[i])
                    return 0;
            return 1;
        }
        bool operator != (const Matrix<D> &b) const
        { return ! this->operator == (b); }

        // assignment
        Matrix<D> operator = (const Matrix<D> &b)
        {
            reinit(b.h (), b.w ());
            for (int i = 0; i < _h; i++)
                (*this)[i] = b[i];
            return *this;
        }

        Matrix<D> operator += (const Matrix<D> &b)
        {
            if (_h != b.h ()) throw MException(INCOMPATIBLE);
            for (int i = 0; i < _h; i++)
                (*this)[i] += b[i];
            return *this;
        }

        Matrix<D> operator -= (const Matrix<D> &b)
        {
            if (_h != b.h ()) throw MException(INCOMPATIBLE);
            for (int i = 0; i < _h; i++)
                (*this)[i] -= b[i];
            return *this;
        }

        Matrix<D> operator *= (const D &b)
        {
            if (_h != b.h ()) throw MException(INCOMPATIBLE);
            for (int i = 0; i < _h; i++)
                (*this)[i] *= b;
            return *this;
        }
        Matrix<D> operator /= (const D &b)
        {
            if (_h != b.h ()) throw MException(INCOMPATIBLE);
            for (int i = 0; i < _h; i++)
                (*this)[i] /= b;
            return *this;
        }

        ~Matrix() { clear (); }
};

template<class D>
class Vector: public Matrix<D>
{
    public:
        Vector(int __len):
            Matrix<D>(1, __len)
        {}

        D& operator [] (int i) { return this->rows[0]->operator [] (i); }
        D operator [] (int i) const { return this->rows[0]->operator [] (i); }

        const DataCursor<D>& row() const { return *(this->rows[0]); }
        DataCursor<D>& row() { return *(this->rows[0]); }

        D operator ^ (const Vector<D> &b) const
        { return *(this->rows[0]) ^ b.row (); }

        D r() const { return sqrt(*(this->rows[0]) ^ *(this->rows[0])); }

        D sr() const { return *(this->rows[0]) ^ *(this->rows[0]); }

};


} // mathmod namespace end

#endif // MATHMOD_H
