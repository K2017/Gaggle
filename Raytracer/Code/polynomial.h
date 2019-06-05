#pragma once

#include <deque>
#include <math.h>
// implementation based on polynomial solver from Graphics Gems

using namespace std;

int SolveQuadric(double c[3], double s[2]);
int SolveCubic(double c[4], double s[3]);
int SolveQuartic(double c[5], double s[4]);
