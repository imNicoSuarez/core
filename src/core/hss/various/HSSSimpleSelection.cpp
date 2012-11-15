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

#include <QSharedData>
#include "HSSMultipleSelection.h"
#include "HSSSimpleSelection.h"

using namespace AXR;

class HSSSimpleSelection::Data : public QSharedData
{
public:
    Data() { }

    Data(const Data &other)
    : QSharedData(other), items(other.items) { }

    ~Data() { }

    /**
     * 
     */
    std::vector<HSSDisplayObject::p> items;
};

HSSSimpleSelection::p HSSSimpleSelection::null()
{
    HSSSimpleSelection::p ret;
    return ret;
}

HSSSimpleSelection::HSSSimpleSelection()
: HSSSelection(HSSSelectionTypeSimpleSelection)
, d(new Data())
{
}

HSSSimpleSelection::HSSSimpleSelection(iterator a, iterator b)
: HSSSelection(HSSSelectionTypeSimpleSelection)
, d(new Data())
{
    std::vector<HSSDisplayObject::p> newItems(a, b);
    this->d->items = newItems;
}

HSSSimpleSelection::HSSSimpleSelection(const_iterator a, const_iterator b)
: HSSSelection(HSSSelectionTypeSimpleSelection)
, d(new Data())
{
    std::vector<HSSDisplayObject::p> newItems(a, b);
    this->d->items = newItems;
}

HSSSimpleSelection::HSSSimpleSelection(const HSSSimpleSelection &other)
: HSSSelection(other)
, d(other.d)
{
}

HSSSimpleSelection::~HSSSimpleSelection()
{
}

HSSSimpleSelection& HSSSimpleSelection::operator=(const HSSSimpleSelection &other)
{
    d = other.d;
    return *this;
}

void HSSSimpleSelection::add(HSSDisplayObject::p theDO)
{
    this->d->items.push_back(theDO);
}

void HSSSimpleSelection::addSelection(HSSSelection::p item)
{
    if (item->isA(HSSSelectionTypeSimpleSelection))
    {
        HSSSimpleSelection::p simpleSel = qSharedPointerCast<HSSSimpleSelection>(item);
        for (const_iterator it = simpleSel->begin(); it != simpleSel->end(); ++it) {
            this->add(*it);
        }
    }
    else if (item->isA(HSSSelectionTypeMultipleSelection))
    {
        HSSMultipleSelection::p multiSel = qSharedPointerCast<HSSMultipleSelection>(item);
        for (HSSMultipleSelection::iterator it = multiSel->begin(); it != multiSel->end(); it++) {
            const HSSSimpleSelection::p & simpleSel = *it;
            for (const_iterator it2 = simpleSel->begin(); it2 != simpleSel->end(); ++it2) {
                this->add(*it2);
            }
        }
    }
}

std::vector< QSharedPointer<HSSDisplayObject> >::iterator HSSSimpleSelection::begin()
{
    return this->d->items.begin();
}

HSSSimpleSelection::const_iterator HSSSimpleSelection::begin() const
{
    return this->d->items.begin();
}

std::vector< QSharedPointer<HSSDisplayObject> >::iterator HSSSimpleSelection::end()
{
    return this->d->items.end();
}

HSSSimpleSelection::const_iterator HSSSimpleSelection::end() const
{
    return this->d->items.end();
}

unsigned HSSSimpleSelection::size() const
{
    return this->d->items.size();
}

void HSSSimpleSelection::clear()
{
    return this->d->items.clear();
}

bool HSSSimpleSelection::empty() const
{
    return this->d->items.empty();
}

HSSDisplayObject::p HSSSimpleSelection::front() const
{
    return this->d->items.front();
}

HSSDisplayObject::p HSSSimpleSelection::back() const
{
    return this->d->items.back();
}

void HSSSimpleSelection::insert(iterator position, iterator first, iterator last)
{
    this->d->items.insert(position, first, last);
}

HSSSimpleSelection::iterator HSSSimpleSelection::erase(iterator position)
{
    return this->d->items.erase(position);
}

HSSSimpleSelection::iterator HSSSimpleSelection::erase(iterator first, iterator last)
{
    return this->d->items.erase(first, last);
}

std::vector< HSSDisplayObject::p > HSSSimpleSelection::getItems() const
{
    return this->d->items;
}

HSSSimpleSelection::p HSSSimpleSelection::joinAll()
{
    HSSSimpleSelection::p ret(new HSSSimpleSelection());
    for (iterator it=this->d->items.begin(); it!=this->d->items.end(); it++) {
        ret->add(*it);
    }
    return ret;
}

HSSMultipleSelection::p HSSSimpleSelection::splitAll()
{
    HSSMultipleSelection::p ret(new HSSMultipleSelection());
    for (iterator it=this->d->items.begin(); it!=this->d->items.end(); it++) {
        HSSSimpleSelection::p newSel(new HSSSimpleSelection());
        newSel->add(*it);
        ret->add(newSel);
    }
    return ret;
}
