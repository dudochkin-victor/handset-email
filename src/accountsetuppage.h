/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef ACCOUNTSETUPPAGE_H
#define ACCOUNTSETUPPAGE_H

#include <MApplicationPage>
#include <MLabel>
#include <MTextEdit>

#include "accountinfo.h"

class AccountSetupPage : public MApplicationPage
{
    Q_OBJECT

public:
    AccountSetupPage (QGraphicsItem *parent = NULL);
    virtual ~AccountSetupPage();
    virtual void createContent();

signals:
    void verifyAccount(AccountInfo *);
    void manualAccountEdit(AccountInfo *);
    void emailSettingsTriggered();

private slots:
    void setupAccount();

private:
    AccountInfo *m_account;

    MTextEdit  *m_username;
    MTextEdit  *m_emailAddress;
    MTextEdit  *m_password;
};

#endif // ACCOUNTSETUPPAGE_H
