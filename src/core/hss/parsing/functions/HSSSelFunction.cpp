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
#include "HSSDisplayObject.h"
#include "HSSSelFunction.h"
#include "HSSSelectorChain.h"
#include "HSSSimpleSelection.h"
#include "HSSValue.h"

using namespace AXR;

HSSSelFunction::HSSSelFunction(AXRController * controller)
: HSSFunction(HSSFunctionTypeSel, controller)
{

}

HSSSelFunction::~HSSSelFunction()
{
    axr_log(LoggerChannelGeneralSpecific, "HSSSelFunction: destructing sel function");
    this->cleanTrackedObservers();
}

HSSSelFunction::HSSSelFunction(const HSSSelFunction & orig)
: HSSFunction(orig)
{

}

QSharedPointer<HSSFunction> HSSSelFunction::clone() const
{
    return qSharedPointerCast<HSSFunction> (this->cloneImpl());
}

AXRString HSSSelFunction::toString()
{
    AXRString tempstr = AXRString("HSSSelFunction\n");
    return tempstr;
}

bool HSSSelFunction::equalTo(QSharedPointer<HSSParserNode> otherNode)
{
    //check wether pointers are the same
    if (this == otherNode.data()) return true;
    //other checks
    if ( ! HSSFunction::equalTo(otherNode)) return false;
    QSharedPointer<HSSSelFunction> castedNode = qSharedPointerCast<HSSSelFunction>(otherNode);
    if ( ! this->selection->equalTo(castedNode->selection) ) return false;
    unsigned i = 0;
    Q_FOREACH(QSharedPointer<HSSSelectorChain> selectorChain, this->selectorChains)
    {
        if ( ! selectorChain->equalTo(castedNode->selectorChains[i])) return false;
        ++i;
    }
    return true;
}

const std::vector<QSharedPointer<HSSSelectorChain> > & HSSSelFunction::getSelectorChains() const
{
    return this->selectorChains;
}

void HSSSelFunction::setSelectorChains(std::vector<QSharedPointer<HSSSelectorChain> > newValues)
{
    this->selectorChains = newValues;
    this->setDirty(true);
}

void HSSSelFunction::selectorChainsAdd(QSharedPointer<HSSSelectorChain> & newSelectorChain)
{
    if (newSelectorChain)
    {
        newSelectorChain->setParentNode(this->shared_from_this());
        this->selectorChains.push_back(newSelectorChain);
    }
}

void HSSSelFunction::selectorChainsRemove(off_t index)
{
    this->selectorChains.erase(this->selectorChains.begin() + index);
}

void HSSSelFunction::selectorChainsRemoveLast()
{
    this->selectorChains.pop_back();
}

QSharedPointer<HSSSelectorChain> & HSSSelFunction::selectorChainsGet(size_t index)
{
    return this->selectorChains[index];
}

QSharedPointer<HSSSelectorChain> & HSSSelFunction::selectorChainsLast()
{
    return this->selectorChains.back();
}

size_t HSSSelFunction::selectorChainsSize() const
{
    return this->selectorChains.size();
}

QSharedPointer<HSSObject> HSSSelFunction::_evaluate()
{
    this->selection = this->getController()->select(this->selectorChains, this->scope, this->getThisObj());
    this->_value = this->selection;
    return this->_value;
}

//void HSSSelFunction::valueChanged(const AXRString source, const QSharedPointer<HSSObject> theObj)
//{
//    this->setDirty(true);
//    this->_value = data;
//    this->notifyObservers(HSSObservablePropertyValue, this->_value);
//}

QSharedPointer<HSSClonable> HSSSelFunction::cloneImpl() const
{
    QSharedPointer<HSSSelFunction> clone = QSharedPointer<HSSSelFunction>(new HSSSelFunction(*this));

    for (HSSSelectorChain::const_it sIt = this->selectorChains.begin(); sIt != this->selectorChains.end(); ++sIt)
    {
        QSharedPointer<HSSSelectorChain> clonedSelectorChain = (*sIt)->clone();
        clone->selectorChainsAdd(clonedSelectorChain);
    }

    return clone;
}
