/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <mabstractcellcreator.h>

#include "messageitem.h"

class MessageItemCreator : public MAbstractCellCreator<MessageItem>
{
public:
    MessageItemCreator();
    void updateCell(const QModelIndex &index, MWidget *cell) const;
};

