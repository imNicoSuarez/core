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

#include <boost/algorithm/string.hpp>
#include <pango/pangocairo.h>
#include "AXRController.h"
#include "AXRDebugging.h"
#include "HSSFunctions.h"
#include "HSSObjectDefinition.h"
#include "HSSSelectorChain.h"
#include "HSSTextBlock.h"
#include "HSSThisSelector.h"
#include "HSSValue.h"

using namespace AXR;

PangoWeight _pangoWeightFromKeyword(std::string keyword)
{
    if (keyword == "normal")
        return PANGO_WEIGHT_NORMAL;
    if (keyword == "bold")
        return PANGO_WEIGHT_BOLD;
    if (keyword == "medium")
        return PANGO_WEIGHT_MEDIUM;
    if (keyword == "thin")
        return PANGO_WEIGHT_THIN;
    if (keyword == "light")
        return PANGO_WEIGHT_LIGHT;
    if (keyword == "book")
        return PANGO_WEIGHT_BOOK;
    if (keyword == "heavy")
        return PANGO_WEIGHT_HEAVY;
    if (keyword == "ultralight")
        return PANGO_WEIGHT_ULTRALIGHT;
    if (keyword == "semibold")
        return PANGO_WEIGHT_SEMIBOLD;
    if (keyword == "ultrabold")
        return PANGO_WEIGHT_ULTRABOLD;
    if (keyword == "ultraheavy")
        return PANGO_WEIGHT_ULTRAHEAVY;

    return PANGO_WEIGHT_NORMAL;
}

HSSTextTransformType HSSTextBlock::textTransformTypeFromString(std::string value)
{
    static boost::unordered_map<std::string, HSSTextTransformType>types;
    if (types.empty())
    {
        types["lowercase"] = HSSTextTransformTypeLowercase;
        types["uppercase"] = HSSTextTransformTypeUppercase;
        types["capitalize"] = HSSTextTransformTypeCapitalize;
        types["humanize"] = HSSTextTransformTypeHumanize;
    }

    HSSTextTransformType type = HSSTextTransformTypeNone;
    if (types.find(value) != types.end())
    {
        type = types[value];
    }
    return type;
}

HSSTextAlignType HSSTextBlock::textAlignTypeFromString(std::string value)
{
    static boost::unordered_map<std::string, HSSTextAlignType>types;
    if (types.empty())
    {
        types["left"] = HSSTextAlignTypeLeft;
        types["right"] = HSSTextAlignTypeRight;
        types["center"] = HSSTextAlignTypeCenter;
        types["justify"] = HSSTextAlignTypeJustify;
    }

    HSSTextAlignType type = HSSTextAlignTypeNone;
    if (types.find(value) != types.end())
    {
        type = types[value];
    }
    return type;
}

HSSTextBlock::HSSTextBlock()
: HSSDisplayObject(HSSObjectTypeTextBlock)
{
    axr_log(AXR_DEBUG_CH_GENERAL_SPECIFIC, "HSSTextBlock: creating text block object");

    this->observedTextAlign = this->observedTransform = this->observedText
            = NULL;

    this->registerProperty(HSSObservablePropertyText, & this->text);
    this->registerProperty(HSSObservablePropertyTransform, & this->transform);
    this->registerProperty(HSSObservablePropertyTextAlign, & this->textAlign);
    //    this->registerProperty(HSSObservablePropertyDirectionPrimary, &this->directionPrimary);
    //    this->registerProperty(HSSObservablePropertyDirectionSecondary, &this->directionSecondary);

    std::vector<std::string> shorthandProperties;
    shorthandProperties.push_back("text");
    shorthandProperties.push_back("textAlign");
    shorthandProperties.push_back("transform");
    shorthandProperties.push_back("direction");
    shorthandProperties.push_back("wrapDirection");
    this->setShorthandProperties(shorthandProperties);

    PangoFontMap * fontMap = pango_cairo_font_map_new();
    this->_layout = pango_layout_new(pango_font_map_create_context(fontMap));
}

HSSTextBlock::HSSTextBlock(const HSSTextBlock & orig)
: HSSDisplayObject(orig)
{
    this->observedTextAlign = this->observedTransform = this->observedText
            = NULL;

    this->registerProperty(HSSObservablePropertyText, & this->text);
    this->registerProperty(HSSObservablePropertyTransform, & this->transform);
    this->registerProperty(HSSObservablePropertyTextAlign, & this->textAlign);
    //    this->registerProperty(HSSObservablePropertyDirectionPrimary, &this->directionPrimary);
    //    this->registerProperty(HSSObservablePropertyDirectionSecondary, &this->directionSecondary);

    std::vector<std::string> shorthandProperties;
    shorthandProperties.push_back("text");
    shorthandProperties.push_back("textAlign");
    shorthandProperties.push_back("transform");
    shorthandProperties.push_back("direction");
    shorthandProperties.push_back("wrapDirection");
    this->setShorthandProperties(shorthandProperties);

    PangoFontMap * fontMap = pango_cairo_font_map_new();
    this->_layout = pango_layout_new(pango_font_map_create_context(fontMap));
}

HSSTextBlock::p HSSTextBlock::clone() const
{
    axr_log(AXR_DEBUG_CH_GENERAL_SPECIFIC, "HSSTextBlock: cloning text block object");
    return boost::static_pointer_cast<HSSTextBlock, HSSClonable > (this->cloneImpl());
}

HSSClonable::p HSSTextBlock::cloneImpl() const
{
    return HSSClonable::p(new HSSTextBlock(*this));
}

HSSTextBlock::~HSSTextBlock()
{
    axr_log(AXR_DEBUG_CH_GENERAL_SPECIFIC, "HSSTextBlock: destructing text block object");
    g_object_unref(this->_layout);
}

std::string HSSTextBlock::defaultObjectType()
{
    return "textBlock";
}

bool HSSTextBlock::isKeyword(std::string value, std::string property)
{
    if (property == "transform")
    {
        if (value == "lowercase"
                || value == "uppercase"
                || value == "capitalize"
                || value == "humanize")
        {
            return true;
        }
    }
    else if (property == "textAlign")
    {
        if (value == "left"
                || value == "right"
                || value == "center"
                || value == "justify")
        {
            return true;
        }
    }
    else if (property == "direction" || property == "wrapDirection")
    {
        if (value == "ltr" || value == "rtl" || value == "ttb" || value == "btt")
        {
            return true;
        }
    }

    //if we reached this far, let the superclass handle it
    return HSSDisplayObject::isKeyword(value, property);
}

std::string HSSTextBlock::toString()
{
    return "Text block with content:\n" + this->text;
}

void HSSTextBlock::setDefaults()
{
    HSSDisplayObject::setDefaults();

    //transform
    HSSKeywordConstant::p newDTransform(new HSSKeywordConstant("none"));
    this->setDTransform(newDTransform);
    //textAlign
    HSSKeywordConstant::p newDTextAlign(new HSSKeywordConstant("inherit"));
    this->setDTextAlign(newDTextAlign);
    //font
    HSSKeywordConstant::p newDFont(new HSSKeywordConstant("inherit"));
    this->setDFont(newDFont);
    //height
    this->setDHeight(HSSKeywordConstant::p(new HSSKeywordConstant("content")));
}

void HSSTextBlock::setProperty(HSSObservableProperty name, HSSParserNode::p value)
{
    switch (name)
    {
    case HSSObservablePropertyTransform:
        this->setDTransform(value);
        break;
    case HSSObservablePropertyTextAlign:
        this->setDTransform(value);
        break;
        //        case HSSObservablePropertyDirectionPrimary:
        //            this->setDDirectionPrimary(value);
        //            break;
        //        case HSSObservablePropertyDirectionSecondary:
        //            this->setDDirectionSecondary(value);
        //            break;
    case HSSObservablePropertyText:
        this->setDText(value);
        break;
    default:
        HSSDisplayObject::setProperty(name, value);
        break;
    }
}

void HSSTextBlock::drawForeground()
{
    cairo_t * cairo = cairo_create(this->foregroundSurface);
    if (this->font.size() > 0)
    {
        HSSFont::p theFont = *this->font.begin();

        if (theFont->getColor())
        {
            HSSRgb::p textColor = boost::static_pointer_cast<HSSRgb > (theFont->getColor());
            cairo_set_source_rgb(cairo, textColor->getRed() / 255, textColor->getGreen() / 255, textColor->getBlue() / 255);
        }
        else
        {
            cairo_set_source_rgb(cairo, 0, 0, 0);
        }

    }
    else
    {
        cairo_set_source_rgb(cairo, 0, 0, 0);
    }
    pango_cairo_update_layout(cairo, this->_layout);
    cairo_move_to(cairo, 0, 0);
    pango_cairo_show_layout(cairo, this->_layout);
    cairo_destroy(cairo);
}

void HSSTextBlock::layout()
{
    this->_needsLayout = false;

    pango_layout_set_width(this->_layout, this->width * PANGO_SCALE);

    PangoFontDescription *font_description = pango_font_description_new();

    // Get the first font available
    HSSFont::p theFont;
    if (font.size() > 0)
        theFont = *font.begin();

    // Set the font family - be sure it's not empty or Pango will crash in pango_layout_get_extents
    if (theFont && !theFont->getFace().empty())
        pango_font_description_set_family(font_description, theFont->getFace().c_str());
    else
        pango_font_description_set_family(font_description, "monospace");

    // Set the weight of the font (bold, italic, etc.) if available
    if (theFont && theFont->getWeight())
        pango_font_description_set_weight(font_description, _pangoWeightFromKeyword(theFont->getWeight()->getValue()));
    else
        pango_font_description_set_weight(font_description, PANGO_WEIGHT_NORMAL);

    pango_font_description_set_absolute_size(font_description, (theFont ? theFont->getSize() : HSSFont::DEFAULT_SIZE) * PANGO_SCALE);

    switch (textAlign)
    {
    case HSSTextAlignTypeLeft:
        pango_layout_set_alignment(this->_layout, PANGO_ALIGN_LEFT);
        break;
    case HSSTextAlignTypeRight:
        pango_layout_set_alignment(this->_layout, PANGO_ALIGN_RIGHT);
        break;
    case HSSTextAlignTypeCenter:
        pango_layout_set_alignment(this->_layout, PANGO_ALIGN_CENTER);
        break;
    case HSSTextAlignTypeJustify:
        pango_layout_set_alignment(this->_layout, PANGO_ALIGN_LEFT);
        pango_layout_set_justify(this->_layout, true);
        break;
    default:
        break;
    }

    pango_layout_set_font_description(this->_layout, font_description);
    pango_font_description_free(font_description);
    pango_layout_set_text(this->_layout, this->getText().c_str(), -1);
    PangoRectangle rect;
    pango_layout_get_extents(this->_layout, NULL, &rect);
    this->height = rect.height / PANGO_SCALE;
    this->_setInnerHeight();
    this->_setOuterHeight();
    this->notifyObservers(HSSObservablePropertyHeight, &this->height);
    this->setNeedsSurface(true);
}

HSSTextTransformType HSSTextBlock::getTransform()
{
    return this->transform;
}

HSSParserNode::p HSSTextBlock::getDTransform()
{
    return this->dTransform;
}

void HSSTextBlock::setDTransform(HSSParserNode::p value)
{
    bool valid = false;

    switch (value->getType())
    {
    case HSSParserNodeTypeObjectNameConstant:
    {
        this->dTransform = value;
        try
        {
            HSSObjectNameConstant::p objname = boost::static_pointer_cast<HSSObjectNameConstant > (value);
            HSSObjectDefinition::p objdef = this->axrController->objectTreeGet(objname->getValue());
            this->setDTransform(objdef);
            valid = true;

        }
        catch (const AXRError::p &e)
        {
            e->raise();
        }

        break;
    }


    case HSSParserNodeTypeFunctionCall:
    {
        this->dTransform = value;
        HSSFunction::p fnct = boost::static_pointer_cast<HSSFunction > (value)->clone();
        if (fnct && fnct->isA(HSSFunctionTypeRef))
        {
            fnct->setScope(this->scope);
            fnct->setThisObj(this->getThisObj());
            boost::any remoteValue = fnct->evaluate();
            try
            {
                this->transform = boost::any_cast<HSSTextTransformType > (remoteValue);
                valid = true;
            }
            catch (boost::bad_any_cast & e)
            {
                //do nothing
            }

            fnct->observe(HSSObservablePropertyValue, HSSObservablePropertyTransform, this, new HSSValueChangedCallback<HSSTextBlock > (this, &HSSTextBlock::transformChanged));
        }

        break;
    }

    case HSSParserNodeTypeKeywordConstant:
    {
        this->dTransform = value;
        this->transform = HSSTextBlock::textTransformTypeFromString(boost::static_pointer_cast<HSSKeywordConstant > (value)->getValue());
        valid = true;
        break;
    }

    default:
        break;
    }

    switch (value->getStatementType())
    {
    case HSSStatementTypeObjectDefinition:
    {
        this->dTransform = value;
        HSSObjectDefinition::p objdef = boost::static_pointer_cast<HSSObjectDefinition > (value);
        objdef->setScope(this->scope);
        objdef->setThisObj(this->getParent());
        objdef->apply();
        HSSObject::p theobj = objdef->getObject();
        if (theobj && theobj->isA(HSSObjectTypeValue))
        {
            //this->transform = HSSTextBlock::textTransformTypeFromString(boost::static_pointer_cast<HSSValue>(theobj)->getStringValue());
            std_log("######## FIXME ################");
            valid = true;
        }

        break;
    }
    default:
        break;
    }

    if (!valid)
        throw AXRWarning::p(new AXRWarning("HSSDGradient", "Invalid value for transform of " + this->name));

    this->notifyObservers(HSSObservablePropertyTransform, &this->transform);
    this->notifyObservers(HSSObservablePropertyValue, NULL);
}

void HSSTextBlock::transformChanged(HSSObservableProperty source, void *data)
{
    switch (this->dTransform->getType())
    {
    case HSSParserNodeTypeFunctionCall:
        this->transform = *(HSSTextTransformType *) data;
        break;
    default:
        break;
    }

    switch (this->dTransform->getStatementType())
    {
    case HSSStatementTypeObjectDefinition:
        this->transform = *(HSSTextTransformType *) data;
        break;
    default:
        break;
    }

    this->notifyObservers(HSSObservablePropertyTransform, data);
    this->notifyObservers(HSSObservablePropertyValue, NULL);
}

HSSTextAlignType HSSTextBlock::getTextAlign()
{
    return this->textAlign;
}

HSSParserNode::p HSSTextBlock::getDTextAlign()
{
    return this->dTextAlign;
}

void HSSTextBlock::setDTextAlign(HSSParserNode::p value)
{
    bool valid = false;

    switch (value->getType())
    {
    case HSSParserNodeTypeObjectNameConstant:
    {
        this->dTextAlign = value;
        try
        {
            HSSObjectNameConstant::p objname = boost::static_pointer_cast<HSSObjectNameConstant > (value);
            HSSObjectDefinition::p objdef = this->axrController->objectTreeGet(objname->getValue());
            this->setDTextAlign(objdef);
            valid = true;
        }
        catch (const AXRError::p &e)
        {
            e->raise();
        }

        break;
    }


    case HSSParserNodeTypeFunctionCall:
    {
        this->dTextAlign = value;
        HSSFunction::p fnct = boost::static_pointer_cast<HSSFunction > (value)->clone();
        if (fnct && fnct->isA(HSSFunctionTypeRef))
        {
            fnct->setScope(this->scope);
            fnct->setThisObj(this->getThisObj());
            boost::any remoteValue = fnct->evaluate();
            try
            {
                this->textAlign = boost::any_cast<HSSTextAlignType > (remoteValue);
                valid = true;
            }
            catch (boost::bad_any_cast & e)
            {
                //do nothing
            }

            fnct->observe(HSSObservablePropertyValue, HSSObservablePropertyTextAlign, this, new HSSValueChangedCallback<HSSTextBlock > (this, &HSSTextBlock::textAlignChanged));
        }

        break;
    }

    case HSSParserNodeTypeKeywordConstant:
    {
        this->dTextAlign = value;
        std::string kwValue = boost::static_pointer_cast<HSSKeywordConstant > (value)->getValue();
        if (kwValue == "inherit")
        {
            if (this->observedTextAlign)
            {
                this->observedTextAlign->removeObserver(this->observedTextAlignProperty, HSSObservablePropertyTextAlign, this);
            }
            HSSContainer::p parent = this->getParent();
            boost::any remoteValue = parent->getProperty(HSSObservablePropertyTextAlign);
            try {
                this->textAlign = * boost::any_cast<HSSTextAlignType *>(remoteValue);
                parent->observe(HSSObservablePropertyTextAlign, HSSObservablePropertyTextAlign, this, new HSSValueChangedCallback<HSSTextBlock > (this, &HSSTextBlock::textAlignChanged));
                valid = true;
            } catch (boost::bad_any_cast & e) {
                //do nothing
            }
        }
        else
        {
            this->textAlign = HSSTextBlock::textAlignTypeFromString(kwValue);
            valid = true;
        }
        break;
    }

    default:
        valid = false;
    }

    switch (value->getStatementType())
    {
    case HSSStatementTypeObjectDefinition:
    {
        this->dTextAlign = value;
        HSSObjectDefinition::p objdef = boost::static_pointer_cast<HSSObjectDefinition > (value);
        objdef->setScope(this->scope);
        objdef->setThisObj(this->getParent());
        objdef->apply();
        HSSObject::p theobj = objdef->getObject();
        if (theobj && theobj->isA(HSSObjectTypeValue))
        {
            //this->textAlign = HSSTextBlock::textAlignTypeFromString(boost::static_pointer_cast<HSSValue>(theobj)->getStringValue());
            std_log("######## FIXME ################");
            valid = true;
        }

        break;
    }

    default:
        break;
    }

    if (!valid)
        throw AXRWarning::p(new AXRWarning("HSSDGradient", "Invalid value for textAlign of " + this->name));

    this->notifyObservers(HSSObservablePropertyTextAlign, &this->textAlign);
    this->notifyObservers(HSSObservablePropertyValue, NULL);
}

void HSSTextBlock::textAlignChanged(HSSObservableProperty source, void *data)
{
    switch (this->dTextAlign->getType())
    {
    case HSSParserNodeTypeFunctionCall:
    case HSSParserNodeTypeKeywordConstant: //assuming 'inherit'
        this->textAlign = *(HSSTextAlignType *) data;
        break;
    default:
        break;
    }

    switch (this->dTextAlign->getStatementType())
    {
    case HSSStatementTypeObjectDefinition:
        this->textAlign = *(HSSTextAlignType *) data;
        break;

    default:
        break;
    }

    this->notifyObservers(HSSObservablePropertyTextAlign, data);
    this->notifyObservers(HSSObservablePropertyValue, NULL);
}

std::string HSSTextBlock::getText()
{
    return this->text;
}

HSSParserNode::p HSSTextBlock::getDText()
{
    return this->dText;
}

void HSSTextBlock::setDText(HSSParserNode::p value)
{
    bool valid = false;

    switch (value->getType())
    {
    case HSSParserNodeTypeStringConstant:
    {
        this->dText = value;
        this->text = boost::static_pointer_cast<HSSStringConstant > (value)->getValue();
        valid = true;
        break;
    }

    case HSSParserNodeTypeObjectNameConstant:
    {
        try
        {
            HSSObjectNameConstant::p objname = boost::static_pointer_cast<HSSObjectNameConstant > (value);
            HSSObjectDefinition::p objdef = this->axrController->objectTreeGet(objname->getValue());
            this->setDText(objdef);
            valid = true;

        }
        catch (const AXRError::p &e)
        {
            e->raise();
        }

        break;
    }


    case HSSParserNodeTypeFunctionCall:
    {
        this->dText = value;
        HSSFunction::p fnct = boost::static_pointer_cast<HSSFunction > (value)->clone();
        if (fnct && fnct->isA(HSSFunctionTypeRef))
        {
            fnct->setScope(this->scope);
            fnct->setThisObj(this->getThisObj());
            boost::any remoteValue = fnct->evaluate();
            try
            {
                this->text = boost::any_cast<std::string > (remoteValue);
                valid = true;
            }
            catch (boost::bad_any_cast & e)
            {
                //do nothing
            }

            fnct->observe(HSSObservablePropertyValue, HSSObservablePropertyText, this, new HSSValueChangedCallback<HSSTextBlock > (this, &HSSTextBlock::textChanged));
        }

        break;
    }

    case HSSParserNodeTypeKeywordConstant:
    {
        this->dText = value;
        HSSKeywordConstant::p kwd = boost::static_pointer_cast<HSSKeywordConstant > (value);
        if (kwd->getValue() == "none")
        {
            this->text = "";
            valid = true;
        }
        break;
    }

    default:
        break;
    }

    switch (value->getStatementType())
    {
    case HSSStatementTypeObjectDefinition:
    {
        this->dText = value;
        HSSObjectDefinition::p objdef = boost::static_pointer_cast<HSSObjectDefinition > (value);
        objdef->setScope(this->scope);
        objdef->setThisObj(this->getParent());
        objdef->apply();
        HSSObject::p theobj = objdef->getObject();
        if (theobj && theobj->isA(HSSObjectTypeValue))
        {
            //this->text = boost::static_pointer_cast<HSSValue>(theobj)->getStringValue();
            std_log("######## FIXME ################");
            valid = true;
        }

        break;
    }

    default:
        break;
    }

    if (!valid)
        throw AXRWarning::p(new AXRWarning("HSSDGradient", "Invalid value for text of " + this->name));

    this->notifyObservers(HSSObservablePropertyText, &this->text);
}

void HSSTextBlock::textChanged(HSSObservableProperty source, void *data)
{
    switch (this->dText->getType())
    {
    case HSSParserNodeTypeFunctionCall:
        this->text = *(std::string *)data;
        break;

    default:
        break;
    }

    switch (this->dText->getStatementType())
    {
    case HSSStatementTypeObjectDefinition:
        this->text = *(std::string *)data;
        break;

    default:
        break;
    }

    this->notifyObservers(HSSObservablePropertyText, data);
    this->notifyObservers(HSSObservablePropertyValue, NULL);
}

void HSSTextBlock::trimContentText()
{
    std::string trmstr = this->text;
    boost::algorithm::trim(trmstr)
            ;
    this->text = trmstr;
}
