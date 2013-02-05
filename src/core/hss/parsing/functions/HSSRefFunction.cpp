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
#include "AXRDebugging.h"
#include "AXRError.h"
#include "HSSCallback.h"
#include "HSSDisplayObject.h"
#include "HSSRefFunction.h"
#include "HSSSelectorChain.h"
#include "HSSSimpleSelection.h"

using namespace AXR;

HSSRefFunction::HSSRefFunction(AXRController * controller)
: HSSFunction(HSSFunctionTypeRef, controller)
{
    this->observed = NULL;
}

HSSRefFunction::HSSRefFunction(const HSSRefFunction & orig)
: HSSFunction(orig)
{
    this->modifier = orig.modifier;
    this->propertyName = orig.propertyName;
    this->observed = NULL;
}

QSharedPointer<HSSFunction> HSSRefFunction::clone() const
{
    return qSharedPointerCast<HSSFunction> (this->cloneImpl());
}

HSSRefFunction::~HSSRefFunction()
{
    if (this->observed)
    {
        this->observed->removeObserver(this->propertyName, HSSObservablePropertyValue, this);
    }
}

const AXRString & HSSRefFunction::getModifier() const
{
    return this->modifier;
}

void HSSRefFunction::setModifier(AXRString newValue)
{
    this->modifier = newValue;
    this->setDirty(true);
}

const HSSObservableProperty & HSSRefFunction::getPropertyName() const
{
    return this->propertyName;
}

void HSSRefFunction::setPropertyName(HSSObservableProperty newValue)
{
    this->propertyName = newValue;
    this->setDirty(true);
}

const std::vector<QSharedPointer<HSSSelectorChain> > & HSSRefFunction::getSelectorChains() const
{
    return this->selectorChains;
}

void HSSRefFunction::setSelectorChains(std::vector<QSharedPointer<HSSSelectorChain> > newValues)
{
    this->selectorChains = newValues;
    this->setDirty(true);
}

void HSSRefFunction::selectorChainsAdd(QSharedPointer<HSSSelectorChain> & newSelectorChain)
{
    if (newSelectorChain)
    {
        std_log3("Added selector chain to HSSRefFunction: " + newSelectorChain->toString());
        newSelectorChain->setParentNode(this->shared_from_this());
        this->selectorChains.push_back(newSelectorChain);
    }
}

void HSSRefFunction::selectorChainsRemove(unsigned int index)
{
    this->selectorChains.erase(this->selectorChains.begin() + index);
}

void HSSRefFunction::selectorChainsRemoveLast()
{
    this->selectorChains.pop_back();
}

QSharedPointer<HSSSelectorChain> & HSSRefFunction::selectorChainsGet(unsigned index)
{
    return this->selectorChains[index];
}

QSharedPointer<HSSSelectorChain> & HSSRefFunction::selectorChainsLast()
{
    return this->selectorChains.back();
}

size_t HSSRefFunction::selectorChainsSize() const
{
    return this->selectorChains.size();
}

QVariant HSSRefFunction::_evaluate()
{
    QSharedPointer<HSSSimpleSelection> selection = this->getController()->select(this->selectorChains, this->scope, this->getThisObj())->joinAll();
    if (selection->empty())
    {
        // ignore
    }
    else if (selection->size() == 1)
    {
        QSharedPointer<HSSDisplayObject> container = selection->front();
        this->_value = container->getProperty(this->propertyName);

        container->observe(this->propertyName, HSSObservablePropertyValue, this, new HSSValueChangedCallback<HSSRefFunction > (this, &HSSRefFunction::valueChanged));

        this->observed = container.data();
    }
    else
    {
        /*
        if (this->modifier == "min"){
            unsigned i, size;
            HSSUnit tempval;
            QSharedPointer<HSSDisplayObject> container;
            for (i=0, size = selection.size(); i<size; ++i) {
                tempval = *(HSSUnit*)selection[i]->getProperty(this->propertyName);
                if (tempval < *(HSSUnit*)this->_value){
                    this->_value = &tempval;
                    container = selection[i];
                }
            }

            this->_value = container->getProperty(this->propertyName);

            container->observe(this->propertyName, HSSObservablePropertyValue, this, new HSSValueChangedCallback<HSSRefFunction>(this, &HSSRefFunction::valueChanged));


        } else if (this->modifier == "max"){

        } else if (this->modifier == "avg"){

        }
         */
        throw AXRError("HSSRefFunciton", "Using modifiers in ref functions is not implemented yet");
    }
    return this->_value;
}

void HSSRefFunction::valueChanged(HSSObservableProperty source, void*data)
{
    this->setDirty(true);
    this->_value = QVariant::fromValue(data);
    this->notifyObservers(HSSObservablePropertyValue, this->_value.value<void*>());
}

QSharedPointer<HSSClonable> HSSRefFunction::cloneImpl() const
{
    QSharedPointer<HSSRefFunction> clone = QSharedPointer<HSSRefFunction>(new HSSRefFunction(*this));

    for (HSSSelectorChain::const_it sIt = this->selectorChains.begin(); sIt != this->selectorChains.end(); ++sIt)
    {
        QSharedPointer<HSSSelectorChain> clonedSelectorChain = (*sIt)->clone();
        clone->selectorChainsAdd(clonedSelectorChain);
    }

    return clone;
}
