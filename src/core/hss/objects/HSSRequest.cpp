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

#include "AXR.h"
#include "AXRController.h"
#include "HSSRequest.h"
#include "HSSSelFunction.h"

using namespace AXR;

HSSRequest::HSSRequest()
: HSSAction(HSSActionTypeRequest)
{
    axr_log(AXR_DEBUG_CH_GENERAL_SPECIFIC, "HSSRequest: creating request object");
    this->observedSrc
            = this->observedTarget
            = NULL;

    std::vector<std::string> shorthandProperties;
    shorthandProperties.push_back("src");
    shorthandProperties.push_back("target");
    //shorthandProperties.push_back("mode");
    this->setShorthandProperties(shorthandProperties);

    this->mode = HSSRequestModeTypeAuto;
}

HSSRequest::HSSRequest(const HSSRequest & orig)
: HSSAction(orig)
{
    this->observedSrc
            = this->observedTarget
            = NULL;

    this->mode = orig.mode;
}

HSSRequest::p HSSRequest::clone() const
{
    axr_log(AXR_DEBUG_CH_GENERAL_SPECIFIC, "HSSRequest: cloning request object");
    return boost::static_pointer_cast<HSSRequest, HSSClonable > (this->cloneImpl());
}

HSSClonable::p HSSRequest::cloneImpl() const
{
    return HSSClonable::p(new HSSRequest(*this));
}

HSSRequest::~HSSRequest()
{
    axr_log(AXR_DEBUG_CH_GENERAL_SPECIFIC, "HSSRequest: destructing request object");
}

std::string HSSRequest::toString()
{
    return "HSSRequest";
}

std::string HSSRequest::defaultObjectType()
{
    return "request";
}

void HSSRequest::setProperty(HSSObservableProperty name, HSSParserNode::p value)
{
    switch (name)
    {
    case HSSObservablePropertySrc:
        this->setDSrc(value);
        break;

    case HSSObservablePropertyTarget:
        this->setDTarget(value);
        break;

        //        case HSSObservablePropertyMode:
        //            this->setDMode(value);
        //            break;
        //
        //        case HSSObservablePropertyContentTarget:
        //            this->setDContentTarget(value);

    default:
        HSSAction::setProperty(name, value);
        break;
    }
}

void HSSRequest::fire()
{
    //if there is no target
    if (this->target.size() == 0)
    {
        AXRCore::tp & core = AXRCore::getInstance();
        AXRWrapper * wrapper = core->getWrapper();
        wrapper->loadXMLFile(core->getFile()->getBasePath() + "/" + this->src);
    }
    else
    {
        switch (this->mode)
        {
        default:
        {
            AXRCore::tp & core = AXRCore::getInstance();
            AXRWrapper * wrapper = core->getWrapper();
            AXRController::p controller = AXRController::p(new AXRController());
            XMLParser::p xmlParser(new XMLParser(controller.get()));
            HSSParser::p hssParser(new HSSParser(controller.get(), wrapper));
            AXRFile::p baseFile = core->getFile();
            AXRFile::p newFile;
            try
            {
                newFile = wrapper->getFile("file://" + baseFile->getBasePath() + "/" + this->src);
            }
            catch (AXRError::p e)
            {
                e->raise();
            }

            if (newFile)
            {
                bool loadingSuccess = xmlParser->loadFile(newFile);
                if (!loadingSuccess)
                {
                    AXRError::p(new AXRError("AXRCore", "Could not load the XML file"))->raise();
                }
                else
                {
                    HSSContainer::p root = boost::static_pointer_cast<HSSContainer > (controller->getRoot());

                    if (root)
                    {
                        std::vector<std::string> loadSheets = controller->loadSheetsGet();
                        std::vector<std::string>::iterator sheetsIt;
                        for (sheetsIt = loadSheets.begin(); sheetsIt != loadSheets.end(); ++sheetsIt)
                        {
                            std::string hssfilepath;
                            std::string hssfilename = *sheetsIt;
                            if (hssfilename.substr(0, 7) == "file://")
                            {
                                hssfilepath = hssfilename;
                            }
                            else if (hssfilename.substr(0, 7) == "http://")
                            {
                                AXRError::p(new AXRError("AXRCore", "HTTP has not been implemented yet"))->raise();
                            }
                            else if (hssfilename.substr(0, 1) == "/")
                            {
                                hssfilepath = newFile->getBasePath() + hssfilename;
                            }
                            else
                            {
                                hssfilepath = "file://" + newFile->getBasePath() + "/" + hssfilename;
                            }
                            AXRFile::p hssfile;
                            try
                            {
                                hssfile = wrapper->getFile(hssfilepath);
                            }
                            catch (AXRError::p e)
                            {
                                e->raise();
                                continue;
                            }

                            hssParser->setBasePath(newFile->getBasePath());
                            if (!hssParser->loadFile(hssfile))
                            {
                                AXRError::p(new AXRError("AXRCore", "Could not load the HSS file"))->raise();
                            }
                        }

                        controller->matchRulesToContentTree();

                        std::vector<HSSDisplayObject::p>::const_iterator targetIt, childIt;
                        for (targetIt = this->target.begin(); targetIt != this->target.end(); ++targetIt)
                        {
                            const HSSDisplayObject::p & theDO = *targetIt;
                            HSSContainer::p theContainer = HSSContainer::asContainer(theDO);
                            if (theContainer)
                            {
                                theContainer->clear();
                                for (childIt = root->getChildren().begin(); childIt != root->getChildren().end(); ++childIt)
                                {
                                    const HSSDisplayObject::p & theChild = *childIt;
                                    theContainer->add(theChild);
                                }
                            }
                        }

                        root->setNeedsRereadRules(true);
                        root->recursiveReadDefinitionObjects();
                        root->handleEvent(HSSEventTypeLoad, NULL);
                        wrapper->setNeedsDisplay(true);
                    }
                }
            }

            //                AXRCore::tp core = AXRCore::getInstance();
            //                AXRWrapper * wrapper = core->getWrapper();
            //                AXRFile::p baseFile = core->getFile();
            //
            //                bool loadingSuccess = wrapper->loadFile(baseFile->basePath+this->src, this->src);
            //                if(loadingSuccess){
            //                    unsigned i, size;
            //                    for (i=0, size=this->target.size(); i<size; i++) {
            //                        std_log1("Adding loaded file to target");
            //
            //                        if(this->target[i]->isA(HSSObjectTypeContainer)){
            //                            const HSSContainer::p & theCont = boost::static_pointer_cast<HSSContainer>(this->target[i]);
            //                            const HSSContainer::p & loadedRoot = fileController.getRoot();
            //                            theCont->add(loadedRoot);
            //
            //                            unsigned j, k, size2, size3;
            //                            const std::vector<HSSDisplayObject::p> &scope = theCont->getChildren();
            //                            for (j=0, size2=fileController.rulesSize(); j<size2; j++) {
            //                                HSSRule::p & theRule = fileController.rulesGet(j);
            //                                theRule->childrenAdd(theRule);
            //                            }
            //                            for (j=0, size2=theCont->rulesSize(); j<size2; j++) {
            //                                HSSRule::p theRule = theCont->rulesGet(j);
            //                                for (k=0, size3=theRule->childrenSize(); k<size3; k++) {
            //                                    HSSRule::p childRule = theRule->childrenGet(k);
            //                                    this->axrController->recursiveMatchRulesToDisplayObjects(childRule, scope, theCont);
            //                                }
            //                            }
            //
            //                            theCont->recursiveReadDefinitionObjects();
            //                            theCont->setNeedsLayout(true);
            //                        }
            //                    }
            //                }
            break;
        }
        }
    }
}

HSSParserNode::p HSSRequest::getDSrc()
{
    return this->dSrc;
}

void HSSRequest::setDSrc(HSSParserNode::p value)
{
    switch (value->getType())
    {
    case HSSParserNodeTypeKeywordConstant:
    case HSSParserNodeTypeFunctionCall:
    case HSSParserNodeTypeStringConstant:
    {
        this->dSrc = value;
        if (this->observedSrc != NULL)
        {
            this->observedSrc->removeObserver(this->observedSrcProperty, HSSObservablePropertySrc, this);
        }

        switch (value->getType())
        {
        case HSSParserNodeTypeKeywordConstant:
            /**
             *  @todo what here?
             */
            break;

        case HSSParserNodeTypeStringConstant:
        {
            HSSStringConstant::p theString = boost::static_pointer_cast<HSSStringConstant > (value);
            this->src = theString->getValue();
            break;
        }

        case HSSParserNodeTypeFunctionCall:
        {
            HSSFunction::p fnct = boost::static_pointer_cast<HSSFunction > (value)->clone();
            fnct->setScope(this->scope);
            fnct->setThisObj(this->getThisObj());
            boost::any remoteValue = fnct->evaluate();
            try
            {
                this->src = boost::any_cast<std::string > (remoteValue);
            }
            catch (boost::bad_any_cast & e)
            {
                //do nothing
            }

            break;
        }

        default:
            break;
        }



        this->notifyObservers(HSSObservablePropertySrc, &this->src);

        break;
    }

    default:
        throw AXRWarning::p(new AXRWarning("HSSRequest", "Invalid value for src of " + this->name));
    }
}

void HSSRequest::srcChanged(AXR::HSSObservableProperty source, void *data)
{
    AXRWarning::p(new AXRWarning("HSSRequest", "unimplemented"))->raise();
}

HSSParserNode::p HSSRequest::getDTarget()
{
    return this->dTarget;
}

void HSSRequest::setDTarget(HSSParserNode::p value)
{
    switch (value->getType())
    {
    case HSSParserNodeTypeKeywordConstant:
    case HSSParserNodeTypeFunctionCall:
    {
        this->dTarget = value;
        if (this->observedTarget != NULL)
        {
            this->observedTarget->removeObserver(this->observedTargetProperty, HSSObservablePropertyTarget, this);
        }

        switch (value->getType())
        {
        case HSSParserNodeTypeKeywordConstant:
            /**
             *  @todo what here?
             */
            break;

        case HSSParserNodeTypeFunctionCall:
        {
            HSSFunction::p fnct = boost::static_pointer_cast<HSSFunction > (value)->clone();
            if (fnct)
            {
                fnct->setScope(this->scope);
                fnct->setThisObj(this->getThisObj());
                boost::any remoteValue = fnct->evaluate();
                try
                {
                    std::vector< std::vector<HSSDisplayObject::p> > selection = boost::any_cast< std::vector< std::vector<HSSDisplayObject::p> > >(remoteValue);
                    unsigned i, size;
                    this->target.clear();
                    for (i = 0, size = selection.size(); i < size; i++)
                    {
                        std::vector<HSSDisplayObject::p> inner = selection[i];
                        this->target.insert(this->target.end(), inner.begin(), inner.end());
                    }
                }
                catch (boost::bad_any_cast & e)
                {
                    //do nothing
                }


                /**
                 *  @todo potentially leaking
                 */
                fnct->observe(HSSObservablePropertyValue, HSSObservablePropertyTarget, this, new HSSValueChangedCallback<HSSRequest > (this, &HSSRequest::targetChanged));
                this->observedTarget = fnct.get();
                this->observedTargetProperty = HSSObservablePropertyValue;
            }

            break;
        }

        default:
            break;
        }

        this->notifyObservers(HSSObservablePropertySrc, &this->src);

        break;
    }

    default:
        throw AXRWarning::p(new AXRWarning("HSSRequest", "Invalid value for src of " + this->name));
    }
}

void HSSRequest::targetChanged(AXR::HSSObservableProperty source, void *data)
{
    AXRWarning::p(new AXRWarning("HSSRequest", "unimplemented"))->raise();
}