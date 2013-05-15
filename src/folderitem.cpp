/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <MLayout>
#include <MLabel>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MWidget>

#include <qmailstore.h>
#include <qmailfolder.h>

#include "folderitem.h"
#include "mailmanager.h"

FolderItem::FolderItem (QMailFolderId id) :
    m_id (id)
{
    // create a new layout attached to this widget
    this->setViewType("background");
    this->setObjectName("EmailFolderItemRow");
    MLayout *layout = new MLayout(this);
    MGridLayoutPolicy *policy = new MGridLayoutPolicy(layout);

    MLabel *label = new MLabel();

    // retrieve the name of the folder
    QMailFolder folder = QMailStore::instance()->folder(id);

    label->setText(folder.displayName());
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    label->setObjectName("EmailFolderItem");
    policy->addItem (label, 0, 0, Qt::AlignLeft);
    m_unreadTotalCount = new MLabel("");
    uint totalCount = MailManager::instance()->folderTotalCount(id);
    uint unreadCount = MailManager::instance()->folderUnreadCount(id);
    if (unreadCount != 0)
    {
        QString str;
        str = QString("%1/%2").arg(unreadCount).arg(totalCount);
        m_unreadTotalCount->setText(str);
    }
    m_unreadTotalCount->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_unreadTotalCount->setObjectName("EmailFolderMessageCount");
    policy->addItem (m_unreadTotalCount, 0, 1, Qt::AlignRight);
    
}

FolderItem::~FolderItem()
{
}

void FolderItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void FolderItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    emit clicked(m_id);
}

void FolderItem::updateMessagesCount()
{
    QString str("");
    uint totalCount = MailManager::instance()->folderTotalCount(m_id);
    uint unreadCount = MailManager::instance()->folderUnreadCount(m_id);
    
    if (unreadCount != 0)
        str = QString("%1/%2").arg(unreadCount).arg(totalCount);

    m_unreadTotalCount->setText(str);
}
