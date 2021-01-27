#ifndef FILTER_H
#define FILTER_H
#include <QVector>

class Filter
{
private:
    static QVector<double> fir;
    static bool module;
    static QVector<double> filterFFT;
    static int size;

public:
    Filter();
    static inline void setFilter(QVector<double> filter) { fir = filter; }
    static inline QVector<double> getFilter() { return fir; }
    static inline const QVector<double>* getFilterFFT() { return &filterFFT; }
    static inline void setStatusModule() { module = !module; }
    static inline bool getStatusModule() { return module; }
    static inline void setSize(int s) { size = s; }
    static inline int getSize() { return size; }
    static inline double atFilterFFT(int position) { return filterFFT.at(position); }

    static void rectangularWindow(double param);
    static void gaussWindow(double param);
    static void hammingWindow(double alfa, double beta);
    static void blackmanWindow(double alfa);
};

#endif // FILTER_H
