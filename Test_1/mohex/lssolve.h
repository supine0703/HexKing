#ifndef LSSOLVE_H
#define LSSOLVE_H

#include "mat.hpp"

/**
 * Solver for linear equation system.
 *
 * @return x where Ax=b
 */
Vec<double> lsSolve(const Mat<double> &A, const Vec<double> &b);


#endif // LSSOLVE_H
