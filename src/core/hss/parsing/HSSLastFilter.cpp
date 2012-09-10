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

#include "HSSLastFilter.h"

using namespace AXR;

HSSLastFilter::HSSLastFilter()
: HSSFilter(HSSFilterTypeLast)
{

}

HSSLastFilter::p HSSLastFilter::clone() const
{
    return boost::static_pointer_cast<HSSLastFilter, HSSClonable > (this->cloneImpl());
}

HSSLastFilter::~HSSLastFilter()
{

}

std::string HSSLastFilter::toString()
{
    return "Last Filter";
}

const std::vector<HSSDisplayObject::p> HSSLastFilter::apply(const std::vector<HSSDisplayObject::p> &scope, bool processing)
{
    if (scope.size() > 0)
    {
        if (this->getNegating())
        {
            std::vector<HSSDisplayObject::p> ret;
            ret.insert(ret.begin(), scope.begin(), scope.end() - 1);
            return ret;
        }
        else
        {
            std::vector<HSSDisplayObject::p> ret;
            ret.push_back(scope[scope.size() - 1]);
            return ret;
        }
    }
    else
    {
        return scope;
    }
}

HSSClonable::p HSSLastFilter::cloneImpl() const
{
    return HSSClonable::p(new HSSLastFilter(*this));
}
