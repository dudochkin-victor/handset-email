/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <qmailfolder.h>
#include <qmailstore.h>

#include "messageitemcreator.h"

MessageItemCreator::MessageItemCreator()
{
}

void MessageItemCreator::updateCell(const QModelIndex &index, MWidget *cell) const
{
    if (!index.isValid() || !cell)
        return;
    MessageItem *messageItem = qobject_cast<MessageItem *>(cell);

    QVariant data = index.data(Qt::DisplayRole);
    QMailMessageId id = data.value<QMailMessageId>();

    messageItem->createContent(id);
}
