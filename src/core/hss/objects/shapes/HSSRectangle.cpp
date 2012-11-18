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

#include "AXRDebugging.h"
#include "HSSBorder.h"
#include "HSSKeywordConstant.h"
#include "HSSRectangle.h"
#include <QPainterPath>

using namespace AXR;

HSSRectangle::HSSRectangle(AXRController * controller)
: HSSShape(HSSShapeTypeRectangle, controller)
{
    axr_log(AXR_DEBUG_CH_GENERAL_SPECIFIC, "HSSRectangle: creating rectangle object");
}

HSSRectangle::HSSRectangle(const HSSRectangle & orig)
: HSSShape(orig)
{

}

HSSRectangle::p HSSRectangle::clone() const
{
    axr_log(AXR_DEBUG_CH_GENERAL_SPECIFIC, "HSSRectangle: cloning rectangle object");
    return qSharedPointerCast<HSSRectangle> (this->cloneImpl());
}

HSSClonable::p HSSRectangle::cloneImpl() const
{
    return HSSRectangle::p(new HSSRectangle(*this));
}

HSSRectangle::~HSSRectangle()
{
    axr_log(AXR_DEBUG_CH_GENERAL_SPECIFIC, "HSSRectangle: destructing rectangle object");
}

AXRString HSSRectangle::toString()
{
    return "HSSRectangle";
}

AXRString HSSRectangle::defaultObjectType()
{
    return "HSSRectangle";
}

AXRString HSSRectangle::defaultObjectType(AXRString property)
{
    return HSSShape::defaultObjectType(property);
}

bool HSSRectangle::isKeyword(AXRString value, AXRString property)
{
    return HSSShape::isKeyword(value, property);
}

void HSSRectangle::createPath(QPainterPath &path, HSSUnit x, HSSUnit y, HSSUnit width, HSSUnit height, std::vector<HSSParserNode::p> segments)
{
    if(segments.size() > 0){
        for (std::vector<HSSParserNode::p>::const_iterator it = segments.begin(); it!=segments.end(); ++it) {
            const HSSParserNode::p & segment = *it;
            switch (segment->getType()) {
                case HSSParserNodeTypeKeywordConstant:
                {
                    HSSKeywordConstant::p theKw = qSharedPointerCast<HSSKeywordConstant>(segment);
                    AXRString theValue = theKw->getValue();
                    if(theValue == "left")
                    {
                        path.moveTo(x, y+height);
                        path.lineTo(x, y);
                    }
                    else if (theValue == "top")
                    {
                        path.moveTo(x, y);
                        path.lineTo(x+width, y);
                    }
                    else if (theValue == "right")
                    {
                        path.moveTo(x+width, y);
                        path.lineTo(x+width, y+height);
                    }
                    else if (theValue == "bottom")
                    {
                        path.moveTo(x+width, y+height);
                        path.lineTo(x, y+height);
                    }
                    break;
                }

                default:
                    break;
            }
        }
    } else {
        path.addRect(x, y, width, height);
    }
}

void HSSRectangle::drawBorders(QPainter &painter, std::vector<QSharedPointer<HSSBorder> > borders, HSSUnit width, HSSUnit height, HSSUnit borderBleeding)
{
    //sort borders in three groups
    std::vector<HSSBorder::p> centered, inside, outside;
    for (HSSBorder::it it=borders.begin(); it!=borders.end(); ++it) {
        const HSSBorder::p & theBorder = *it;
        HSSBorderPosition thePos = theBorder->getPosition();
        if (thePos == HSSBorderPositionCentered)
        {
            centered.push_back(theBorder);
        }
        else if (thePos == HSSBorderPositionInside)
        {
            inside.push_back(theBorder);
        }
        else if (thePos == HSSBorderPositionOutside)
        {
            outside.push_back(theBorder);
        }
    }

    HSSUnit topThickness = 0., rightThickness = 0., bottomThickness = 0., leftThickness = 0.;
    for (HSSBorder::it it=centered.begin(); it!=centered.end(); ++it) {
        const HSSBorder::p & theBorder = *it;
        const std::vector<HSSParserNode::p> & segments = theBorder->getSegments();
        bool hasAll = false;
        for (std::vector<HSSParserNode::p>::const_iterator it = segments.begin(); it!=segments.end(); ++it) {
            const HSSParserNode::p & segment = *it;
            if(segment->isA(HSSParserNodeTypeKeywordConstant))
            {
                HSSKeywordConstant::p theKw = qSharedPointerCast<HSSKeywordConstant>(segment);
                if (theKw->getValue() == "all")
                {
                    hasAll = true;
                    break;
                }
            }
        }
        if(segments.size() == 0 || hasAll){
            topThickness += theBorder->getSize();
            rightThickness += theBorder->getSize();
            bottomThickness += theBorder->getSize();
            leftThickness += theBorder->getSize();
        }
        else
        {
            for (HSSParserNode::const_it it2=segments.begin(); it2!=segments.end(); ++it2) {
                const HSSParserNode::p & theSeg = *it2;
                if (theSeg->isA(HSSParserNodeTypeKeywordConstant)) {
                    AXRString segment = qSharedPointerCast<HSSKeywordConstant>(theSeg)->getValue();
                    if(segment == "top"){
                        topThickness += theBorder->getSize();
                    }
                    else if(segment == "right")
                    {
                        rightThickness += theBorder->getSize();
                    }
                    else if(segment == "bottom")
                    {
                        bottomThickness += theBorder->getSize();
                    }
                    else if(segment == "left")
                    {
                        leftThickness += theBorder->getSize();
                    }
                }
            }
        }
    }

    HSSUnit topCorrection = 0., rightCorrection = 0., bottomCorrection = 0., leftCorrection = 0.;
    if ((int) topThickness % 2) {
        topCorrection = 0.5;
    }
    if ((int) rightThickness % 2) {
        rightCorrection = 0.5;
    }
    if ((int) bottomThickness % 2) {
        bottomCorrection = 0.5;
    }
    if ((int) leftThickness % 2) {
        leftCorrection = 0.5;
    }

    // Uncomment this to see the dimensions of the borders surface
    //QPainterPath outerPath;
    //outerPath.addRect(0.5, 0.5, width+borderBleeding*2-1, height+borderBleeding*2-1);
    //painter.strokePath(outerPath, QPen(Qt::red, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));

    HSSUnit topCumulative = 0., rightCumulative = 0., bottomCumulative = 0., leftCumulative = 0.;
    for (HSSBorder::it it=centered.begin(); it!=centered.end(); ++it) {
        const HSSBorder::p & theBorder = *it;
        HSSUnit theSize = theBorder->getSize();

        QPainterPath path;
        std::vector<HSSParserNode::p> segments = theBorder->getSegments();

        bool hasAll = false;
        for (std::vector<HSSParserNode::p>::const_iterator it = segments.begin(); it!=segments.end(); ++it) {
            const HSSParserNode::p & segment = *it;
            if(segment->isA(HSSParserNodeTypeKeywordConstant))
            {
                HSSKeywordConstant::p theKw = qSharedPointerCast<HSSKeywordConstant>(segment);
                if (theKw->getValue() == "all")
                {
                    hasAll = true;
                    break;
                }
            }
        }
        if(segments.size() > 0 && !hasAll){
            for (std::vector<HSSParserNode::p>::const_iterator it = segments.begin(); it!=segments.end(); ++it) {
                const HSSParserNode::p & segment = *it;
                switch (segment->getType()) {
                    case HSSParserNodeTypeKeywordConstant:
                    {
                        HSSKeywordConstant::p theKw = qSharedPointerCast<HSSKeywordConstant>(segment);
                        AXRString theValue = theKw->getValue();
                        if (theValue == "top")
                        {
                            HSSUnit leftOffset = (leftThickness / 2) - leftCumulative + leftCorrection;
                            HSSUnit topOffset = (topThickness / 2) - topCumulative - (theSize / 2) + topCorrection;
                            HSSUnit rightOffset = (rightThickness / 2) - rightCumulative + rightCorrection;

                            path.moveTo(borderBleeding+leftOffset, borderBleeding+topOffset);
                            path.lineTo(borderBleeding+width-rightOffset, borderBleeding+topOffset);
                            topCumulative += theSize;
                        }
                        else if (theValue == "right")
                        {
                            HSSUnit topOffset = (topThickness / 2) - topCumulative + topCorrection;
                            HSSUnit rightOffset = (rightThickness / 2) - rightCumulative - (theSize / 2) + rightCorrection;
                            HSSUnit bottomOffset = (bottomThickness / 2) - bottomCumulative + bottomCorrection;

                            path.moveTo(borderBleeding+width-rightOffset, borderBleeding+topOffset);
                            path.lineTo(borderBleeding+width-rightOffset, borderBleeding+height-bottomOffset);
                            rightCumulative += theSize;
                        }
                        else if (theValue == "bottom")
                        {
                            HSSUnit leftOffset = (leftThickness / 2) - leftCumulative + leftCorrection;
                            HSSUnit rightOffset = (rightThickness / 2) - rightCumulative + rightCorrection;
                            HSSUnit bottomOffset = (bottomThickness / 2) - bottomCumulative - (theSize / 2) + bottomCorrection;

                            path.moveTo(borderBleeding+width-rightOffset, borderBleeding+height-bottomOffset);
                            path.lineTo(borderBleeding+leftOffset, borderBleeding+height-bottomOffset);
                            bottomCumulative += theSize;
                        }
                        else if(theValue == "left")
                        {
                            HSSUnit leftOffset = (leftThickness / 2) - leftCumulative - (theSize / 2) + leftCorrection;
                            HSSUnit topOffset = (topThickness / 2) - topCumulative + topCorrection;
                            HSSUnit bottomOffset = (bottomThickness / 2) - bottomCumulative + bottomCorrection;

                            path.moveTo(borderBleeding+leftOffset, borderBleeding+height-bottomOffset);
                            path.lineTo(borderBleeding+leftOffset, borderBleeding+topOffset);
                            leftCumulative += theSize;
                        }
                        break;
                    }

                    default:
                        break;
                }
            }
        } else {
            HSSUnit leftOffset = (leftThickness / 2) - leftCumulative - (theSize / 2) + leftCorrection;
            HSSUnit topOffset = (topThickness / 2) - topCumulative - (theSize / 2) + topCorrection;
            HSSUnit rightOffset = (rightThickness / 2) - rightCumulative - (theSize / 2) + rightCorrection;
            HSSUnit bottomOffset = (bottomThickness / 2) - bottomCumulative - (theSize / 2) + bottomCorrection;

            path.addRect(borderBleeding + leftOffset, borderBleeding + topOffset, width-rightOffset-leftOffset, height-bottomOffset-topOffset);

            leftCumulative += theSize;
            topCumulative += theSize;
            rightCumulative += theSize;
            bottomCumulative += theSize;
        }
        theBorder->draw(painter, path);
    }

    topCumulative = rightCumulative = bottomCumulative = leftCumulative = 0.;
    for (std::vector<HSSBorder::p>::reverse_iterator it=inside.rbegin(); it!=inside.rend(); ++it) {
        const HSSBorder::p & theBorder = *it;
        HSSUnit theSize = theBorder->getSize();

        QPainterPath path;
        std::vector<HSSParserNode::p> segments = theBorder->getSegments();

        bool hasAll = false;
        for (std::vector<HSSParserNode::p>::const_iterator it = segments.begin(); it!=segments.end(); ++it) {
            const HSSParserNode::p & segment = *it;
            if(segment->isA(HSSParserNodeTypeKeywordConstant))
            {
                HSSKeywordConstant::p theKw = qSharedPointerCast<HSSKeywordConstant>(segment);
                if (theKw->getValue() == "all")
                {
                    hasAll = true;
                    break;
                }
            }
        }
        if(segments.size() > 0 && !hasAll){
            for (std::vector<HSSParserNode::p>::const_iterator it = segments.begin(); it!=segments.end(); ++it) {
                const HSSParserNode::p & segment = *it;
                switch (segment->getType()) {
                    case HSSParserNodeTypeKeywordConstant:
                    {
                        HSSUnit leftOffset = (leftThickness / 2) + leftCumulative + leftCorrection;
                        HSSUnit topOffset = (topThickness / 2) + topCumulative + topCorrection;
                        HSSUnit rightOffset = (rightThickness / 2) + rightCumulative + rightCorrection;
                        HSSUnit bottomOffset = (bottomThickness / 2) + bottomCumulative + bottomCorrection;

                        HSSKeywordConstant::p theKw = qSharedPointerCast<HSSKeywordConstant>(segment);
                        AXRString theValue = theKw->getValue();
                        if (theValue == "top")
                        {
                            path.moveTo(borderBleeding+leftOffset, borderBleeding+topOffset+(theSize/2));
                            path.lineTo(borderBleeding+width-rightOffset, borderBleeding+topOffset+(theSize/2));
                            topCumulative += theSize;
                        }
                        else if (theValue == "right")
                        {
                            path.moveTo(borderBleeding+width-rightOffset+(theSize/2), borderBleeding+topOffset);
                            path.lineTo(borderBleeding+width-rightOffset+(theSize/2), borderBleeding+height-bottomOffset);
                            rightCumulative += theSize;
                        }
                        else if (theValue == "bottom")
                        {
                            path.moveTo(borderBleeding+width-rightOffset, borderBleeding+height-(bottomOffset+(theSize/2)));
                            path.lineTo(borderBleeding+leftOffset, borderBleeding+height-(bottomOffset+(theSize/2)));
                            bottomCumulative += theSize;
                        }
                        else if(theValue == "left")
                        {
                            path.moveTo(borderBleeding+leftOffset+(theSize/2), borderBleeding+height-bottomOffset);
                            path.lineTo(borderBleeding+leftOffset+(theSize/2), borderBleeding+topOffset);
                            leftCumulative += theSize;
                        }
                        break;
                    }

                    default:
                        break;
                }
            }
        } else {
            HSSUnit leftOffset = (leftThickness / 2) + leftCumulative + (theSize / 2) + leftCorrection;
            HSSUnit topOffset = (topThickness / 2) + topCumulative + (theSize / 2) + topCorrection;
            HSSUnit rightOffset = (rightThickness / 2) + rightCumulative + (theSize / 2) + rightCorrection;
            HSSUnit bottomOffset = (bottomThickness / 2) + bottomCumulative + (theSize / 2) + bottomCorrection;

            path.addRect(borderBleeding + leftOffset, borderBleeding + topOffset, width-rightOffset-leftOffset, height-bottomOffset-topOffset);

            leftCumulative += theSize;
            topCumulative += theSize;
            rightCumulative += theSize;
            bottomCumulative += theSize;
        }

        theBorder->draw(painter, path);
    }

    topCumulative = rightCumulative = bottomCumulative = leftCumulative = 0.;
    for (std::vector<HSSBorder::p>::iterator it=outside.begin(); it!=outside.end(); ++it) {
        const HSSBorder::p & theBorder = *it;
        HSSUnit theSize = theBorder->getSize();

        QPainterPath path;
        std::vector<HSSParserNode::p> segments = theBorder->getSegments();

        bool hasAll = false;
        for (std::vector<HSSParserNode::p>::const_iterator it = segments.begin(); it!=segments.end(); ++it) {
            const HSSParserNode::p & segment = *it;
            if(segment->isA(HSSParserNodeTypeKeywordConstant))
            {
                HSSKeywordConstant::p theKw = qSharedPointerCast<HSSKeywordConstant>(segment);
                if (theKw->getValue() == "all")
                {
                    hasAll = true;
                    break;
                }
            }
        }
        if(segments.size() > 0 && !hasAll){
            for (std::vector<HSSParserNode::p>::const_iterator it = segments.begin(); it!=segments.end(); ++it) {
                const HSSParserNode::p & segment = *it;
                switch (segment->getType()) {
                    case HSSParserNodeTypeKeywordConstant:
                    {
                        HSSUnit leftOffset = (leftThickness / 2) + leftCumulative + leftCorrection;
                        HSSUnit topOffset = (topThickness / 2) + topCumulative + topCorrection;
                        HSSUnit rightOffset = (rightThickness / 2) + rightCumulative + rightCorrection;
                        HSSUnit bottomOffset = (bottomThickness / 2) + bottomCumulative + bottomCorrection;

                        HSSKeywordConstant::p theKw = qSharedPointerCast<HSSKeywordConstant>(segment);
                        AXRString theValue = theKw->getValue();
                        if (theValue == "top")
                        {
                            path.moveTo(borderBleeding-leftOffset, borderBleeding-(topOffset+(theSize/2)));
                            path.lineTo(borderBleeding+width+rightOffset, borderBleeding-(topOffset+(theSize/2)));
                            topCumulative += theSize;
                        }
                        else if (theValue == "right")
                        {
                            path.moveTo(borderBleeding+width+(rightOffset+(theSize/2)), borderBleeding-topOffset);
                            path.lineTo(borderBleeding+width+(rightOffset+(theSize/2)), borderBleeding+height+bottomOffset);
                            rightCumulative += theSize;
                        }
                        else if (theValue == "bottom")
                        {
                            path.moveTo(borderBleeding+width+rightOffset, borderBleeding+height+(bottomOffset+(theSize/2)));
                            path.lineTo(borderBleeding-leftOffset, borderBleeding+height+(bottomOffset+(theSize/2)));
                            bottomCumulative += theSize;
                        }
                        else if(theValue == "left")
                        {
                            path.moveTo(borderBleeding-(leftOffset+(theSize/2)), borderBleeding+height+bottomOffset);
                            path.lineTo(borderBleeding-(leftOffset+(theSize/2)), borderBleeding-topOffset);
                            leftCumulative += theSize;
                        }
                        break;
                    }

                    default:
                        break;
                }
            }
        } else {
            HSSUnit leftOffset = (leftThickness / 2) + leftCumulative + (theSize / 2) + leftCorrection;
            HSSUnit topOffset = (topThickness / 2) + topCumulative + (theSize / 2) + topCorrection;
            HSSUnit rightOffset = (rightThickness / 2) + rightCumulative + (theSize / 2) + rightCorrection;
            HSSUnit bottomOffset = (bottomThickness / 2) + bottomCumulative + (theSize / 2) + bottomCorrection;

            path.addRect(borderBleeding - leftOffset, borderBleeding - topOffset, width+rightOffset+leftOffset, height+bottomOffset+topOffset);

            leftCumulative += theSize;
            topCumulative += theSize;
            rightCumulative += theSize;
            bottomCumulative += theSize;
        }

        theBorder->draw(painter, path);
    }
}
