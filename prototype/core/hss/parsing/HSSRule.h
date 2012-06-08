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
 *      AUTHORS: Miro Keller
 *      
 *      COPYRIGHT: ©2011 - All Rights Reserved
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
 ********************************************************************
 *
 *      FILE INFORMATION:
 *      =================
 *      Last changed: 2012/03/25
 *      HSS version: 1.0
 *      Core version: 0.47
 *      Revision: 9
 *
 ********************************************************************/

#ifndef HSSRULE_H
#define HSSRULE_H


#include "HSSStatement.h"
#include "HSSPropertyDefinition.h"
#include "HSSSelectorChain.h"
#include "HSSInstruction.h"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "HSSFilterType.h"

namespace AXR {
    
    class HSSDisplayObject;
    class HSSFilter;
    
    /**
     *  @addtogroup typeEnums
     *  @{
     *  @enum HSSRuleState
     *  The status of the rule, to determine if the properties should be applied or not.
     */
    enum HSSRuleState
    {
        HSSRuleStateOff = 0, /**< Don't apply the contents of the rule. */
        HSSRuleStateOn = 1, /**< Do apply the contents of the rule. */
        HSSRuleStateActivate, /**< Set to ruleStateOn the next cycle. */
        HSSRuleStatePurge /**< Set to ruleStateOff on the next cycle. */
    };
    /** @} */
    
    /**
     *  @brief This class encapsulates a relation between a rule a its state.
     *
     *  Display objects use these to determine if the rules they hold pointers to
     *  are to be applied or not.
     */
    class HSSRuleStatus
    {
    public:
        
        /**
         *  The shared pointer to this class.
         */
        typedef boost::shared_ptr<HSSRuleStatus> p;
        
        /**
         *  The current state the rule.
         */
        HSSRuleState state;
        
        /**
         *  A shared pointer to the rule.
         */
        boost::shared_ptr<HSSRule> rule;
    };
    
    /**
     *  @brief Rules are made up of selector chain + block, which can contain property definitions and
     *  other rules.
     *
     *  Rules are read by the parser, then applied on display objects, where the property definitions
     *  are used to determine how the elements are going to be displayed on the page.
     */
    class HSSRule : public HSSStatement
    {    
    public:
        typedef boost::shared_ptr<HSSRule> p;
        typedef std::deque<p>::iterator it;
        typedef std::deque<p>::const_iterator const_it;
        
        /**
         *  Creates a new instance of a request action.
         */
        HSSRule();
        
        /**
         *  Copy constructor for HSSRequest objects. Do not call directly,
         *  use clone() instead.
         */
        HSSRule(const HSSRule &orig);
        
        /**
         *  Clones an instance of HSSRequest and gives a shared pointer of the
         *  newly instanciated object.
         *  @return A shared pointer to the new HSSRequest
         */
        p clone() const;
        
        /**
         *  Destructor for this class.
         */
        ~HSSRule();
        
        std::string toString();
        
        /**
         *  Setter fo the selector chain, which is used to select elements from the
         *  content tree.
         *  @param newChain     A shared pointer to a selector chain.
         */
        void setSelectorChain(HSSSelectorChain::p newChain);
        
        HSSSelectorChain::p getSelectorChain();
        
        const std::vector<HSSPropertyDefinition::p> & getProperties() const;
        
        void propertiesAdd(HSSPropertyDefinition::p & newProperty);
        
        HSSPropertyDefinition::p &propertiesGet(unsigned index);
        
        void propertiesRemove(unsigned index);
        
        void propertiesRemoveLast();
        
        HSSPropertyDefinition::p &propertiesLast();
        
        const int propertiesSize();
        
        void childrenAdd(HSSRule::p newRule);
        
        HSSRule::p childrenGet(unsigned index);
        
        void childrenRemove(unsigned index);
        
        void childrenRemoveLast();
        
        const int childrenSize();
        
        void setInstruction(HSSInstruction::p newInstruction);
        
        HSSInstruction::p getInstruction();
        
        virtual void setThisObj(boost::shared_ptr<HSSDisplayObject> value);
        
        void treeChanged(HSSObservableProperty source, void*data);
        
        bool getActiveByDefault();
        
        void setActiveByDefault(bool newValue);
        
        const std::vector<boost::weak_ptr<HSSDisplayObject> > getAppliedTo() const;
        
        void setAppliedTo(std::vector<boost::weak_ptr<HSSDisplayObject> > newObjects);
        
        void appliedToAdd(boost::shared_ptr<HSSDisplayObject> displayObject);
        
        const std::vector<boost::shared_ptr<HSSDisplayObject> > getOriginalScope() const;
        
        void setOriginalScope(const std::vector<boost::shared_ptr<HSSDisplayObject> > & scope);
        
        void setObservedTreeChanger(HSSObservable * newValue);
        
        HSSObservable * getObservedTreeChanger();
        
    protected:
        HSSRule::p shared_from_this();
        
        HSSSelectorChain::p selectorChain;
        std::vector<HSSPropertyDefinition::p> properties;
        std::vector<HSSRule::p>children;
        HSSInstruction::p instruction;
        
        bool _activeByDefault;
        
        std::vector<boost::weak_ptr<HSSDisplayObject> > appliedTo;
        
        std::vector<boost::shared_ptr<HSSDisplayObject> > _originalScope;
        
    private:
        virtual HSSClonable::p cloneImpl() const;
        
        boost::unordered_map<HSSFilterType, std::vector<boost::shared_ptr<HSSDisplayObject> > > _interactors;
        HSSObservable * observedTreeChanger;
        
    };
}

#endif
