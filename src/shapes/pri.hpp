/*
    This file is part of polyquad.
    Copyright (C) 2014  Freddie Witherden <freddie@witherden.org>

    Polyquad is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Polyquad is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with polyquad.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef POLYQUAD_SHAPES_PRI_HPP
#define POLYQUAD_SHAPES_PRI_HPP

#include "shapes/base.hpp"
#include "utils/ortho_poly.hpp"

#include <Eigen/Dense>

#include <cassert>

namespace polyquad {

template<typename T>
class PriDomain : public BaseDomain<PriDomain<T>, T, 3, 6>
{
public:
    typedef BaseDomain<PriDomain<T>, T, 3, 6> Base;
    typedef typename Base::MatrixXT MatrixXT;
    typedef typename Base::VectorXT VectorXT;
    typedef typename Base::MatrixPtsT MatrixPtsT;
    typedef typename Base::VectorOrb VectorOrb;

    typedef Eigen::Matrix<T, 3, 1> Vector3T;

public:
    PriDomain() : Base(2)
    {}

    static bool validate_orbit(const VectorOrb& orb)
    { return orb(0) <= 1; }

private:
    friend class BaseDomain<PriDomain<T>, T, 3, 6>;

    static constexpr int npts_for_orbit[] = {1, 2, 3, 6, 6, 12};
    static constexpr int narg_for_orbit[] = {0, 1, 1, 2, 2,  3};
    static constexpr int nbfn_for_qdeg(int qdeg);

    void expand_orbit(int i, int aoff, int poff,
                      const VectorXT& args, MatrixPtsT& pts) const;

    void seed_orbit(int i, int aoff, VectorXT& args);

    template<typename D1, typename D2>
    void eval_orthob_block(const D1 pqr, D2 out) const;


    static void clamp_arg(int i, int aoff, VectorXT& args);

    static void sort_arg(int i, int aoff, VectorXT& args);

private:
    static Vector3T bary_to_cart(const T& p1, const T& p2, const T& p3,
                                 const T& z)
    { return {-p1 + p2 - p3, -p1 - p2 + p3, z}; }
};

template<typename T>
inline constexpr int
PriDomain<T>::nbfn_for_qdeg(int qdeg)
{
    int n = 0;

    for (int i = 0; i <= qdeg; i += 2)
        for (int j = i; j <= qdeg - i; ++j)
            for (int k = 0; k <= qdeg - i - j; k += 2, ++n);

    return n;
}

template<typename T>
EIGEN_ALWAYS_INLINE void
PriDomain<T>::expand_orbit(int i, int aoff, int poff,
                           const VectorXT& args, MatrixPtsT& pts) const
{
    switch (i)
    {
        case 0:
        {
            const T& a = static_cast<T>(1) / 3;
            pts.row(poff) = bary_to_cart(a, a, a, 0);
            break;
        }
        case 1:
        {
            const T& a = T(1) / 3;
            const T& b = args(aoff);
            pts.row(poff + 0) = bary_to_cart(a, a, a, -b);
            pts.row(poff + 1) = bary_to_cart(a, a, a, b);
            break;
        }
        case 2:
        {
            const T& a = args(aoff);
            pts.row(poff + 0) = bary_to_cart(a, a, 1 - 2*a, 0);
            pts.row(poff + 1) = bary_to_cart(a, 1 - 2*a, a, 0);
            pts.row(poff + 2) = bary_to_cart(1 - 2*a, a, a, 0);
            break;
        }
        case 3:
        {
            const T& a = args(aoff + 0);
            const T& b = args(aoff + 1);
            pts.row(poff + 0) = bary_to_cart(a, a, 1 - 2*a, -b);
            pts.row(poff + 1) = bary_to_cart(a, 1 - 2*a, a, -b);
            pts.row(poff + 2) = bary_to_cart(1 - 2*a, a, a, -b);
            pts.row(poff + 3) = bary_to_cart(a, a, 1 - 2*a, b);
            pts.row(poff + 4) = bary_to_cart(a, 1 - 2*a, a, b);
            pts.row(poff + 5) = bary_to_cart(1 - 2*a, a, a, b);
            break;
        }
        case 4:
        {
            const T& a = args(aoff + 0);
            const T& b = args(aoff + 1);
            pts.row(poff + 0) = bary_to_cart(a, b, 1 - a - b, 0);
            pts.row(poff + 1) = bary_to_cart(a, 1 - a - b, b, 0);
            pts.row(poff + 2) = bary_to_cart(b, a, 1 - a - b, 0);
            pts.row(poff + 3) = bary_to_cart(b, 1 - a - b, a, 0);
            pts.row(poff + 4) = bary_to_cart(1 - a - b, a, b, 0);
            pts.row(poff + 5) = bary_to_cart(1 - a - b, b, a, 0);
            break;
        }
        case 5:
        {
            const T& a = args(aoff + 0);
            const T& b = args(aoff + 1);
            const T& c = args(aoff + 2);
            pts.row(poff + 0)  = bary_to_cart(a, b, 1 - a - b, -c);
            pts.row(poff + 1)  = bary_to_cart(a, 1 - a - b, b, -c);
            pts.row(poff + 2)  = bary_to_cart(b, a, 1 - a - b, -c);
            pts.row(poff + 3)  = bary_to_cart(b, 1 - a - b, a, -c);
            pts.row(poff + 4)  = bary_to_cart(1 - a - b, a, b, -c);
            pts.row(poff + 5)  = bary_to_cart(1 - a - b, b, a, -c);
            pts.row(poff + 6)  = bary_to_cart(a, b, 1 - a - b, c);
            pts.row(poff + 7)  = bary_to_cart(a, 1 - a - b, b, c);
            pts.row(poff + 8)  = bary_to_cart(b, a, 1 - a - b, c);
            pts.row(poff + 9)  = bary_to_cart(b, 1 - a - b, a, c);
            pts.row(poff + 10) = bary_to_cart(1 - a - b, a, b, c);
            pts.row(poff + 11) = bary_to_cart(1 - a - b, b, a, c);
            break;
        }
        default:
            assert(0 && "Bad orbit"), abort();
    }
}

template<typename T>
inline void
PriDomain<T>::seed_orbit(int i, int aoff, VectorXT& args)
{
    auto seeda = [&]() { return this->rand(0, 0.5); };
    auto seedb = [&]() { return this->rand(0, 1.0 / 3.0); };
    auto seedc = [&]() { return sqrt(1 - pow(this->rand(), 2)); };


    switch (i)
    {
        case 0:
            break;
        case 1:
            args(aoff) = seedc();
            break;
        case 2:
            args(aoff) = seeda();
            break;
        case 3:
            args(aoff + 0) = seeda();
            args(aoff + 1) = seedc();
            break;
        case 4:
            args(aoff + 0) = seedb();
            args(aoff + 1) = seedb();
            break;
        case 5:
            args(aoff + 0) = seedb();
            args(aoff + 1) = seedb();
            args(aoff + 2) = seedc();
            break;
        default:
            assert(0 && "Bad orbit"), abort();
    }
}

template<typename T>
template<typename D1, typename D2>
inline void
PriDomain<T>::eval_orthob_block(const D1 pqr, D2 out) const
{
    typedef Eigen::Array<T, D1::RowsAtCompileTime, 1> ArrayT;

    const auto& p = pqr.col(0);
    const auto& q = pqr.col(1);
    const auto& r = pqr.col(2);

    const ArrayT a = (q != 1).select(2*(1 + p)/(1 - q) - 1, 0);
    const auto& b = q;
    const auto& c = r;

    T pow2ip1 = 0.5;

    ArrayT pow1mqi = ArrayT::Constant(p.size(), 1);

    EvenLegendreP<ArrayT> jpa(a);

    for (int i = 0, off = 0; i <= this->qdeg(); i += 2)
    {
        JacobiP<ArrayT> jpb(2*i + 1, 0, b);

        for (int j = i; j <= this->qdeg() - i; ++j)
        {
            EvenLegendreP<ArrayT> jpc(c);

            for (int k = 0; k <= this->qdeg() - i - j; k += 2, ++off)
            {
                T cijk = pow2ip1*sqrt(T((2*i + 1)*(2*k + 1)*(i + j + 1)));

                out.row(off) = cijk*pow1mqi*jpa(i)*jpb(j)*jpc(k);
            }
        }

        pow1mqi *= (1 - b)*(1 - b);
        pow2ip1 /= 4;
    }
}

template<typename T>
inline void
PriDomain<T>::clamp_arg(int i, int aoff, VectorXT& args)
{
    switch (i)
    {
        case 0:
            break;
        case 1:
            args(aoff) = clamp(0, args(aoff), 1);
            break;
        case 2:
            args(aoff) = clamp(0, args(aoff), 0.5);
            break;
        case 3:
            args(aoff + 0) = clamp(0, args(aoff + 0), 0.5);
            args(aoff + 1) = clamp(0, args(aoff + 1), 1);
            break;
        case 4:
            args(aoff + 0) = clamp(0, args(aoff + 0), 1);
            args(aoff + 1) = clamp(0, args(aoff + 1), 1 - args(aoff + 0));
            break;
        case 5:
            args(aoff + 0) = clamp(0, args(aoff + 0), 1);
            args(aoff + 1) = clamp(0, args(aoff + 1), 1 - args(aoff + 0));
            args(aoff + 2) = clamp(0, args(aoff + 2), 1);
            break;
        default:
            assert(0 && "Bad orbit"), abort();
    }
}

template<typename T>
inline void
PriDomain<T>::sort_arg(int i, int aoff, VectorXT& args)
{
    if (i == 4 || i == 5)
    {
        T baryc[] =
        {
            args(aoff + 0),
            args(aoff + 1),
            1 - args(aoff + 0) - args(aoff + 1)
        };
        std::sort(baryc, baryc + 3);
        std::copy(baryc, baryc + 2, args.data() + aoff);
    }
}

}

#endif /* POLYQUAD_SHAPES_PRI_HPP */
