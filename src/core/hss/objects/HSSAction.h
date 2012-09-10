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

#ifndef HSSACTION_H
#define HSSACTION_H

#include "HSSMultipleValueDefinition.h"
#include "HSSObject.h"

namespace AXR
{
    /**
     *  @brief Abstract base class for all action objects.
     */
    class HSSAction : public HSSObject
    {
    public:
        typedef boost::shared_ptr<HSSAction> p;

        /**
         *  When logging, you often need a string representation of the action type.
         *  @param actionType   The action type to represent as a string.
         *  @return A string representation of the given type.
         */
        static std::string actionTypeStringRepresentation(HSSActionType actionType);

        /**
         *  Clones an instance of HSSAction and gives a shared pointer of the
         *  newly instanciated object.
         *  @return A shared pointer to the new HSSAction.
         */
        p clone() const;

        /**
         *  Destructor for this class.
         */
        virtual ~HSSAction();

        //see HSSObject.h for documentation of these
        virtual std::string toString();
        virtual std::string defaultObjectType();
        virtual std::string defaultObjectType(std::string property);
        virtual void setProperty(HSSObservableProperty name, HSSParserNode::p value);

        /**
         *  Allows you to check if this action is of the given type.
         *  @param  type    The action type to which to check against.
         *  @return Wether it is of the given type or not.
         */
        bool isA(HSSActionType type);

        /**
         *  @return The action type of this instance.
         */
        HSSActionType getActionType();

        /**
         *  This method is called when the action should be executed. The actual implementation
         *  is defined by each specific subclass.
         */
        virtual void fire();

    protected:
        /**
         *  This class shouldn't be called directly, but by the subclasses.
         *  @param  type    The HSSActionType that this action corresponds to. Usually set
         *                  from withing a subclass' constructor method.
         */
        HSSAction(HSSActionType type);

        /**
         *  Copy constructor for HSSAction objects. Do not call directly, use clone() instead.
         */
        HSSAction(const HSSAction & orig);

    private:
        HSSActionType actionType;
        HSSClonable::p cloneImpl() const;
    };
}


#endif
