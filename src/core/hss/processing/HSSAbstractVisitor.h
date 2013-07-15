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

#ifndef HSSABSTRACTVISITOR_H
#define HSSABSTRACTVISITOR_H

#include "AXRGlobal.h"

namespace AXR
{
    class HSSContainer;
    class HSSDisplayObject;
    class HSSTextBlock;

    class AXR_API HSSAbstractVisitor
    {
        Q_DISABLE_COPY(HSSAbstractVisitor)
    public:
        enum VisitorFilterFlags
        {
            VisitorFilterNone = 0,
            VisitorFilterAll = 0x01,
            VisitorFilterSkip = 0x02,
            VisitorFilterRendering = 0x04,
            VisitorFilterDiagnostic = 0x08,

            // Custom flags use the highest bits of a 16-bit
            // integer, the minimum size required by the C++
            // standard for an int type; this way we can add
            // new flags without moving custom ones
            VisitorFilterCustom1 = 0x1000,
            VisitorFilterCustom2 = 0x2000,
            VisitorFilterCustom3 = 0x4000,
            VisitorFilterCustom4 = 0x8000
        };

        HSSAbstractVisitor();
        virtual ~HSSAbstractVisitor();

        void setFilterFlags(VisitorFilterFlags filterFlags);
        HSSVisitorFilterFlags getFilterFlags() const;

        virtual void initializeVisit() = 0;
        virtual void visit(HSSContainer &container) = 0;
        virtual void visit(HSSTextBlock &textBlock) = 0;
        virtual void finalizeVisit() = 0;
        virtual void reset() = 0;

    private:
        class Private;
        Private *d;
    };
}

#endif // HSSABSTRACTVISITOR_H
