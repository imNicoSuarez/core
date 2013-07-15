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

#include <cmath>
#include <QtGlobal>
#include "AXRController.h"
#include "AXRDocument.h"
#include "AXRLoggerManager.h"
#include "AXRWarning.h"
#include "HSSAbstractVisitor.h"
#include "HSSBorder.h"
#include "HSSCallback.h"
#include "HSSContainer.h"
#include "HSSExpression.h"
#include "HSSFunction.h"
#include "HSSLayoutLine.h"
#include "HSSKeywordConstant.h"
#include "HSSNumberConstant.h"
#include "HSSObjectDefinition.h"
#include "HSSObjectNameConstant.h"
#include "HSSPercentageConstant.h"
#include "HSSRectangle.h"
#include "HSSRoundedRect.h"
#include "HSSStringConstant.h"
#include "HSSTextBlock.h"
#include "HSSValue.h"

using namespace AXR;

QSharedPointer<HSSDisplayObject> HSSContainer::asDisplayObject(QSharedPointer<HSSContainer> theContainer)
{
    return qSharedPointerCast<HSSDisplayObject > (theContainer);
}

//always test the return of this function. E.g. "QSharedPointer<HSSContainer> cont = HSSContainer::asContainer(myDO); if(cont) { /* ... */ }

QSharedPointer<HSSContainer> HSSContainer::asContainer(QSharedPointer<HSSDisplayObject> theDisplayObject)
{
    QSharedPointer<HSSContainer> ret;
    if (theDisplayObject->isA(HSSObjectTypeContainer))
    {
        ret = qSharedPointerCast<HSSContainer > (theDisplayObject);
    }
    return ret;
}

HSSContainer::HSSContainer(AXRController * controller)
: HSSDisplayObject(HSSObjectTypeContainer, controller)
{
    axr_log(LoggerChannelGeneralSpecific, "HSSContainer: creating container");
    this->_initialize();
    std::vector<AXRString> shorthandProperties;
    shorthandProperties.push_back("isA");
    shorthandProperties.push_back("width");
    shorthandProperties.push_back("height");
    shorthandProperties.push_back("contentAlignX");
    shorthandProperties.push_back("contentAlignY");
    shorthandProperties.push_back("font");
    shorthandProperties.push_back("background");
    shorthandProperties.push_back("shape");
    shorthandProperties.push_back("textAlign");
    this->setShorthandProperties(shorthandProperties);
}

void HSSContainer::_initialize()
{
    this->distributeX = false;
    this->distributeXLinear = false;
    this->distributeY = false;
    this->distributeYLinear = false;

    this->children = QSharedPointer<HSSSimpleSelection>(new HSSSimpleSelection());
    this->allChildren = QSharedPointer<HSSSimpleSelection>(new HSSSimpleSelection());

    this->addCallback("contentAlignX", new HSSComputeCallback<HSSContainer>(this, &HSSContainer::computeContentAlignX));
    this->addListenCallback("contentAlignX", new HSSObserveCallback<HSSContainer>(this, &HSSContainer::listenContentAlignX));
    this->addCallback("contentAlignY", new HSSComputeCallback<HSSContainer>(this, &HSSContainer::computeContentAlignY));
    this->addListenCallback("contentAlignY", new HSSObserveCallback<HSSContainer>(this, &HSSContainer::listenContentAlignY));
    this->addCallback("content", new HSSComputeCallback<HSSContainer>(this, &HSSContainer::computeContent));
    this->addCallback("shape", new HSSComputeCallback<HSSContainer>(this, &HSSContainer::computeShape), new HSSObserveCallback<HSSContainer>(this, &HSSContainer::listenShape), new HSSObserveCallback<HSSContainer>(this, &HSSContainer::notifyShape));
}

HSSContainer::HSSContainer(const HSSContainer & orig)
: HSSDisplayObject(orig)
{
    this->_initialize();
}

QSharedPointer<HSSContainer> HSSContainer::clone() const
{
    axr_log(LoggerChannelGeneralSpecific, "HSSContainer: cloning container");
    return qSharedPointerCast<HSSContainer> (this->cloneImpl());
}

QSharedPointer<HSSClonable> HSSContainer::cloneImpl() const
{
    return QSharedPointer<HSSContainer>(new HSSContainer(*this));
}

HSSContainer::~HSSContainer()
{
    axr_log(LoggerChannelGeneralSpecific, "HSSContainer: destructing container");
    if (this->children)
    {
        this->children->clear();
    }
    if (this->allChildren){
        this->allChildren->clear();
    }
}

void HSSContainer::setDefaults()
{
    HSSDisplayObject::setDefaults();
    this->setDefaultKw("contentAlignX", "left");
    this->setDefaultKw("contentAlignY", "top");
    this->setDefaultKw("direction", "ltr");
    this->setDefaultKw("wrapDirection", "ttb");
    this->setDefaultKw("shape", "default");
}

AXRString HSSContainer::toString()
{
    AXRString tempstr;

    if (this->isNamed())
    {
        tempstr = AXRString("HSSContainer: ").append(this->name);
    }
    else
    {
        tempstr = "Anonymous HSSContainer";
    }

    if (this->attributes.size() > 0)
    {
        tempstr.append("\n").append("with the following attributes:");
        for (std::map<AXRString, AXRString>::iterator it = this->attributes.begin(); it != this->attributes.end(); ++it)
        {
            tempstr.append("\n").append("- ").append((*it).first).append(": ").append((*it).second);
        }
        tempstr.append("\n");
    }

    if (this->children->size() > 0)
    {
        tempstr.append("\n").append("with the following children objects:");
        for (HSSSimpleSelection::iterator it = this->children->begin(); it!= this->children->end(); ++it)
        {
            tempstr.append("\n").append((*it)->toString());
        }
        tempstr.append("\n");
    }

    return tempstr;
}

QVariantMap HSSContainer::toMap() const
{
    QVariantMap map = HSSObject::toMap();

    if (this->children->size() > 0)
    {
        QList<QVariant> childrenList;

        for (HSSSimpleSelection::iterator it = this->children->begin(); it!= this->children->end(); ++it)
        {
            childrenList.append((*it)->toMap());
        }

        map.insert("children", QVariant(childrenList));
    }
    else
    {
        map.insert("children", QVariant(false));
    }

    return map;
}

AXRString HSSContainer::defaultObjectType()
{
    return "container";
}

AXRString HSSContainer::defaultObjectType(AXRString property)
{
    if (property == "shape")
    {
        return "roundedRect";
    }
    else
    {
        return HSSDisplayObject::defaultObjectType(property);
    }
}

bool HSSContainer::isKeyword(AXRString value, AXRString property)
{
    if (property == "contentAlignX" || property == "contentAlignY")
    {
        if (value == "center"
                || value == "middle"
                || value == "top"
                || value == "bottom"
                || value == "left"
                || value == "right"
                || value == "even"
                || value == "justify")
        {
            return true;
        }
    }
    else if (property == "direction" || property == "wrapDirection")
    {
        if (value == "ltr"
                || value == "rtl"
                || value == "ttb"
                || value == "btt")
        {
            return true;
        }
    }
    else if (property == "textAlign")
    {
        if (value == "left"
                || value == "right"
                || value == "center"
                || value == "justify")
        {
            return true;
        }
    }

    //if we reached this far, let the superclass handle it
    return HSSDisplayObject::isKeyword(value, property);
}

//children
void HSSContainer::add(QSharedPointer<HSSDisplayObject> child)
{
    QSharedPointer<HSSContainer> sharedThis = this->shared_from_this();
    child->setParent(sharedThis);
    axr_log(LoggerChannelGeneralSpecific, "HSSContainer: added child " + child->getElementName() + " to " + this->getElementName());
    child->setIndex(this->allChildren->size());
    if (!child->isA(HSSObjectTypeTextBlock))
    {
        this->children->add(child);
    }
    this->allChildren->add(child);
    this->notifyObservers("treeChange", sharedThis);
}

void HSSContainer::remove(size_t index)
{
    HSSSimpleSelection::iterator it = this->children->begin();
    while (it != this->children->end())
    {
        QSharedPointer<HSSDisplayObject> child = *it;
        if (child->getIndex() == index)
        {
            it = this->children->erase(it);
        }
        else
        {
            ++it;
        }
    }

    HSSSimpleSelection::iterator it2 = this->allChildren->begin();
    while (it2 != this->allChildren->end())
    {
        QSharedPointer<HSSDisplayObject> child = *it2;
        if (child->getIndex() == index)
        {
            it2 = this->allChildren->erase(it2);
        }
        else
        {
            ++it2;
        }
    }

    this->resetChildrenIndexes();
    this->notifyObservers("treeChange", this->shared_from_this());
}

void HSSContainer::clear()
{
    this->children->clear();
    this->allChildren->clear();
}

void HSSContainer::resetChildrenIndexes()
{
    unsigned i = 0;
    for (HSSSimpleSelection::iterator it = this->allChildren->begin(); it!= this->allChildren->end(); ++it)
    {
        (*it)->setIndex(i);
        ++i;
    }
}

void HSSContainer::setContentText(const AXRString &contentText)
{
    AXRString text = contentText.trimmed();

    AXRController * controller = this->getController();

    if (!text.isEmpty())
    {
        if (this->allChildren->empty())
        {
            QSharedPointer<HSSTextBlock> txtBlck = QSharedPointer<HSSTextBlock>(new HSSTextBlock(controller));
            txtBlck->setText(text);
            this->add(txtBlck);
        }
        else
        {
            QSharedPointer<HSSDisplayObject> lastChild = this->allChildren->back();
            if (lastChild->isA(HSSObjectTypeTextBlock))
            {
                QSharedPointer<HSSTextBlock> textBlock = qSharedPointerCast<HSSTextBlock > (lastChild);
                textBlock->setText(text);
            }
            else
            {
                QSharedPointer<HSSTextBlock> txtBlck = QSharedPointer<HSSTextBlock>(new HSSTextBlock(controller));
                txtBlck->setText(text);
                this->add(txtBlck);
            }
        }
    }
}

void HSSContainer::appendContentText(const AXRString &contentText)
{
    AXRString text = contentText.trimmed();

    AXRController * controller = this->getController();

    if (!text.isEmpty())
    {
        if (this->allChildren->empty())
        {
            QSharedPointer<HSSTextBlock> txtBlck = QSharedPointer<HSSTextBlock>(new HSSTextBlock(controller));
            txtBlck->setText(text);
            this->add(txtBlck);
        }
        else
        {
            QSharedPointer<HSSDisplayObject> lastChild = this->allChildren->back();
            if (lastChild->isA(HSSObjectTypeTextBlock))
            {
                QSharedPointer<HSSTextBlock> textBlock = qSharedPointerCast<HSSTextBlock > (lastChild);
                text = textBlock->getText() + " " + text;
                textBlock->setText(text);
            }
            else
            {
                QSharedPointer<HSSTextBlock> txtBlck = QSharedPointer<HSSTextBlock>(new HSSTextBlock(controller));
                txtBlck->setText(text);
                this->add(txtBlck);
            }
        }
        this->setComputedValue("content", text);
    }
}

void HSSContainer::accept(HSSAbstractVisitor* visitor, HSSVisitorFilterFlags filterFlags)
{
    if ((filterFlags & HSSVisitorFilterLayout))
    {
        for (HSSSimpleSelection::iterator child = this->allChildren->begin(); child != this->allChildren->end(); ++child)
        {
            (*child)->accept(visitor, filterFlags);
        }
    }

    visitor->visit(*this);

    if (!(filterFlags & HSSVisitorFilterTraverse))
        return;

    for (HSSSimpleSelection::iterator child = this->allChildren->begin(); child != this->allChildren->end(); ++child)
    {
        (*child)->accept(visitor, filterFlags);
    }
}

void HSSContainer::recursiveResetLayout()
{
    for (HSSSimpleSelection::const_iterator it = this->allChildren->begin(); it!= this->allChildren->end(); ++it)
    {
        (*it)->recursiveResetLayout();
    }

    this->setNeedsLayout(true);
}

void HSSContainer::setGlobalX(HSSUnit newValue)
{
    HSSUnit delta = newValue - this->globalX;
    HSSDisplayObject::setGlobalX(newValue);

    for (HSSSimpleSelection::iterator it = this->allChildren->begin(); it!= this->allChildren->end(); ++it)
    {
        QSharedPointer<HSSDisplayObject> theChild = *it;
        theChild->setGlobalX(theChild->globalX + delta);
    }
}

void HSSContainer::setGlobalY(HSSUnit newValue)
{
    HSSUnit delta = newValue - this->globalY;
    HSSDisplayObject::setGlobalY(newValue);

    for (HSSSimpleSelection::iterator it = this->allChildren->begin(); it!= this->allChildren->end(); ++it)
    {
        QSharedPointer<HSSDisplayObject> theChild = *it;
        theChild->setGlobalY(theChild->globalY + delta);
    }
}

void HSSContainer::setChildren(QSharedPointer<HSSSimpleSelection> newChildren)
{
    this->children = newChildren;
    this->allChildren = newChildren;

    for (HSSSimpleSelection::iterator it = this->allChildren->begin(); it!= this->allChildren->end(); ++it)
    {
        (*it)->setParent(this->shared_from_this());
    }
}

QSharedPointer<HSSSimpleSelection> HSSContainer::getChildren() const
{
    return this->getChildren(false);
}

QSharedPointer<HSSSimpleSelection> HSSContainer::getChildren(bool includeTextBlocks) const
{
    if (includeTextBlocks)
    {
        return this->allChildren;
    }
    else
    {
        return this->children;
    }
}

bool HSSContainer::handleEvent(HSSInputEvent *event)
{
    bool handled = false;
    for (HSSSimpleSelection::iterator it = this->allChildren->begin(); it < this->allChildren->end(); ++it)
    {
        QSharedPointer<HSSDisplayObject> child = *it;
        bool childHandled = child->handleEvent(event);
        if (childHandled)
        {
            handled = true;
        }
    }

    bool superHandled = HSSDisplayObject::handleEvent(event);
    if (superHandled)
    {
        handled = true;
    }

    return handled;
}

void HSSContainer::setController(AXRController * controller)
{
    //propagate
    for (HSSSimpleSelection::iterator it = this->allChildren->begin(); it < this->allChildren->end(); ++it)
    {
        QSharedPointer<HSSDisplayObject> child = *it;
        child->setController(controller);
    }

    HSSDisplayObject::setController(controller);
}

QSharedPointer<HSSContainer> HSSContainer::shared_from_this()
{
    return qSharedPointerCast<HSSContainer > (HSSDisplayObject::shared_from_this());
}

//contentAlignX
const HSSUnit HSSContainer::getContentAlignX() const
{
    return this->getComputedNumber("contentAlignX");
}

QSharedPointer<HSSObject> HSSContainer::computeContentAlignX(QSharedPointer<HSSParserNode> parserNode)
{
    switch (parserNode->getType())
    {
        case HSSParserNodeTypeKeywordConstant:
        {
            QSharedPointer<HSSKeywordConstant> theKW = qSharedPointerCast<HSSKeywordConstant>(parserNode);
            AXRString kwValue = theKW->getValue();
            if (kwValue == "left")
            {
                return this->computeValueObject(this->percentageToConstant(0));
            }
            else if (kwValue == "middle" || kwValue == "center")
            {
                return this->computeValueObject(this->percentageToConstant(50));
            }
            else if (kwValue == "right")
            {
                return this->computeValueObject(this->percentageToConstant(100));
            }
            else if (kwValue == "even")
            {
                this->distributeX = true;
                this->distributeXLinear = false;
            }
            else if (kwValue == "justify")
            {
                this->distributeX = true;
                this->distributeXLinear = true;
            }
            break;
        }

        default:
            break;
    }
    return this->computeValueObject(parserNode, "contentAlignX");
}

void HSSContainer::listenContentAlignX(QSharedPointer<HSSObject> theObj)
{
    if (theObj->isA(HSSObjectTypeValue))
    {
        QSharedPointer<HSSContainer> parent = this->getParent();
        if (parent)
        {
            QSharedPointer<HSSValue> valueObj = qSharedPointerCast<HSSValue>(theObj);
            valueObj->listen(parent, "innerWidth");
            valueObj->setPercentageBase(this->getInnerWidth());
            valueObj->observe("value", "contentAlignX", this, new HSSValueChangedCallback<HSSDisplayObject>(this, &HSSDisplayObject::alignXChanged));
        }
    }
}

//contentAlignY
const HSSUnit HSSContainer::getContentAlignY() const
{
    return this->getComputedNumber("contentAlignY");
}

QSharedPointer<HSSObject> HSSContainer::computeContentAlignY(QSharedPointer<HSSParserNode> parserNode)
{
    switch (parserNode->getType())
    {
        case HSSParserNodeTypeKeywordConstant:
        {
            QSharedPointer<HSSKeywordConstant> theKW = qSharedPointerCast<HSSKeywordConstant>(parserNode);
            AXRString kwValue = theKW->getValue();
            if (kwValue == "top")
            {
                return this->computeValueObject(this->percentageToConstant(0));
            }
            else if (kwValue == "middle" || kwValue == "center")
            {
                return this->computeValueObject(this->percentageToConstant(50));
            }
            else if (kwValue == "bottom")
            {
                return this->computeValueObject(this->percentageToConstant(100));
            }
            else if (kwValue == "even")
            {
                this->distributeY = true;
                this->distributeYLinear = false;
            }
            else if (kwValue == "justify")
            {
                this->distributeY = true;
                this->distributeYLinear = true;
            }
            break;
        }

        default:
            break;
    }
    return this->computeValueObject(parserNode, "contentAlignY");
}

void HSSContainer::listenContentAlignY(QSharedPointer<HSSObject> theObj)
{
    if (theObj->isA(HSSObjectTypeValue))
    {
        QSharedPointer<HSSContainer> parent = this->getParent();
        if (parent)
        {
            QSharedPointer<HSSValue> valueObj = qSharedPointerCast<HSSValue>(theObj);
            valueObj->listen(parent, "innerWidth");
            valueObj->setPercentageBase(this->getInnerWidth());
            valueObj->observe("value", "contentAlignY", this, new HSSValueChangedCallback<HSSDisplayObject>(this, &HSSDisplayObject::alignXChanged));
        }
    }
}

//direction
const HSSDirectionValue HSSContainer::getDirection() const
{
    QSharedPointer<HSSObject> value = this->getComputedValue("direction");
    if (value && value->isA(HSSObjectTypeValue))
    {
        QSharedPointer<HSSParserNode> parserNode = qSharedPointerCast<HSSValue>(value)->getValue();
        if (parserNode && parserNode->isA(HSSParserNodeTypeKeywordConstant))
        {
            AXRString kwValue = qSharedPointerCast<HSSKeywordConstant>(parserNode)->getValue();
            if (kwValue == "ltr")
            {
                return HSSDirectionLeftToRight;
            }
            if (kwValue == "rtl")
            {
                return HSSDirectionRightToLeft;
            }
            if (kwValue == "ttb")
            {
                return HSSDirectionTopToBottom;
            }
            if (kwValue == "btt")
            {
                return HSSDirectionBottomToTop;
            }
        }
    }
    return HSSDirectionLeftToRight;
}

//wrapDirection
const HSSDirectionValue HSSContainer::getWrapDirection() const
{
    QSharedPointer<HSSObject> value = this->getComputedValue("wrapDirection");
    if (value && value->isA(HSSObjectTypeValue))
    {
        QSharedPointer<HSSParserNode> parserNode = qSharedPointerCast<HSSValue>(value)->getValue();
        if (parserNode && parserNode->isA(HSSParserNodeTypeKeywordConstant))
        {
            AXRString kwValue = qSharedPointerCast<HSSKeywordConstant>(parserNode)->getValue();
            if (kwValue == "ltr")
            {
                return HSSDirectionLeftToRight;
            }
            if (kwValue == "rtl")
            {
                return HSSDirectionRightToLeft;
            }
            if (kwValue == "ttb")
            {
                return HSSDirectionTopToBottom;
            }
            if (kwValue == "btt")
            {
                return HSSDirectionBottomToTop;
            }
        }
    }
    return HSSDirectionLeftToRight;
}

//content
const AXRString HSSContainer::getContent() const
{
    return this->getComputedString("content");
}
QSharedPointer<HSSObject> HSSContainer::computeContent(QSharedPointer<HSSParserNode> parserNode)
{
    if (parserNode->isA(HSSParserNodeTypeStringConstant))
    {
        AXRString text = qSharedPointerCast<HSSStringConstant>(parserNode)->getValue();
        this->setContentText(text);
    }
    return this->computeValueObject(parserNode, "content");
}

//shape
QSharedPointer<HSSShape> HSSContainer::getShape()
{
    return qSharedPointerCast<HSSShape>(this->getComputedValue("shape"));
}

QSharedPointer<HSSObject> HSSContainer::computeShape(QSharedPointer<HSSParserNode> parserNode)
{
    switch (parserNode->getType())
    {
        case HSSParserNodeTypeKeywordConstant:
        {
            QSharedPointer<HSSKeywordConstant> theKW = qSharedPointerCast<HSSKeywordConstant>(parserNode);
            AXRString kwValue = theKW->getValue();
            if (kwValue == "default")
            {
                return QSharedPointer<HSSRectangle>(new HSSRectangle(this->getController()));
            }
            break;
        }

        default:
            break;
    }

    return this->computeValueObject(parserNode);
}

void HSSContainer::listenShape(QSharedPointer<HSSObject> theObj)
{
    if (theObj->isA(HSSObjectTypeShape))
    {
        QSharedPointer<HSSShape> shapeObj = qSharedPointerCast<HSSShape>(theObj);
        shapeObj->observe("value", "shape", this, new HSSValueChangedCallback<HSSContainer>(this, &HSSContainer::shapeChanged));
    }
}

void HSSContainer::notifyShape(QSharedPointer<HSSObject> theObj)
{
    this->setDirty(true);
    this->notifyObservers("shape", theObj);
}

void HSSContainer::shapeChanged(const AXRString source, const QSharedPointer<HSSObject> theObj)
{
    this->setDirty(true);
    this->notifyObservers("shape", theObj);
