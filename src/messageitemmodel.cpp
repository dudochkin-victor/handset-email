/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "messageitemmodel.h"

MessageItemModel::MessageItemModel(const QMailMessageIdList &ids)
{

    m_messageIds = ids;
}

int MessageItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_messageIds.size();
}

QVariant MessageItemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.isValid() && index.row() < m_messageIds.size()) {
            return QVariant::fromValue<QMailMessageId>(m_messageIds.at(index.row()));
        }
    }
    return QVariant();
}

void MessageItemModel::addMessage(const QMailMessageId &id)
{

    if (m_messageIds.contains(id))
        return;

    int i;
    for (i = m_messageIds.size(); i > 0; i--)
    {
        if (QMailMessage(id).receivedDate() < QMailMessage(m_messageIds[i - 1]).receivedDate())
        {
            break;
        }
    }
    beginInsertRows(QModelIndex(), i, i);
    m_messageIds.insert(i, id);
    endInsertRows();
}

void MessageItemModel::removeMessage(const QMailMessageId &id)
{
    for (int i = 0; i < m_messageIds.size(); i++)
    {
        if (m_messageIds[i] == id)
        {
            beginRemoveRows(QModelIndex(), i, i);
            m_messageIds.removeAt(i);
            endRemoveRows();
            break;
        }
    }
}

void MessageItemModel::update(const QMailMessageIdList &ids)
{
    if (m_messageIds.empty())
        return;

    QMailMessageId lastId = m_messageIds.last();
    int index = 0;
    if (lastId.isValid())
    {
        index = ids.indexOf(lastId);
        index++;
    }

    beginInsertRows(QModelIndex(), index, ids.size()- index);
    for (int i = index+1; i < ids.size(); i++)
    {
        m_messageIds.append(ids[i]);
    }
    endInsertRows();
}
