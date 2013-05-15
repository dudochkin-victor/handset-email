/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MESSAGEITEMMODEL_H
#define MESSAGEITEMMODEL_H

#include <QAbstractListModel>
//#include <MListModel>
#include <qmailfolder.h>

#include <qmailmessage.h>

class MessageItemModel : public QAbstractListModel
{
    Q_OBJECT

public:
    MessageItemModel(const QMailMessageIdList &ids);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void addMessage (const QMailMessageId &id);
    void removeMessage (const QMailMessageId &id);
    void update(const QMailMessageIdList &ids);

public slots:
    
private:
    QMailMessageIdList m_messageIds;
};

#endif // MESSAGEITEMMODEL_H

