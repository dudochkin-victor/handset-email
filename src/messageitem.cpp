/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDebug>

#include <MAction>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MWidgetCreator>
#include <MImageWidget>
#include <MLabel>
#include <qmailmessage.h>
#include <qmailstore.h>

#include "messageitem.h"
#include "messageitemmodel.h"
#include "messageitemstyle.h"
#include "mailmanager.h"
#include "emailapplication.h"
#include "emailutils.h"

M_REGISTER_WIDGET(MessageItem);

MessageItem::MessageItem(MWidget *parent)
    : m_pressed(false), m_markReadUnreadAction(NULL)
{
    Q_UNUSED(parent);
    this->setViewType("background");
    MLayout *layout = new MLayout();
    layout->setAnimation(NULL);
    layout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    setLayout(layout);

    MStylableWidget *senderRow = new MStylableWidget();
    MLayout *senderLayout = new MLayout(senderRow);
    MGridLayoutPolicy *senderPolicy = new MGridLayoutPolicy(senderLayout);

    m_sender = new MLabel;
    m_sender->setTextElide(true);
    m_sender->setWordWrap(false);
    m_sender->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    senderPolicy->addItem(m_sender, 0, 0, Qt::AlignLeft);

    m_timeStamp = new MLabel;
    m_timeStamp->setTextElide(true);
    m_timeStamp->setWordWrap(false);
    m_timeStamp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    senderPolicy->addItem(m_timeStamp, 0, 1, Qt::AlignRight);

    m_subject = new MLabel;
    m_subject->setTextElide(true);
    m_subject->setWordWrap(false);
    m_subject->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    policy->addItem(m_subject);
    policy->addItem(senderRow);

    createActions();
}

void MessageItem::createContent(const QMailMessageId & id)
{
    m_id = id;  // saves the message id
    QMailMessage message (id);

    // Extract sender's email address and Subject from message
    //% "From: %1"
    m_sender->setText(QString(qtTrId("xx_from_sender").arg(message.from().address())));
    //% "Subject: %1"
    m_subject->setText(QString(qtTrId("xx_subject_text").arg(message.subject())));
    m_timeStamp->setText(Utils::humanDateTime(message.receivedDate().toLocalTime()));

    updateMarkAsReadUnreadAction();
}

void MessageItem::createActions()
{
    //% "Delete"
    MAction *action = new MAction(qtTrId("xx_delete_action"), this);
    action->setLocation(MAction::ObjectMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(deleteActionTriggered()));

    //% "Forward"
    action = new MAction(qtTrId("xx_forward_action"), this);
    action->setLocation(MAction::ObjectMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(forwardActionTriggered()));

    //% "Reply"
    action = new MAction(qtTrId("xx_reply_action"), this);
    action->setLocation(MAction::ObjectMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(replyActionTriggered()));

    //% "Reply to all"
    action = new MAction(qtTrId("xx_reply_all_action"), this);
    action->setLocation(MAction::ObjectMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(replyAllActionTriggered()));

    m_markReadUnreadAction = new MAction("", this);
    m_markReadUnreadAction->setLocation(MAction::ObjectMenuLocation);
    addAction(m_markReadUnreadAction);
    connect(m_markReadUnreadAction, SIGNAL(triggered()), this, SLOT(markAsReadActionTriggered()));

    connect(QMailStore::instance(), SIGNAL(messagesUpdated(QMailMessageIdList)), this,
               SLOT(onMessagesUpdated(QMailMessageIdList)));
}

void MessageItem::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    m_pressed = true;
    style().setModePressed();
    update();
    ev->accept();
}

void MessageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
    if (m_pressed) {
        m_pressed = false;
        style().setModeDefault();
        update();
        ev->accept();
        if (ev->button() == Qt::LeftButton)
        {
            m_sender->setObjectName("MessageSenderRead");
            m_timeStamp->setObjectName("MessageTimeStampRead");
            m_subject->setObjectName("MessageSubjectRead");
            emit clicked();
        }
    } else {
        ev->ignore();
    }
}

void MessageItem::cancelEvent(MCancelEvent *ev)
{
    m_pressed = false;
    style().setModeDefault();
    update();
    ev->accept();
}

void MessageItem::deleteActionTriggered()
{

    QMailMessageIdList ids;
    ids << m_id;
    MailManager::instance()->deleteMessages(ids);
}

void MessageItem::forwardActionTriggered()
{
    EmailApplication::instance()->composeForward(m_id);
}

void MessageItem::replyActionTriggered()
{
    EmailApplication::instance()->composeReply(m_id);
}

void MessageItem::replyAllActionTriggered()
{
    EmailApplication::instance()->composeReplyAll(m_id);
}

void MessageItem::markAsReadActionTriggered()
{
    QMailMessage message(m_id);
    quint64 status(QMailMessage::Read);
    bool setFlag = true;

    if (message.status() &  QMailMessage::Read)
        setFlag = false;

    QMailStore::instance()->updateMessagesMetaData(QMailMessageKey::id(m_id), status, setFlag);
}

void MessageItem::onMessagesUpdated(const QMailMessageIdList & idList)
{
    if (idList.contains(m_id))
    {
        updateMarkAsReadUnreadAction();
    }
}

void MessageItem::updateMarkAsReadUnreadAction()
{
   // update the mark as read/unread action label
    QMailMessage message(m_id);
    quint64 status = message.status();

    if (m_markReadUnreadAction)
    {
        removeAction(m_markReadUnreadAction);
        delete m_markReadUnreadAction;
    }

    if (status & QMailMessage::Read)
    {
        //% "Mark as unread"
        m_markReadUnreadAction = new MAction(qtTrId("xx_mark_as_unread"), this);
        m_sender->setObjectName("MessageSenderRead");
        m_timeStamp->setObjectName("MessageTimeStampRead");
        m_subject->setObjectName("MessageSubjectRead");
    }
    else
    {
        //% "Mark as read"
        m_markReadUnreadAction = new MAction(qtTrId("xx_mark_as_read"), this);
        m_sender->setObjectName("MessageSender");
        m_timeStamp->setObjectName("MessageTimeStamp");
        m_subject->setObjectName("MessageSubject");
    }

    m_markReadUnreadAction->setLocation(MAction::ObjectMenuLocation);
    addAction(m_markReadUnreadAction);
    connect(m_markReadUnreadAction, SIGNAL(triggered()), this, SLOT(markAsReadActionTriggered()));

}
