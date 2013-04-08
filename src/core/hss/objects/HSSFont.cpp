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

#include "AXRController.h"
#include "AXRLoggerManager.h"
#include "AXRWarning.h"
#include "HSSCallback.h"
#include "HSSDisplayObject.h"
#include "HSSExpression.h"
#include "HSSFont.h"
#include "HSSFunction.h"
#include "HSSKeywordConstant.h"
#include "HSSNumberConstant.h"
#include "HSSObjectDefinition.h"
#include "HSSObjectNameConstant.h"
#include "HSSParserNode.h"
#include "HSSPercentageConstant.h"
#include "HSSRgb.h"
#include "HSSStringConstant.h"
#include "HSSSimpleSelection.h"

using namespace AXR;

const HSSUnit HSSFont::DEFAULT_SIZE = 18;

HSSFont::HSSFont(AXRController * controller)
: HSSObject(HSSObjectTypeFont, controller)
{
    axr_log(LoggerChannelGeneralSpecific, "HSSFont: creating font object");

    this->observedSize = this->observedFace = this->observedColor = this->observedWeight = NULL;

    this->size = DEFAULT_SIZE;
    std::vector<AXRString> shorthandProperties;
    shorthandProperties.push_back("face");
    shorthandProperties.push_back("size");
    shorthandProperties.push_back("color");
    shorthandProperties.push_back("weight");
    this->setShorthandProperties(shorthandProperties);
}

HSSFont::HSSFont(const HSSFont & orig)
: HSSObject(orig)
{
    this->observedSize = this->observedFace = this->observedColor = this->observedWeight = NULL;
    this->size = DEFAULT_SIZE;
    std::vector<AXRString> shorthandProperties;
    shorthandProperties.push_back("face");
    shorthandProperties.push_back("size");
    shorthandProperties.push_back("color");
    shorthandProperties.push_back("weight");
    this->setShorthandProperties(shorthandProperties);
}

QSharedPointer<HSSFont> HSSFont::clone() const
{
    return qSharedPointerCast<HSSFont> (this->cloneImpl());
}

QSharedPointer<HSSClonable> HSSFont::cloneImpl() const
{
    return QSharedPointer<HSSFont>(new HSSFont(*this));
}

HSSFont::~HSSFont()
{

}

AXRString HSSFont::toString()
{
    if (this->isNamed())
    {
        return AXRString("HSSFont: ").append(this->name);
    }
    else
    {
        return "Annonymous HSSFont";
    }
}

AXRString HSSFont::defaultObjectType()
{
    return "font";
}

AXRString HSSFont::defaultObjectType(AXRString property)
{
    if (property == "color")
    {
        return "rgb";
    }
    else
    {
        return HSSObject::defaultObjectType(property);
    }
}

bool HSSFont::isKeyword(AXRString value, AXRString property)
{
    if (property == "weight")
    {
        if (
                value == "normal"
                || value == "bold"
                || value == "medium"
                || value == "thin"
                || value == "light"
                || value == "book"
                || value == "heavy"
                || value == "ultralight"
                || value == "semibold"
                || value == "ultrabold"
                || value == "ultraheavy"
                )
        {
            return true;
        }
    }

    //if we reached this far, let the superclass handle it
    return HSSObject::isKeyword(value, property);
}

void HSSFont::setProperty(HSSObservableProperty name, QSharedPointer<HSSParserNode> value)
{
    switch (name)
    {
    case HSSObservablePropertySize:
        this->setDSize(value);
        break;
    case HSSObservablePropertyFace:
        this->setDFace(value);
        break;
    case HSSObservablePropertyColor:
        this->setDColor(value);
        break;
    case HSSObservablePropertyWeight:
        this->setDWeight(value);
        break;

    default:
        HSSObject::setProperty(name, value);
        break;
    }
}

HSSUnit HSSFont::getSize()
{
    return this->size;
}

void HSSFont::setDSize(QSharedPointer<HSSParserNode> value)
{
    switch (value->getType())
    {
    case HSSParserNodeTypeNumberConstant:
    case HSSParserNodeTypePercentageConstant:
    case HSSParserNodeTypeExpression:
        break;
    default:
        throw AXRWarning("HSSDisplayObject", "Invalid value for size of @font object " + this->name);
    }

    this->dSize = value;
    this->size = this->_evaluatePropertyValue(
                                      &HSSFont::sizeChanged,
                                      value,
                                      DEFAULT_SIZE,
                                      HSSObservablePropertySize,
                                      this->observedSize,
                                      this->observedSizeProperty
                                      );

    this->notifyObservers(HSSObservablePropertySize, &this->size);
}

void HSSFont::sizeChanged(AXR::HSSObservableProperty source, void *data)
{
    axr_log(LoggerChannelObsolete1, "********************** sizeChanged unimplemented ****************************");
}

AXRString HSSFont::getFace()
{
    return this->face;
}

void HSSFont::setDFace(QSharedPointer<HSSParserNode> value)
{

    if (value->isA(HSSParserNodeTypeStringConstant))
    {
        this->dFace = value;
        QSharedPointer<HSSStringConstant> theString = qSharedPointerCast<HSSStringConstant > (value);
        this->face = theString->getValue();
    }
    else
    {
        throw AXRWarning("HSSDisplayObject", "Invalid value for face of @font object " + this->name);
    }
}

void HSSFont::faceChanged(AXR::HSSObservableProperty source, void *data)
{
    axr_log(LoggerChannelObsolete1, "********************** faceChanged unimplemented ****************************");
}

QSharedPointer<HSSRgb> HSSFont::getColor()
{
    return this->color;
}

QSharedPointer<HSSParserNode> HSSFont::getDColor()
{
    return this->dColor;
}

void HSSFont::setDColor(QSharedPointer<HSSParserNode> value)
{
    bool valid = false;

    switch (value->getType())
    {
    case HSSParserNodeTypeObjectNameConstant:
    {
        this->dColor = value;
        try
        {
            QSharedPointer<HSSObjectNameConstant> objname = qSharedPointerCast<HSSObjectNameConstant > (value);
            QSharedPointer<HSSObjectDefinition> objdef = this->getController()->objectTreeNodeNamed(objname->getValue());
            this->setDColor(objdef);
            valid = true;

        }
        catch (const AXRError &e)
        {
            e.raise();
        }

        break;
    }


    case HSSParserNodeTypeFunctionCall:
    {
        this->dColor = value;
        QSharedPointer<HSSFunction> fnct = qSharedPointerCast<HSSFunction > (value)->clone();
        if (fnct && fnct->isA(HSSFunctionTypeRef))
        {
            fnct->setScope(this->scope);
            fnct->setThisObj(this->getThisObj());

            QVariant remoteValue = fnct->evaluate();
            if (remoteValue.canConvert<QSharedPointer<HSSRgb> >())
            {
                this->color = remoteValue.value<QSharedPointer<HSSRgb> >();
            }
            else
            {
                this->color = QSharedPointer<HSSRgb>(new HSSRgb(this->getController()));
            }

            fnct->observe(HSSObservablePropertyValue, HSSObservablePropertyColor, this, new HSSValueChangedCallback<HSSFont > (this, &HSSFont::colorChanged));
            valid = true;
        }

        break;
    }

    default:
        valid = false;
    }

    switch (value->getStatementType())
    {
    case HSSStatementTypeObjectDefinition:
    {
        this->dColor = value;
        QSharedPointer<HSSObjectDefinition> objdef = qSharedPointerCast<HSSObjectDefinition > (value);
        objdef->setScope(this->scope);
        objdef->setThisObj(this->getThisObj());
        objdef->apply();
        QSharedPointer<HSSObject> theobj = objdef->getObject();
        if (theobj && theobj->isA(HSSObjectTypeRgb))
        {
            this->color = qSharedPointerCast<HSSRgb>(theobj);
            valid = true;
        }

        break;
    }

    default:
        break;
    }

    if (!valid)
        throw AXRWarning("HSSDGradient", "Invalid value for color of " + this->name);

    this->notifyObservers(HSSObservablePropertyColor, &this->color);
    this->notifyObservers(HSSObservablePropertyValue, NULL);
}

void HSSFont::colorChanged(AXR::HSSObservableProperty source, void *data)
{
    axr_log(LoggerChannelObsolete1, "********************** colorChanged unimplemented ****************************");
}

QSharedPointer<HSSKeywordConstant> HSSFont::getWeight()
{
    return this->weight;
}

void HSSFont::setDWeight(QSharedPointer<HSSParserNode> value)
{

    bool valid = false;

    switch (value->getType())
    {

    case HSSParserNodeTypeFunctionCall:
    {
        this->dWeight = value;
        QSharedPointer<HSSFunction> fnct = qSharedPointerCast<HSSFunction > (value)->clone();
        if (fnct && fnct->isA(HSSFunctionTypeRef))
        {
            fnct->setScope(this->scope);
            fnct->setThisObj(this->getThisObj());
            QVariant remoteValue = fnct->evaluate();
            if (remoteValue.canConvert<QSharedPointer<HSSKeywordConstant> >())
            {
                this->weight = remoteValue.value<QSharedPointer<HSSKeywordConstant> >();
                valid = true;
            }

            fnct->observe(HSSObservablePropertyValue, HSSObservablePropertyWeight, this, new HSSValueChangedCallback<HSSFont > (this, &HSSFont::weightChanged));
        }

        break;
    }

    case HSSParserNodeTypeKeywordConstant:
    {
        QSharedPointer<HSSKeywordConstant> keywordValue = qSharedPointerCast<HSSKeywordConstant > (value);
        this->weight = keywordValue;
        valid = true;
        break;
    }

    default:
        break;
    }

    if (!valid)
        throw AXRWarning("HSSFont", "Invalid value for weight of " + this->name);

    this->notifyObservers(HSSObservablePropertyWeight, &this->weight);
    this->notifyObservers(HSSObservablePropertyValue, NULL);
}

void HSSFont::weightChanged(AXR::HSSObservableProperty source, void *data)
{
    axr_log(LoggerChannelObsolete1, "********************** weightChanged unimplemented ****************************");
}

HSSUnit HSSFont::_evaluatePropertyValue(
                                    void(HSSFont::*callback)(HSSObservableProperty property, void* data),
                                    QSharedPointer<HSSParserNode> value,
                                    HSSUnit percentageBase,
                                    HSSObservableProperty observedSourceProperty,
                                    HSSObservable * &observedStore,
                                    HSSObservableProperty &observedStoreProperty
                                    )
{
    HSSUnit ret = 0;

    HSSParserNodeType nodeType = value->getType();
    switch (nodeType)
    {
    case HSSParserNodeTypeNumberConstant:
    {
        QSharedPointer<HSSNumberConstant> numberValue = qSharedPointerCast<HSSNumberConstant > (value);
        ret = numberValue->getValue();
        break;
    }

    case HSSParserNodeTypePercentageConstant:
    {
        QSharedPointer<HSSPercentageConstant> percentageValue = qSharedPointerCast<HSSPercentageConstant > (value);
        ret = percentageValue->getValue(percentageBase);
        break;
    }

    case HSSParserNodeTypeExpression:
    {
        QSharedPointer<HSSExpression> expressionValue = qSharedPointerCast<HSSExpression > (value);
        expressionValue->setPercentageBase(percentageBase);
        //expressionValue->setScope(scope);
        ret = expressionValue->evaluate();
        if (callback)
        {
            expressionValue->observe(HSSObservablePropertyValue, observedSourceProperty, this, new HSSValueChangedCallback<HSSFont > (this, callback));
            observedStore = expressionValue.data();
            observedStoreProperty = HSSObservablePropertyValue;
        }

        break;
    }

    case HSSParserNodeTypeKeywordConstant:

        break;

    default:
        AXRWarning("HSSFont", "Unknown parser node type while setting value for HSSFont property").raise();
        break;
    }

    return ret;
}
