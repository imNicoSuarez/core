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

#include <QSharedPointer>
#include "AXRLoggerManager.h"
#include "HSSDisplayObject.h"
#include "HSSMultipleValue.h"
#include "HSSParserNode.h"
#include "HSSSimpleSelection.h"

using namespace AXR;

HSSMultipleValue::HSSMultipleValue(AXRController * controller)
: HSSObject(HSSObjectTypeMultipleValue, controller)
{
    axr_log(LoggerChannelGeneralSpecific, "HSSMultipleValue: creating multiple value object");
}

HSSMultipleValue::HSSMultipleValue(const HSSMultipleValue & orig)
: HSSObject(orig)
{
    Q_FOREACH(QSharedPointer<HSSObject> obj, orig.valueList)
    {
        this->valueList.push_back(obj->clone());
    }
}

QSharedPointer<HSSMultipleValue> HSSMultipleValue::clone() const
{
    return qSharedPointerCast<HSSMultipleValue> (this->cloneImpl());
}

QSharedPointer<HSSClonable> HSSMultipleValue::cloneImpl() const
{
    return QSharedPointer<HSSMultipleValue>(new HSSMultipleValue(*this));
}

HSSMultipleValue::~HSSMultipleValue()
{
    axr_log(LoggerChannelGeneralSpecific, "HSSMultipleValue: destructing multiple value object");
    this->cleanTrackedObservers();
    this->valueList.clear();
}

AXRString HSSMultipleValue::toString()
{
    AXRString tempstr = AXRString("HSSMultipleValue");

    const size_t vcount = this->size();
    if (vcount > 0)
    {
        tempstr.append(" with the following values: \n");
        for (size_t j = 0; j < vcount; ++j)
        {
            tempstr.append("   ").append(this->valueList[j]->toString()).append("\n");
        }
    }

    return tempstr;
}

bool HSSMultipleValue::equalTo(QSharedPointer<HSSObject> otherObj)
{
    //check wether pointers are the same
    if (this == otherObj.data()) return true;
    //other checks
    if ( ! HSSObject::equalTo(otherObj)) return false;
    QSharedPointer<HSSMultipleValue> castedObj = qSharedPointerCast<HSSMultipleValue>(otherObj);
    if ( this->valueList.size() != castedObj->valueList.size() ) return false;
    QListIterator<QSharedPointer<HSSObject> > it1(this->valueList);
    QListIterator<QSharedPointer<HSSObject> > it2(castedObj->valueList);
    while (it1.hasNext() && it2.hasNext())
    {
        if ( ! it1.next()->equalTo(it2.next())) return false;

    }
    return true;
}

void HSSMultipleValue::add(QSharedPointer<HSSObject> newValue)
{
    if (newValue)
    {
        this->valueList.push_back(newValue);
    }
}

void HSSMultipleValue::removeLast()
{
    this->valueList.pop_back();
}

QSharedPointer<HSSObject> HSSMultipleValue::first()
{
    return this->valueList.front();
}

QSharedPointer<HSSObject> HSSMultipleValue::last()
{
    return this->valueList.back();
}

size_t HSSMultipleValue::size() const
{
    return this->valueList.size();
}

const QList<QSharedPointer<HSSObject> > HSSMultipleValue::getValues() const
{
    return this->valueList;
}

void HSSMultipleValue::commitStackValues()
{
    QListIterator<QSharedPointer<HSSObject> > it(this->valueList);
    while (it.hasNext())
    {
        QSharedPointer<HSSObject> item = it.next();
        item->commitStackValues();
    }
}

void HSSMultipleValue::fillWithDefaults()
{
    QListIterator<QSharedPointer<HSSObject> > it(this->valueList);
    while (it.hasNext())
    {
        QSharedPointer<HSSObject> item = it.next();
        item->fillWithDefaults();
    }
}

void HSSMultipleValue::setThisObj(QSharedPointer<HSSDisplayObject> value)
{
    QListIterator<QSharedPointer<HSSObject> > it(this->valueList);
    while (it.hasNext())
    {
        QSharedPointer<HSSObject> item = it.next();
        item->setThisObj(value);
    }
}

void HSSMultipleValue::setScope(QSharedPointer<HSSSimpleSelection> newScope)
{
    QListIterator<QSharedPointer<HSSObject> > it(this->valueList);
    while (it.hasNext())
    {
        QSharedPointer<HSSObject> item = it.next();
        item->setScope(newScope);
    }
}
