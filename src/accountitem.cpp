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
#include <MWidget>

#include <qmailstore.h>

#include "accountitem.h"

AccountItem::AccountItem (const QMailAccountId &id) :
    m_id (id)
{
    // create a new layout attached to this widget
    this->setViewType("background");
    this->setObjectName("EmailFolderItemRow");
    MLayout *layout = new MLayout(this);
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    policy->setContentsMargins(0, 0, 0, 0);

    MLabel *label = new MLabel();

    label->setText(QMailStore::instance()->account(id).name());
    label->setObjectName("EMailFolderAccountItem");
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    policy->addItem (label);
}

AccountItem::~AccountItem()
{
}

void AccountItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void AccountItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    emit clicked(m_id);
}
