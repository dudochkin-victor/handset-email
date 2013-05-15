/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MESSAGEITEMSTYLE_H
#define MESSAGEITEMSTYLE_H

#include <MWidgetStyle>
#include <MStyle>


class M_EXPORT MessageItemStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(MessageItemStyle)

};

class M_EXPORT MessageItemStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(MessageItemStyle)
    M_STYLE_MODE(Pressed)
};

#endif // MESSAGEITEMSTYLE_H
