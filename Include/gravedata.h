#ifndef GRAVE_DATA_H
#define GRAVE_DATA_H
#include <iostream>

namespace ian
{
    class GraveData
    {
        public:

        struct Changes
        {
            Changes(int i, int d):
            increments(i),
            decrements(d)
            {}
            int increments;
            int decrements;
        };

        GraveData()
        {
            _data = 0;
            inc();
        }
        GraveData(const GraveData& other)
        {
            inc();
            _data = other._data;
        }
        GraveData(GraveData&& other)
        {
            inc();
            _data = other._data;
        }
        GraveData(int t)
        {
            inc();
            _data = t;
        }
        ~GraveData()
        {
            dec();
        }

        operator int() const
        {
            return _data;
        }

        GraveData& operator = (const GraveData& other)
        {
            _data = other._data;
            return *this;
        }

        GraveData& operator = (GraveData&& other)
        {
            _data = other._data;
            return *this;
        }

        GraveData& operator = (const int& t)
        {
            _data = t;
            return *this;
        }

        friend std::ostream& operator << (std::ostream& out, const GraveData& gd)
        {
            out << gd._data;
            return out;
        }

        friend std::istream& operator >> (std::istream& in, GraveData& gd)
        {
            in >> gd._data;
            return in;
        }

        static void init()
        {
            _count = 0;
            _incs = 0;
            _decs = 0;
        }

        static int count()
        {
            return _count;
        }

        static Changes changes()
        {
            Changes mods(_incs, _decs);
            _incs = 0;
            _decs = 0;
            return mods;
        }



        private:

        static inline void inc()
        {
            ++_count;
            ++_incs;
        }

        static inline void dec()
        {
            --_count;
            ++_decs;
        }

        static int _count;
        static int _incs;
        static int _decs;
        int _data;
    };
}
int ian::GraveData::_count = 0;
int ian::GraveData::_decs = 0;
int ian::GraveData::_incs = 0;


#endif