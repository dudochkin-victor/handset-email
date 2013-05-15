/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef FOLDERITEM_H
#define FOLDERITEM_H

#include <MLabel>
#include <MWidgetController>

#include <qmailfolder.h>

class FolderItem: public MWidgetController
{
    Q_OBJECT

public:
    FolderItem(QMailFolderId folderId);
    virtual ~FolderItem();

    QMailFolderId folderId() { return m_id; }
    void updateMessagesCount();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void clicked(QMailFolderId folderId);

private:
    QMailFolderId  m_id;
    MLabel *m_unreadTotalCount;
};

#endif  // FOLDERITEM_H
