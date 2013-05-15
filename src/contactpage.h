/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef CONTACTPAGE_H
#define CONTACTPAGE_H

#include <MApplicationPage>

class ContactPage : public MApplicationPage
{
    Q_OBJECT

public:
    ContactPage();
    virtual ~ContactPage();

signals:
    void emailsSelected(const QStringList &);

private slots:
    void emailsClicked(const QStringList &);

private:
};

#endif // CONTACTPAGE_H
