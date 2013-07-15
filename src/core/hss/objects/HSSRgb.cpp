/********************************************************************
 *             a  A
 *            AM\/MA
 *           (MA:MMD
 *            :: VD
 *           ::  º
 *          ::
 *         ::   **      .A$MMMMND   AMMMD     AMMM6    MMMM  MMMM6
 +       6::Z. TMMM    MMMMMMMMMDA   VMMMD   AMMM6     MMMMMMMMM6
 *      6M:AMMJMMOD     V     MMMA    VMMMD AMMM6      MMMMMMM6
 *      ::  TMMTMC         ___MMMM     VMMMMMMM6       MMMM
 *     MMM  TMMMTTM,     AMMMMMMMM      VMMMMM6        MMMM
 *    :: MM TMMTMMMD    MMMMMMMMMM       MMMMMM        MMMM
 *   ::   MMMTTMMM6    MMMMMMMMMMM      AMMMMMMD       MMMM
 *  :.     MMMMMM6    MMMM    MMMM     AMMMMMMMMD      MMMM
 *         TTMMT      MMMM    MMMM    AMMM6  MMMMD     MMMM
 *        TMMMM8       MMMMMMMMMMM   AMMM6    MMMMD    MMMM
 *       TMMMMMM$       MMMM6 MMMM  AMMM6      MMMMD   MMMM
 *      TMMM MMMM
 *     TMMM  .MMM
 *     TMM   .MMD       ARBITRARY·······XML········RENDERING
 *     TMM    MMA       ====================================
 *     TMN    MM
 *      MN    ZM
 *            MM,
 *
 *
 *      AUTHORS: see AUTHORS file
 *
 *      COPYRIGHT: ©2013 - All Rights Reserved
 *
 *      LICENSE: see LICENSE file
 *
 *      WEB: http://axrproject.org
 *
 *      THIS CODE AND INFORMATION ARE PROVIDED "AS IS"
 *      WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *      IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 *      FITNESS FOR A PARTICULAR PURPOSE.
 *
 ********************************************************************/

#include <QColor>
#include <QVariant>
#include "AXRError.h"
#include "AXRLoggerManager.h"
#include "HSSCallback.h"
#include "HSSDisplayObject.h"
#include "HSSExpression.h"
#include "HSSNumberConstant.h"
#include "HSSPercentageConstant.h"
#include "HSSRgb.h"
#include "HSSSimpleSelection.h"

using namespace AXR;

QSharedPointer<HSSRgb> HSSRgb::blackColor(AXRController * controller)
{
    //create a new color object, it will have all its color channels set to
    //0 and the alpha to 255 by default in the constructor
    return QSharedPointer<HSSRgb>(new HSSRgb(controller));
}

QSharedPointer<HSSRgb> HSSRgb::whiteColor(AXRController * controller)
{
    //create a new color object with default value
    QSharedPointer<HSSRgb> whiteColor = QSharedPointer<HSSRgb>(new HSSRgb(controller));
    //set all the color channels to the maximum value
    return whiteColor;
}

HSSRgb::HSSRgb(const HSSRgb & orig)
: HSSObject(orig)
{
    this->_initialize();

    std::vector<AXRString> shorthandProperties;
    shorthandProperties.push_back("isA");
    shorthandProperties.push_back("alpha");
    shorthandProperties.push_back("red");
    shorthandProperties.push_back("green");
    shorthandProperties.push_back("blue");

    this->setShorthandProperties(shorthandProperties);
}

HSSRgb::HSSRgb(const HSSRgb & orig)
: HSSObject(orig)
{
    this->_initialize();
}

void HSSRgb::_initialize()
{
}

QSharedPointer<HSSRgb> HSSRgb::clone() const
{
    axr_log(LoggerChannelGeneralSpecific, "HSSRgb: cloning rgb object");
    return qSharedPointerCast<HSSRgb> (this->cloneImpl());
}

QSharedPointer<HSSClonable> HSSRgb::cloneImpl() const
{
    return QSharedPointer<HSSRgb>(new HSSRgb(*this));
}

HSSRgb::~HSSRgb()
{
    axr_log(LoggerChannelGeneralSpecific, "HSSRgb: destructing rgb object");
}

AXRString HSSRgb::toString()
{
    if (this->isNamed())
    {
        return AXRString("HSSRgb: ").append(this->name);
    }
    else
    {
        return AXRString("Annonymous HSSRgb with red: %1 green: %2 blue: %3 alpha: %4").arg(this->red).arg(this->green).arg(this->blue).arg(this->alpha);
    }
}

AXRString HSSRgb::defaultObjectType()
{
    return "rgb";
}

AXRString HSSRgb::defaultObjectType(AXRString property)
{
    if (property == "red" || property == "green" || property == "blue" || property == "alpha")
    {
        return "value";
    }
    else
    {
        return HSSObject::defaultObjectType(property);
    }
}

//bool HSSRgb::isKeyword(AXRString value, AXRString property)
//{
//    if (value == "transparent"){
//        return true;
//    }
//
//    //if we reached this far, let the superclass handle it
//    return HSSObject::isKeyword(value, property);
//}

QColor HSSRgb::toQColor() const
{
    return QColor(static_cast<int>(red), static_cast<int>(green), static_cast<int>(blue), static_cast<int>(alpha));
}

QColor HSSRgb::toQColorWithAlpha(int alpha) const
{
    QColor color = toQColor();
    color.setAlpha(alpha);
    return color;
}
