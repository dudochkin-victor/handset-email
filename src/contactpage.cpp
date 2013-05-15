/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QGraphicsLinearLayout>

#include <MApplicationPage>

#include "contactpage.h"
#include "seasidelist.h"
#include <stdio.h>

ContactPage::ContactPage()
{

    QGraphicsLinearLayout *linear = new QGraphicsLinearLayout;
    linear->setContentsMargins(0, 0, 0, 0);
    linear->setSpacing(0);
    centralWidget()->setLayout(linear);

    SeasideList *list = new SeasideList(SeasideList::DetailEmail);
    linear->addItem(list);

    connect(list, SIGNAL(emailsSelected(QStringList)), this, SLOT(emailsClicked(QStringList)));

}

ContactPage::~ContactPage()
{
}

void ContactPage::emailsClicked(const QStringList& emails)
{
    emit emailsSelected(emails);
    dismiss();
}
