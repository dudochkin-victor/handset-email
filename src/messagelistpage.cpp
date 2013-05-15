/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <MAction>
#include <MButton>
#include <MLabel>
#include <MLayout>
#include <MList>
#include <MLinearLayoutPolicy>

#include <qmailstore.h>
#include <qmailfolder.h>
#include <qmailaccountkey.h>

#include "emailutils.h"
#include "messagelistpage.h"
#include "messageitemcreator.h"
#include "messageitemmodel.h"
#include "mailmanager.h"

MessageListPage::MessageListPage()
    : m_folderId (0), m_model(0)
{
}

MessageListPage::MessageListPage(QMailFolderId folderId)
    : m_folderId (folderId), m_model(0)
{
}

MessageListPage::~MessageListPage()
{
}

void MessageListPage::createContent()
{
    MApplicationPage::createContent();
    setContentsMargins(0, 0, 0, 0);
    setPannable(true);

    createActions();
    //% "Mail"
    setTitle(qtTrId("xx_page_title"));

    MLayout *layout = new MLayout(centralWidget());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAnimation(0);

    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    policy->setObjectName("VerticalPolicy");
    policy->setContentsMargins(0, 0, 0, 0);
    policy->setSpacing(1);

    // Get this folder
    QMailFolder folder = QMailStore::instance()->folder(m_folderId);

    QMailMessageSortKey descendingDateKey(QMailMessageSortKey::receptionTimeStamp(Qt::DescendingOrder));

    QMailMessageKey mKey = QMailMessageKey::parentFolderId(m_folderId);

    // Exclude removed messages form the mkey
    mKey &= QMailMessageKey::status(QMailMessage::Removed | QMailMessage::Outgoing, 
                QMailDataComparator::Excludes);

    QMailMessageIdList ids = QMailStore::instance()->queryMessages(mKey, descendingDateKey);


    MList *messages = new MList;
    messages->setViewType("fast");
    MessageItemCreator *messageItemCreator = new MessageItemCreator();
    messages->setCellCreator(messageItemCreator);

    m_model = new MessageItemModel(ids);
    messages->setItemModel(m_model);
    messages->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect (QMailStore::instance(), SIGNAL(messagesAdded(QMailMessageIdList)), this,
                SLOT(addMessages(QMailMessageIdList)));
    connect (MailManager::instance(), SIGNAL(messagesDeleted(QMailMessageIdList)), this,
                SLOT(deleteMessages(QMailMessageIdList)));

    connect (MailManager::instance(), SIGNAL(retrievalCompleted()), this,
                SLOT(update()));
                 
    connect (messages, SIGNAL(itemClicked(const QModelIndex &)), this,
                SLOT(messageItemClicked(const QModelIndex &)));

    policy->addItem(messages);
    if (folder.serverCount() > MailManager::instance()->folderTotalCount(m_folderId))
    {
        //% "Get more messages"
        MButton *button = new MButton(qtTrId("xx_get_more_messages"));
        policy->addItem(button);
        connect(button, SIGNAL(clicked(bool)), this, SLOT(getMoreMessages()));
    }
    layout->setPolicy(policy);
    centralWidget()->setLayout(layout);
}

void MessageListPage::createActions()
{
    //% "Write new mail"
    MAction *action = new MAction(qtTrId("xx_write_new_mail"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(composeActionTriggered()));

    //% "Refresh mail"
    action = new MAction(qtTrId("xx_refresh_mail"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(refreshActionTriggered()));

}

void MessageListPage::composeActionTriggered()
{
    emit composeMail ();
}

void MessageListPage::refreshActionTriggered()
{
    MailManager::instance()->accountsSync();
}

void MessageListPage::getMoreMessages()
{
    MailManager::instance()->retrieveMoreMessages(m_folderId);
}

void MessageListPage::messageItemClicked(const QModelIndex &index)
{
    QMailMessageId id = index.data(Qt::DisplayRole).value<QMailMessageId>();
    QMailMessage message(id);
    quint64 status(QMailMessage::Read);
    QMailStore::instance()->updateMessagesMetaData(QMailMessageKey::id(id), status, true);
    emit showMessageBody (id);

    m_currentIndex = index;
}

void MessageListPage::deleteMessages(const QMailMessageIdList &ids)
{
    foreach (QMailMessageId id, ids)
        m_model->removeMessage (id);
}

void MessageListPage::addMessages(const QMailMessageIdList &ids)
{
    foreach (QMailMessageId id, ids)
    {
        QMailMessage message (id);
        QMailFolderId folderId = message.parentFolderId();
        if (folderId == m_folderId)
            m_model->addMessage (id);
    }
}

void MessageListPage::update()
{
    QMailMessageSortKey descendingDateKey(QMailMessageSortKey::receptionTimeStamp(Qt::DescendingOrder));

    QMailMessageKey mKey = QMailMessageKey::parentFolderId(m_folderId);

    // Exclude removed messages form the mkey
    mKey &= QMailMessageKey::status(QMailMessage::Removed | QMailMessage::Outgoing,
                QMailDataComparator::Excludes);

    QMailMessageIdList ids = QMailStore::instance()->queryMessages(mKey, descendingDateKey);

    m_model->update (ids);
}
