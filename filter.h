#ifndef FILTER_H
#define FILTER_H
#include <QVector>

class Filter
{
private:
    static QVector<double> fir;

public:
    Filter();
    static inline void setFilter(QVector<double> filter) { fir = filter; }
    static inline QVector<double> getFilter() { return fir; }
};

#endif // FILTER_H
