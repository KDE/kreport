#ifndef KREPORTGROUPTRACKER_H
#define KREPORTGROUPTRACKER_H

#include <QObject>
#include "kreport_export.h"

/*!
 * @brief Keeps track of groups as the data for the group changes
 */
class KREPORT_EXPORT KReportGroupTracker : public QObject {
    Q_OBJECT

protected:
    KReportGroupTracker() {}
    ~KReportGroupTracker() override{}

public:
    Q_SLOT virtual void setGroupData(const QMap<QString, QVariant> &groupData) = 0;
};

#endif // KREPORTGROUPTRACKER_H

