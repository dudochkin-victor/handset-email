/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MESSAGEITEM_H
#define MESSAGEITEM_H

#include <QPixmap>
#include <QGraphicsSceneMouseEvent>

#include <MAction>
#include <MStylableWidget>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MImageWidget>
#include <MCancelEvent>

#include <qmailmessage.h>

#include "messageitemstyle.h"

class M_EXPORT MessageItem : public MWidgetController
{

    Q_OBJECT
public:
    MessageItem(MWidget *parent = NULL);
    virtual ~MessageItem() {}

    void createContent (const QMailMessageId &);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent*);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
    virtual void cancelEvent(MCancelEvent*);

signals:
    void clicked();
    void composeReply(QMailMessageId);
    void composeReplyAll(QMailMessageId);
    void composeForward(QMailMessageId);
    void deleteMessage (QMailMessageId);
    void messageItemLongTapped(QMailMessageId);

private slots:
    void deleteActionTriggered();
    void forwardActionTriggered();
    void replyActionTriggered();
    void replyAllActionTriggered();
    void markAsReadActionTriggered();
    void onMessagesUpdated(const QMailMessageIdList&);

private:
    void createActions();
    void updateMarkAsReadUnreadAction();

private:
    MLabel *m_sender;
    MLabel *m_subject;
    MLabel *m_timeStamp;
    bool   m_pressed;
    QMailMessageId m_id;
    MAction *m_markReadUnreadAction;
};

#endif // MESSAGEITEM_H
