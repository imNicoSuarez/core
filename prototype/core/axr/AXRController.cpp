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
 ********************************************************************/

#include "AXRController.h"
#include "../AXR.h"
#include "AXRExceptions.h"
#include <sstream>
#include "../axr/AXRDebugging.h"
#include "../axr/errors/errors.h"
#include "../hss/objects/HSSDisplayObject.h"
#include <boost/pointer_cast.hpp>
#include "../hss/parsing/HSSSelectorChain.h"
#include "../hss/parsing/HSSFilters.h"
#include "../hss/parsing/HSSNegation.h"
#include "../hss/parsing/HSSFlag.h"

using namespace AXR;


AXRController::AXRController()
{
    axr_log(AXR_DEBUG_CH_GENERAL | AXR_DEBUG_CH_GENERAL_SPECIFIC, "AXRController: creating controller");
    this->currentContext = std::stack<HSSContainer::p>();
}

AXRController::~AXRController()
{
    axr_log(AXR_DEBUG_CH_GENERAL | AXR_DEBUG_CH_GENERAL_SPECIFIC, "AXRController: destructing controller");
    
    this->objectTree.clear();
    this->loadSheets.clear();
    this->rules.clear();
    this->parserTree.clear();
}

void AXRController::matchRulesToContentTree()
{
    const std::vector<HSSRule::p> rules = this->getRules();
    const HSSDisplayObject::p rootScopeArr[1] = {this->getRoot()};
    const std::vector<HSSDisplayObject::p>scope(rootScopeArr, rootScopeArr+1);
    unsigned i, size;
    
    for (i=0, size=rules.size(); i<size; i++) {
        const HSSRule::p& rule = rules[i];
        std::vector<HSSSelectorChain::p> selectorChains = rule->getSelectorChains();
        if(selectorChains.size() > 0){
            std::vector< std::vector<HSSDisplayObject::p> > selection;
            try {
                selection = this->select(selectorChains, scope, this->getRoot());
            } catch (AXRError::p e) {
                e->raise();
            } catch (AXRWarning::p e) {
                e->raise();
            }
            
            unsigned i, j, k, size, size2, size3;
            for (i=0, size=selection.size(); i<size; i++) {
                std::vector<HSSDisplayObject::p> inner = selection[i];
                for (j=0, size2=inner.size(); j<size2; j++) {
                    const HSSDisplayObject::p & displayObject = inner[j];
                    axr_log(AXR_DEBUG_CH_GENERAL, "AXRController: match "+displayObject->getElementName());
                    displayObject->rulesAdd(rule, (rule->getActiveByDefault() ? HSSRuleStateOn : HSSRuleStateOff ));
                    
                    displayObject->setNeedsRereadRules(true);
                    displayObject->setNeedsSurface(true);
                    displayObject->setDirty(true);
                    
                    //if it is a container it may have children
                    if(displayObject->isA(HSSObjectTypeContainer)){
                        HSSContainer::p selectedContainer = boost::static_pointer_cast<HSSContainer>(displayObject);
                        this->currentContext.push(selectedContainer);
                        for (k=0, size3=rule->childrenSize(); k<size3; k++) {
                            const HSSRule::p childRule = rule->childrenGet(k);
                            this->recursiveMatchRulesToDisplayObjects(childRule, selectedContainer->getChildren(), selectedContainer, true);
                        }
                        this->currentContext.pop();
                    }
                    
                    displayObject->setNeedsRereadRules(true);
                }
            }
        }
    }
}

void AXRController::recursiveMatchRulesToDisplayObjects(const HSSRule::p & rule, const std::vector<HSSDisplayObject::p> & scope, HSSContainer::p container, bool applyingInstructions)
{
    axr_log(AXR_DEBUG_CH_GENERAL | AXR_DEBUG_CH_GENERAL_SPECIFIC, "AXRController: recursive matching rules to display objects");
    HSSInstruction::p instruction = rule->getInstruction();
    if (instruction && applyingInstructions) {
        switch (instruction->getInstructionType()) {
            case HSSNewInstruction:
            {
                if(container){
                    HSSSimpleSelector::p subject = rule->selectorChainsLast()->subject();
                    std::string elementName = "";
                    if(subject){
                        HSSNameSelector::p nameSel = subject->getName();
                        elementName = nameSel->getElementName();
                    }
                    
                    unsigned i;
                    unsigned argssize = 1;
                    HSSParserNode::p argument = instruction->getArgument();
                    if(argument){
                        if (argument->isA(HSSParserNodeTypeNumberConstant)) {
                            HSSNumberConstant::p argnum = boost::static_pointer_cast<HSSNumberConstant>(argument);
                            argssize = (int)argnum->getValue();
                        }
                    }
                    
                    for (i=0; i<argssize; i++) {
                        HSSContainer::p newContainer = HSSContainer::p(new HSSContainer());
                        newContainer->setName(elementName);
                        newContainer->setElementName(elementName);
                        rule->setThisObj(newContainer);
                        newContainer->setController(this);
                        newContainer->rulesAdd(rule, (rule->getActiveByDefault() ? HSSRuleStateOn : HSSRuleStateOff ));
                        axr_log(AXR_DEBUG_CH_GENERAL, "AXRController: created "+newContainer->getElementName());
                        this->add(newContainer);
                        newContainer->setNeedsRereadRules(true);
                        newContainer->setNeedsSurface(true);
                        newContainer->setDirty(true);
                        unsigned i, size;
                        this->currentContext.push(newContainer);
                        for (i=0, size=rule->childrenSize(); i<size; i++) {
                            const HSSRule::p childRule = rule->childrenGet(i);
                            this->recursiveMatchRulesToDisplayObjects(childRule, newContainer->getChildren(), newContainer, applyingInstructions);
                        }
                        newContainer->setNeedsRereadRules(true);
                        //newContainer->fireEvent(HSSEventTypeLoad);
                        this->currentContext.pop();
                    }
                } else {
                    AXRWarning::p(new AXRWarning("AXRController", "You cannot use the #new instruction at root level"))->raise();
                }
                
                break;
            }
                
            case HSSMoveInstruction:
            {
                if(container){
                    HSSContainer::p parent = container->getParent();
                    if (parent) {
                        std::vector<HSSDisplayObject::p> moveScope = parent->getChildren();
                        std::vector< std::vector<HSSDisplayObject::p> > selection = this->select(rule->getSelectorChains(), moveScope, container);
                        unsigned i, j, k, size, size2, size3;
                        
                        this->currentContext.push(container);
                        //move the children over
                        for (i=0, size=selection.size(); i<size; i++) {
                            std::vector<HSSDisplayObject::p> inner = selection[i];
                            for (j=0, size2=inner.size(); j<size2; j++) {
                                HSSDisplayObject::p theDO = inner[j];
                                if(theDO != container){
                                    theDO->removeFromParent();
                                    rule->setThisObj(theDO);
                                    theDO->setController(this);
                                    theDO->rulesAdd(rule, (rule->getActiveByDefault() ? HSSRuleStateOn : HSSRuleStateOff ));
                                    axr_log(AXR_DEBUG_CH_GENERAL, "AXRController: moved "+theDO->getElementName());
                                    this->add(theDO);
                                    theDO->setNeedsRereadRules(true);
                                    theDO->setNeedsSurface(true);
                                    theDO->setDirty(true);
                                    
                                    if(theDO->isA(HSSObjectTypeContainer)){
                                        HSSContainer::p theContainer = boost::static_pointer_cast<HSSContainer>(theDO);
                                        this->currentContext.push(theContainer);
                                        //assign more rules
                                        for (k=0, size3=rule->childrenSize(); k<size3; k++) {
                                            const HSSRule::p childRule = rule->childrenGet(k);
                                            this->recursiveMatchRulesToDisplayObjects(childRule, theContainer->getChildren(), theContainer, applyingInstructions);
                                        }
                                        this->currentContext.pop();
                                    }
                                    
                                    theDO->setNeedsRereadRules(true);
                                    //theDO->fireEvent(HSSEventTypeLoad);
                                }
                                
                            }
                        }
                        this->currentContext.pop();
                    } else {
                        AXRWarning::p(new AXRWarning("AXRController", "You cannot use the #move instruction at root level"))->raise();
                    }
                }
                
                break;
            }
                
            case HSSDeleteInstruction:
            {
                if(container){
                    //select the items to be deleted
                    std::vector< std::vector<HSSDisplayObject::p> > selection = this->select(rule->getSelectorChains(), scope, container);
                    unsigned i, j, size, size2;
                    for (i=0, size=selection.size(); i<size; i++) {
                        std::vector<HSSDisplayObject::p> inner = selection[i];
                        for (j=0, size2=inner.size(); j<size2; j++) {
                            inner[j]->removeFromParent();
                        }
                    }
                } else {
                    AXRWarning::p(new AXRWarning("AXRController", "You cannot use the #delete instruction at root level"))->raise();
                }
                break;
            }
            
            default:
                break;
        }
        
    } else if(!instruction) {
        std::vector<HSSSelectorChain::p> selectorChains = rule->getSelectorChains();
        if(selectorChains.size() > 0){
            std::vector< std::vector<HSSDisplayObject::p> > selection;
            
            if (container){
                if (applyingInstructions) {
                    //we observe the parent for dom changes
                    container->observe(HSSObservablePropertyTreeChange, HSSObservablePropertyValue, rule.get(), new HSSValueChangedCallback<HSSRule>(rule.get(), &HSSRule::treeChanged));
                    rule->setObservedTreeChanger(container.get());
                    rule->setThisObj(container);
                    rule->setOriginalScope(scope);
                }
                
                try {
                    selection = this->select(selectorChains, scope, container);
                } catch (AXRError::p e) {
                    e->raise();
                } catch (AXRWarning::p e) {
                    e->raise();
                }
                
            } else {
                try {
                    selection = this->select(selectorChains, scope, this->getRoot());
                } catch (AXRError::p e) {
                    e->raise();
                } catch (AXRWarning::p e) {
                    e->raise();
                }
            }
            
            unsigned i, j, k, size, size2, size3;
            for (i=0, size=selection.size(); i<size; i++) {
                std::vector<HSSDisplayObject::p> inner = selection[i];
                for (j=0, size2=inner.size(); j<size2; j++) {
                    const HSSDisplayObject::p & displayObject = inner[j];
                    axr_log(AXR_DEBUG_CH_GENERAL, "AXRController: match "+displayObject->getElementName());
                    displayObject->rulesAdd(rule, (rule->getActiveByDefault() ? HSSRuleStateOn : HSSRuleStateOff ));
                    
                    displayObject->setNeedsRereadRules(true);
                    displayObject->setNeedsSurface(true);
                    displayObject->setDirty(true);
                    
                    //if it is a container it may have children
                    if(displayObject->isA(HSSObjectTypeContainer)){
                        HSSContainer::p selectedContainer = boost::static_pointer_cast<HSSContainer>(displayObject);
                        this->currentContext.push(selectedContainer);
                        for (k=0, size3=rule->childrenSize(); k<size3; k++) {
                            const HSSRule::p childRule = rule->childrenGet(k);
                            this->recursiveMatchRulesToDisplayObjects(childRule, selectedContainer->getChildren(), selectedContainer, applyingInstructions);
                        }
                        this->currentContext.pop();
                    }
                    
                    displayObject->setNeedsRereadRules(true);
                }
            }
        }
    }
}

void AXRController::initializeSelectorChain(HSSSelectorChain::p selectorChain)
{
    this->currentChain = selectorChain;
    this->currentChainSize = selectorChain->size();
    this->currentChainCount = 0;
    this->currentSelectorNode = selectorChain->get(0);
}

void AXRController::readNextSelectorNode()
{
    if(this->currentChainCount < this->currentChainSize-1){
        this->currentSelectorNode = this->currentChain->get(this->currentChainCount+1);
        this->currentChainCount++;
    }
}

bool AXRController::isAtEndOfSelector()
{
    return this->currentChainCount >= this->currentChainSize-1;
}

std::vector< std::vector<HSSDisplayObject::p> > AXRController::select(std::vector<HSSSelectorChain::p> selectorChains, const std::vector<HSSDisplayObject::p> & scope, HSSDisplayObject::p thisObj)
{
    return this->select(selectorChains, scope, thisObj, true);
}

std::vector< std::vector<HSSDisplayObject::p> > AXRController::select(std::vector<HSSSelectorChain::p> selectorChains, const std::vector<HSSDisplayObject::p> & scope, HSSDisplayObject::p thisObj, bool processing)
{
    std::vector< std::vector<HSSDisplayObject::p> > ret;
    std::vector< std::vector<HSSDisplayObject::p> > selections;
    HSSSelectorChain::const_it it;
    
    for (it=selectorChains.begin(); it!=selectorChains.end(); it++) {
        this->initializeSelectorChain(*it);
        selections = this->selectHierarchical(scope, thisObj, processing);
        if(selections.size() > 0){
            std::vector< std::vector<HSSDisplayObject::p> >::const_iterator it2;
            for (it2=selections.begin(); it2!=selections.end(); it2++) {
                const std::vector<HSSDisplayObject::p> sel = *it2;
                ret.push_back(sel);
            }
        }
    }
    
    return ret;
}

std::vector< std::vector<HSSDisplayObject::p> > AXRController::selectHierarchical(const std::vector<HSSDisplayObject::p> & scope, HSSDisplayObject::p thisObj)
{
    return this->selectHierarchical(scope, thisObj, true);
}

std::vector< std::vector<HSSDisplayObject::p> > AXRController::selectHierarchical(const std::vector<HSSDisplayObject::p> & scope, HSSDisplayObject::p thisObj, bool processing)
{
    std::vector< std::vector<HSSDisplayObject::p> > selections = this->selectOnLevel(scope, thisObj, processing);
    bool atEnd = this->isAtEndOfSelector();
    if(!atEnd){
        std::vector< std::vector<HSSDisplayObject::p> > ret;
        HSSCombinatorType combinatorType = this->currentSelectorNode->getCombinatorType();
        switch (combinatorType) {
            case HSSCombinatorTypeChildren:
            {
                //if found, create a new scope with the children of the previous selections
                std::vector<HSSDisplayObject::p> newScope;
                this->readNextSelectorNode();
                unsigned i, size, j, size2, k, size3;
                for (i=0, size=selections.size(); i<size; i++) {
                    std::vector<HSSDisplayObject::p> selection = selections[i];
                    for(j=0, size2 = selection.size(); j<size2; j++){
                        if(selection[j]->isA(HSSObjectTypeContainer)){
                            HSSContainer::p theContainer = boost::static_pointer_cast<HSSContainer>(selection[j]);
                            const std::vector<HSSDisplayObject::p> children = theContainer->getChildren();
                            for(k=0, size3 = children.size(); k<size3; k++){
                                newScope.push_back(children[k]);
                            }
                        }
                    }
                    return this->selectHierarchical(newScope, thisObj, processing);
                }
                return ret;
            }
                
            case HSSCombinatorTypeDescendants:
            {
                //if found, create a new scope with the children of the previous selections
                std::vector<HSSDisplayObject::p> newScope;
                this->readNextSelectorNode();
                unsigned i, size, j, size2, k, size3;
                for (i=0, size=selections.size(); i<size; i++) {
                    std::vector<HSSDisplayObject::p> selection = selections[i];
                    for(j=0, size2 = selection.size(); j<size2; j++){
                        if(selection[j]->isA(HSSObjectTypeContainer)){
                            HSSContainer::p theContainer = boost::static_pointer_cast<HSSContainer>(selection[j]);
                            const std::vector<HSSDisplayObject::p> children = theContainer->getChildren();
                            for(k=0, size3 = children.size(); k<size3; k++){
                                newScope.push_back(children[k]);
                            }
                        }
                    }
                }
                //recursively search for matches
                return this->selectAllHierarchical(newScope, thisObj, processing);
                break;
            }
                
            default:
                break;
        } 
    }
    
    return selections;
}

std::vector< std::vector<HSSDisplayObject::p> > AXRController::selectAllHierarchical(const std::vector<HSSDisplayObject::p> & scope, HSSDisplayObject::p thisObj, bool processing)
{
    std::vector< std::vector<HSSDisplayObject::p> > selections = this->selectOnLevel(scope, thisObj, processing);
    
    unsigned i, j, k, size, size2, size3;
    for (i=0, size=selections.size(); i<size; i++) {
        
        std::vector<HSSDisplayObject::p> newSearch;
        for(j=0, size2=scope.size(); j<size2; j++){
            if(scope[j]->isA(HSSObjectTypeContainer)){
                HSSContainer::p theContainer = boost::static_pointer_cast<HSSContainer>(scope[j]);
                const std::vector<HSSDisplayObject::p> children = theContainer->getChildren();
                for(k=0, size3 = children.size(); k<size3; k++){
                    newSearch.push_back(children[k]);
                }
            }
        }
        
        std::vector< std::vector<HSSDisplayObject::p> > childSelection;
        if(newSearch.size() > 0){
            this->currentChainCount -= 1;
            this->readNextSelectorNode();
            childSelection = this->selectAllHierarchical(newSearch, thisObj, processing);
            for (j=0, size2=selections.size(); j<size2; j++) {
                //std::vector<HSSDisplayObject::p> & selection = selections[j];
                for (k=0, size3=childSelection.size(); k<size3; k++) {
                    selections[j].insert(selections[j].end(), childSelection[k].begin(), childSelection[k].end());
                }
            }
        }
    }
    
    return selections;
}

std::vector< std::vector<HSSDisplayObject::p> > AXRController::selectOnLevel(const std::vector<HSSDisplayObject::p> & scope, HSSDisplayObject::p thisObj, bool processing)
{
    std::vector< std::vector<HSSDisplayObject::p> > selections = this->selectSimple(scope, thisObj, processing);
    bool atEnd = this->isAtEndOfSelector();
    if(!atEnd){
        std::vector< std::vector<HSSDisplayObject::p> > ret;
        HSSCombinatorType combinatorType = this->currentSelectorNode->getCombinatorType();
        switch (combinatorType) {
            case HSSCombinatorTypeSiblings:
            {
                //if found, just select the right part
                this->readNextSelectorNode();
                unsigned i, size;
                for (i=0, size=selections.size(); i<size; i++) {
                    std::vector<HSSDisplayObject::p> selection = selections[i];
                    std::vector< std::vector<HSSDisplayObject::p> > newSelection = this->selectSimple(scope, thisObj, processing);
                    ret.insert(ret.end(), newSelection.begin(), newSelection.end());
                }
                return ret;
            }
                
            case HSSCombinatorTypeNextSiblings:
            {
                //find the selected ones on the right, and select all the following ones that match
                unsigned i, size;
                for (i=0, size=selections.size(); i<size; i++) {
                    std::vector<HSSDisplayObject::p> selection = selections[i];
                    std::vector<HSSDisplayObject::p> vect(scope);
                    if(selection.size() > 0){
                        std::vector<HSSDisplayObject::p>::iterator it;
                        it = std::find(vect.begin(), vect.end(), selection.front());
                        std::vector<HSSDisplayObject::p> right(it+1, vect.end());
                        this->readNextSelectorNode();
                        std::vector< std::vector<HSSDisplayObject::p> > newSelection = this->selectSimple(right, thisObj, processing);
                        ret.insert(ret.end(), newSelection.begin(), newSelection.end());
                    }
                }
                return ret;
            }
                
            case HSSCombinatorTypePreviousSiblings:
            {
                //find the selected ones on the left, and select all the previous ones that match
                this->readNextSelectorNode();
                unsigned i, size;
                for (i=0, size=selections.size(); i<size; i++) {
                    std::vector<HSSDisplayObject::p> selection = selections[i];
                    if(selection.size() > 0){
                        std::vector<HSSDisplayObject::p> vect(scope);
                        std::vector<HSSDisplayObject::p>::iterator it;
                        it = std::find(vect.begin(), vect.end(), selection.back());
                        std::vector<HSSDisplayObject::p> right(vect.begin(), it);
                        this->readNextSelectorNode();
                        std::vector< std::vector<HSSDisplayObject::p> > newSelection = this->selectSimple(right, thisObj, processing);
                        ret.insert(ret.end(), newSelection.begin(), newSelection.end());
                    }
                    
                }
                return ret;
            }
                
            default:
                break;
        } 
    }
    
    return selections;
}

std::vector< std::vector<HSSDisplayObject::p> > AXRController::selectSimple(const std::vector<HSSDisplayObject::p> & scope, HSSDisplayObject::p thisObj, bool processing)
{
    std::vector< std::vector<HSSDisplayObject::p> > ret;
    bool done = false;
    bool needsReadNext = true;
    std::vector<HSSDisplayObject::p> selection = scope;
    
    while (!done){
        switch (this->currentSelectorNode->getSelectorType()) {
            case HSSSelectorTypeSimpleSelector:
            {
                HSSSimpleSelector::p ss = boost::static_pointer_cast<HSSSimpleSelector>(this->currentSelectorNode);
                selection = ss->filterSelection(selection, thisObj, processing);
                break;
            }
            case HSSSelectorTypeThisSelector:
            {
                selection.clear();
                selection.push_back(thisObj);
                break;
            }
                
            default:
            {
                done = true;
                needsReadNext = false;
                break;
            }
        }
        
        if(this->isAtEndOfSelector()){
            done = true;
        } else if(needsReadNext) {
            this->readNextSelectorNode();
        }
    }
    
    ret.push_back(selection);
    
    return ret;
}

std::string AXRController::toString()
{
    std::string tempstr = "AXR Controller\n";
    if(this->root != NULL){
        tempstr.append("\n\n\nROOT:");
        tempstr = this->root->toString();  
    }
    unsigned i;
    if(this->objectTree.size() > 0)
    {
        tempstr.append("\n\n\nOBJECT TREE:");
        for (i=0; i<this->objectTree.size(); i++) {
            tempstr.append("\n").append(this->objectTree[i]->toString());
        }
    }
    
    if(this->parserTree.size() > 0)
    {
        tempstr.append("\n\n\nPARSER TREE:");
        HSSParserNode::it pIt;
        for (pIt=this->parserTree.begin(); pIt!=this->parserTree.end(); pIt++) {
            tempstr.append("\n").append((*pIt)->toString());
        }
    }
    
    return tempstr;
}


void AXRController::reset()
{
    unsigned i;
    
    this->objectTree.clear();
    this->loadSheets.clear();
    this->parserTree.clear();
    this->rules.clear();
    this->root.reset();
    for(i=0; i<this->currentContext.size(); i++)
    {
        this->currentContext.pop();
    }
    
    this->currentChain.reset();
    this->currentSelectorNode.reset();
    this->currentChainCount = 0;
    this->currentChainSize = 0;
}

HSSContainer::p & AXRController::getRoot()
{
    return this->root;
}

void AXRController::setRoot(HSSContainer::p newRoot){
    this->root = newRoot;
    HSSParser::p hssparser = AXRCore::getInstance()->getParserHSS();
    if(hssparser->currentObjectContextSize() == 0){
        hssparser->currentObjectContextAdd(newRoot);
    }
    newRoot->setRoot(true);
}

void AXRController::enterElement(std::string elementName)
{
    //std_log1("enter element " + elementName);
    HSSContainer::p newContainer(new HSSContainer());
    newContainer->setName(elementName+"_source_obj");
    newContainer->setElementName(elementName);
    this->add(newContainer);
    this->currentContext.push(newContainer);
}

void AXRController::addAttribute(std::string name, std::string value)
{
    //std_log1(std::string("adding attribute " + name + " and value " + value));
    this->currentContext.top()->attributesAdd(name, value);
}

void AXRController::setContentText(std::string text)
{
    this->currentContext.top()->setContentText(text);
}

void AXRController::appendContentText(std::string text)
{
    this->currentContext.top()->appendContentText(text);
}

void AXRController::exitElement()
{
    this->currentContext.pop();
}


void AXRController::add(HSSDisplayObject::p newElement)
{
    newElement->setController(this);
    
    if(!this->root){
        HSSContainer::p cont = HSSContainer::asContainer(newElement);
        if (cont){
            this->setRoot(cont);
        } else {
            std_log("############## HSSController: cannot add non-controller as root");
        }
        
    } else {
        if(this->currentContext.size() != 0){
            HSSContainer::p theCurrent = this->currentContext.top();
            theCurrent->add(newElement);
            
        } else {
            std_log("############## HSSController: tried to add a container to nonexistent current");
        }
    }
}

//object tree
void AXRController::objectTreeAdd(HSSObjectDefinition::p & newObject)
{
    this->objectTree.push_back(newObject);
}

void AXRController::objectTreeRemove(unsigned index)
{
    this->objectTree.erase(this->objectTree.begin()+index);
}

HSSObjectDefinition::p & AXRController::objectTreeGet(unsigned index)
{
    return this->objectTree[index];
}

HSSObjectDefinition::p & AXRController::objectTreeGet(std::string name)
{
    /**
     *  @todo do this with an unordered_map for better performance
     */
    unsigned i, size;
    for (i=0, size=this->objectTree.size(); i<size; i++) {
        HSSObjectDefinition::p & theObj = this->objectTreeGet(i);
        if(theObj && theObj->getObject()->name == name){
            return theObj;
        }
    }
    
    throw AXRError::p(new AXRError("AXRController", "No object with name "+name+" was found"));
}

//loadSheets

void AXRController::loadSheetsAdd(std::string sheet)
{
    this->loadSheets.push_back(sheet);
}

void AXRController::loadSheetsRemove(unsigned index)
{
    this->loadSheets.erase(this->loadSheets.begin()+index);
}

std::string AXRController::loadSheetsGet(unsigned index)
{
    return this->loadSheets[index];
}

const std::vector<std::string> AXRController::loadSheetsGet() const
{
    return this->loadSheets;
}

//parser node tree
void AXRController::setParserTree(std::vector<HSSParserNode::p> newTree)
{
    this->parserTree = newTree;
}

const std::vector<HSSParserNode::p> AXRController::getParserTree() const
{
    return this->parserTree;
}

void AXRController::parserTreeAdd(HSSParserNode::p node)
{
    this->parserTree.push_back(node);
}

void AXRController::parserTreeRemove(HSSParserNode::p node)
{
    HSSParserNode::it it = find(this->parserTree.begin(), this->parserTree.end(), node);
    if(it != this->parserTree.end()){
        this->parserTree.erase(it);
    }
}

//rules
const std::vector<HSSRule::p>& AXRController::getRules() const
{
    return this->rules;
}

void AXRController::rulesAdd(HSSRule::p & rule)
{
    this->rules.push_back(rule);
}

void AXRController::rulesRemove(unsigned index)
{
    this->rules.erase(this->rules.begin()+index);
}

HSSRule::p & AXRController::rulesGet(unsigned index)
{
    return this->rules[index];
}

unsigned const AXRController::rulesSize()
{
    return this->rules.size();
}





