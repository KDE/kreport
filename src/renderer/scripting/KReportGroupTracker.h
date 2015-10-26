#ifndef KREPORTGROUPTRACKER_H
#define KREPORTGROUPTRACKER_H

#include <QObject>
#include "kreport_export.h"

class KREPORT_EXPORT KReportGroupTracker : public QObject {
    Q_OBJECT

protected:
    KReportGroupTracker() {}
    virtual ~KReportGroupTracker(){}

public:
    Q_SLOT virtual void setGroupData(const QMap<QString, QVariant> &groupData) = 0;
};

#endif // KREPORTGROUPTRACKER_H

