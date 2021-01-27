#include "filter.h"
#include <QDebug>
#include <armadillo>

QVector<double> Filter::fir = {1};
bool Filter::module = false;
QVector<double> Filter::filterFFT = {1};
int Filter::size = 1;

Filter::Filter() {}

void Filter::rectangularWindow(double param)
{
    filterFFT.clear();
    for(int i = 0; i < size; i++)
        filterFFT.push_back(param);
}

void Filter::gaussWindow(double param)
{
    filterFFT.clear();
    for(int n = 0; n < size; n++)
        filterFFT.push_back(exp( (-0.5)* pow(((n-(size-1.0)/2.0)/(param*(size-1.0)/2.0)), 2)));
}

void Filter::hammingWindow(double alfa, double beta)
{
    filterFFT.clear();
    for(int n = 0; n < size; n++)
        filterFFT.push_back(alfa - beta*cos( (2*M_PI*n)/(size-1) ));
}

void Filter::blackmanWindow(double alfa)
{
    double a0 = (1-alfa)/2;
    double a1 = 0.5;
    double a2 = alfa/2;

    filterFFT.clear();
    for(int n = 0; n < size; n++)
        filterFFT.push_back(a0 - a1*cos( (2*M_PI*n)/(size-1.0) ) + a2*cos( (4*M_PI*n)/(size-1.0) ));
}
