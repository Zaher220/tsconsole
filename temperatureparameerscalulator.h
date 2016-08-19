#ifndef TEMPERATUREPARAMEERSCALULATOR_H
#define TEMPERATUREPARAMEERSCALULATOR_H

#include <QObject>

class TemperatureParameersCalulator : public QObject
{
    Q_OBJECT
public:
    explicit TemperatureParameersCalulator(QObject *parent = 0);

signals:

public slots:
};

#endif // TEMPERATUREPARAMEERSCALULATOR_H