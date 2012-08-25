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
 *      COPYRIGHT: ©2012 - All Rights Reserved
 *
 *      LICENSE: see License.txt file
 *
 *      WEB: http://axr.vg
 *
 *      THIS CODE AND INFORMATION ARE PROVIDED "AS IS"
 *      WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *      IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 *      FITNESS FOR A PARTICULAR PURPOSE.
 *
 ********************************************************************/

#include <boost/unordered_map.hpp>
#include "AXR.h"
#include "AXRController.h"
#include "HSSDisplayObject.h"
#include "HSSFlag.h"

using namespace AXR;

HSSFlag::HSSFlag()
: HSSFilter(HSSFilterTypeFlag)
{
    this->_purging = HSSRuleStateOff;
}

HSSFilter::p HSSFlag::clone() const
{
    return boost::static_pointer_cast<HSSFlag, HSSClonable > (this->cloneImpl());
}

HSSFlag::~HSSFlag()
{

}

std::string HSSFlag::toString()
{
    return "Flag with name " + this->_name;
}

std::string HSSFlag::getName()
{
    return this->_name;
}

void HSSFlag::setName(std::string newValue)
{
    this->_name = newValue;
}

void HSSFlag::flagChanged(HSSRuleState newStatus)
{
    //parent is simple selector, grandparent is selector chain, grandgrandparent is the rule
    HSSParserNode::p selectorChainNode = this->getParentNode()->getParentNode();
    HSSParserNode::p ruleNode = selectorChainNode->getParentNode();
    if (ruleNode->isA(HSSParserNodeTypeStatement))
    {
        HSSStatement::p ruleStatement = boost::static_pointer_cast<HSSStatement > (ruleNode);
        if (ruleStatement->isA(HSSStatementTypeRule))
        {
            HSSRule::p theRule = boost::static_pointer_cast<HSSRule > (ruleStatement);
            std::vector<HSSDisplayObject::p> scope = theRule->getOriginalScope();
            AXRController::p controller = AXRCore::getInstance()->getController();
            this->setPurging(newStatus);
            std::vector<std::vector<HSSDisplayObject::p> > selection = controller->select(theRule->getSelectorChains(), scope, this->getThisObj(), false);
            this->setPurging(HSSRuleStateOff);
            std::vector<std::vector<HSSDisplayObject::p> >::const_iterator outer;
            std::vector<HSSDisplayObject::p>::const_iterator inner;
            for (outer = selection.begin(); outer != selection.end(); outer++)
            {
                const std::vector<HSSDisplayObject::p> & innerv = *outer;
                for (inner = innerv.begin(); inner != innerv.end(); inner++)
                {
                    (*inner)->setRuleStatus(theRule, newStatus);
                }
            }
        }
    }
}

const std::vector<HSSDisplayObject::p> HSSFlag::apply(const std::vector<HSSDisplayObject::p> &scope, bool processing)
{
    if (processing)
    {
        HSSDisplayObject::const_it it;
        for (it = scope.begin(); it != scope.end(); it++)
        {
            const HSSDisplayObject::p & theDO = *it;
            //parent is simple selector, grandparent is selector chain, grandgrandparent is the rule
            HSSParserNode::p ruleNode = this->getParentNode()->getParentNode()->getParentNode();
            if (ruleNode->isA(HSSParserNodeTypeStatement))
            {
                HSSStatement::p ruleStatement = boost::static_pointer_cast<HSSStatement > (ruleNode);
                if (ruleStatement->isA(HSSStatementTypeRule))
                {
                    HSSRule::p theRule = boost::static_pointer_cast<HSSRule > (ruleStatement);

                    theRule->setActiveByDefault(this->getNegating());
                    theDO->createFlag(this->shared_from_this(), (this->getNegating() ? HSSRuleStateOn : HSSRuleStateOff));
                }
            }
        }

        return scope;
    }
    else
    {
        if (scope.size() > 0)
        {
            std::vector<HSSDisplayObject::p> ret;
            HSSDisplayObject::const_it it;
            for (it = scope.begin(); it != scope.end(); it++)
            {
                const HSSDisplayObject::p & theDO = *it;
                HSSRuleState purgingState = this->getPurging();
                if (purgingState)
                {
                    HSSRuleState state = theDO->flagState(this->getName());
                    if (state == purgingState)
                    {
                        ret.push_back(theDO);
                    }

                }
                else
                {
                    bool firstMatch = (theDO->flagState(this->getName()) == HSSRuleStateOn) && !this->getNegating();
                    bool secondMatch = (theDO->flagState(this->getName()) == HSSRuleStateOff) && this->getNegating();
                    if (firstMatch || secondMatch)
                    {
                        ret.push_back(theDO);
                    }
                }
            }
            return ret;
        }
        else
        {
            return scope;
        }
    }
}

HSSRuleState HSSFlag::getPurging()
{
    return this->_purging;
}

void HSSFlag::setPurging(HSSRuleState newValue)
{
    this->_purging = newValue;
}

HSSClonable::p HSSFlag::cloneImpl() const
{
    return HSSClonable::p(new HSSFlag(*this));
}

HSSFlag::p HSSFlag::shared_from_this()
{
    return boost::static_pointer_cast<HSSFlag > (HSSParserNode::shared_from_this());
}
