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
 *      COPYRIGHT: ©2012 - All Rights Reserved
 *
 *      LICENSE: see LICENSE file
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

#include "HSSContainer.h"
#include "HSSLastChildFilter.h"

using namespace AXR;

HSSLastChildFilter::HSSLastChildFilter(AXRController * controller)
: HSSFilter(HSSFilterTypeLast, controller)
{

}

HSSLastChildFilter::p HSSLastChildFilter::clone() const
{
    return qSharedPointerCast<HSSLastChildFilter> (this->cloneImpl());
}

HSSLastChildFilter::~HSSLastChildFilter()
{

}

AXRString HSSLastChildFilter::toString()
{
    return "Last Child Filter";
}

HSSSelection::p HSSLastChildFilter::apply(HSSSelection::p scope, bool processing)
{
    HSSSimpleSelection::p ret(new HSSSimpleSelection());
    if (scope->isA(HSSSelectionTypeMultipleSelection))
    {
        HSSMultipleSelection::p multiSel = qSharedPointerCast<HSSMultipleSelection>(scope);
        for (HSSMultipleSelection::iterator it=multiSel->begin(); it!=multiSel->end(); ++it) {
            this->_apply(ret, *it);
        }
    }
    else if (scope->isA(HSSSelectionTypeSimpleSelection))
    {
        this->_apply(ret, qSharedPointerCast<HSSSimpleSelection>(scope));
    }
    return ret;
}

inline void HSSLastChildFilter::_apply(HSSSimpleSelection::p & ret, HSSSimpleSelection::p selection)
{
    for (HSSSimpleSelection::const_iterator it = selection->begin(); it != selection->end(); ++it)
    {
        const HSSDisplayObject::p & theDO = *it;
        unsigned int lastIndex = theDO->getParent()->getChildren()->size() - 1;
        if (this->getNegating())
        {
            if (theDO->getIndex() != lastIndex)
            {
                ret->add(theDO);
            }
        }
        else
        {
            if (theDO->getIndex() == lastIndex)
            {
                ret->add(theDO);
            }
        }
    }
}

HSSClonable::p HSSLastChildFilter::cloneImpl() const
{
    return HSSLastChildFilter::p(new HSSLastChildFilter(*this));
}
