/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef ACCOUNTITEM_H
#define ACCOUNTITEM_H

#include <MWidgetController>
#include <qmailstore.h>

class AccountItem: public MWidgetController
{
    Q_OBJECT

public:
    AccountItem(const QMailAccountId &id);
    virtual ~AccountItem();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void clicked(const QMailAccountId &);

private:
    QMailAccountId  m_id;
};

#endif  // ACCOUNTITEM_H
