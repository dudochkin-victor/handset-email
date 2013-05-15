/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef REVIEWACCOUNTPAGE_H
#define REVIEWACCOUNTPAGE_H

#include <MApplicationPage>
#include <MLabel>
#include <MTextEdit>
#include <MWidget>
#include <MLayout>
#include <MLinearLayoutPolicy>

#include "accountinfo.h"

class ReviewAccountPage : public MApplicationPage
{
    Q_OBJECT

public:
    ReviewAccountPage (AccountInfo *);
    virtual ~ReviewAccountPage();
    virtual void createContent();

signals:
    void accountReady();
    void manualAccountEdit(AccountInfo *);

private slots:
    void saveAndContinue();
    void manualEdit();

private:
    AccountInfo *m_account;
    MLinearLayoutPolicy *m_policy;

    MTextEdit *m_inServerType;
    MTextEdit *m_inServerAddress;
    MTextEdit *m_inServerPort;
    MTextEdit *m_inServerSecurity;
    MTextEdit *m_inServerUsername;
    MTextEdit *m_inServerPassword;

    MTextEdit *m_outServerType;
    MTextEdit *m_outServerAddress;
    MTextEdit *m_outServerPort;
    MTextEdit *m_outServerSecurity;
    MTextEdit *m_outServerUsername;
    MTextEdit *m_outServerPassword;
};

#endif // REVIEWACCOUNTPAGE_H
