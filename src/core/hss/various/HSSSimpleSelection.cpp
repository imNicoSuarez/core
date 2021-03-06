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

#include <QSharedData>
#include "AXRError.h"
#include "HSSDisplayObject.h"
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
    std::vector<QSharedPointer<HSSDisplayObject> > items;
};

QSharedPointer<HSSSimpleSelection> HSSSimpleSelection::null()
{
    QSharedPointer<HSSSimpleSelection> ret;
    return ret;
}

HSSSimpleSelection::HSSSimpleSelection(AXRController * controller)
: HSSSelection(HSSSelectionTypeSimpleSelection, controller)
, d(new Data())
{
}

HSSSimpleSelection::HSSSimpleSelection(iterator a, iterator b, AXRController * controller)
: HSSSelection(HSSSelectionTypeSimpleSelection, controller)
, d(new Data())
{
    std::vector<QSharedPointer<HSSDisplayObject> > newItems(a, b);
    this->d->items = newItems;
}

HSSSimpleSelection::HSSSimpleSelection(const_iterator a, const_iterator b, AXRController * controller)
: HSSSelection(HSSSelectionTypeSimpleSelection, controller)
, d(new Data())
{
    std::vector<QSharedPointer<HSSDisplayObject> > newItems(a, b);
    this->d->items = newItems;
}

HSSSimpleSelection::HSSSimpleSelection(const HSSSimpleSelection &other)
: HSSSelection(other)
, d(other.d)
{
}

HSSSimpleSelection::HSSSimpleSelection(const std::vector<QWeakPointer<HSSDisplayObject> > &weakVector, AXRController * controller)
: HSSSelection(HSSSelectionTypeSimpleSelection, controller)
, d(new Data())
{
    std::vector<QWeakPointer<HSSDisplayObject> >::const_iterator it;
    for (it=weakVector.begin(); it!=weakVector.end(); ++it) {
        QSharedPointer<HSSDisplayObject> theDO = (*it).toStrongRef();
        this->d->items.push_back(theDO);
    }
}

HSSSimpleSelection::~HSSSimpleSelection()
{
}

HSSSimpleSelection& HSSSimpleSelection::operator=(const HSSSimpleSelection &other)
{
    d = other.d;
    return *this;
}

void HSSSimpleSelection::add(QSharedPointer<HSSDisplayObject> theDO)
{
    if (!theDO)
    {
        AXRError("HSSSimpleSelection", "Adding empty object to selection").raise();
    }
    this->d->items.push_back(theDO);
}

void HSSSimpleSelection::addSelection(QSharedPointer<HSSSelection> item)
{
    if (item->isA(HSSSelectionTypeSimpleSelection))
    {
        QSharedPointer<HSSSimpleSelection> simpleSel = qSharedPointerCast<HSSSimpleSelection>(item);
        for (const_iterator it = simpleSel->begin(); it != simpleSel->end(); ++it) {
            this->add(*it);
        }
    }
    else if (item->isA(HSSSelectionTypeMultipleSelection))
    {
        QSharedPointer<HSSMultipleSelection> multiSel = qSharedPointerCast<HSSMultipleSelection>(item);
        for (HSSMultipleSelection::iterator it = multiSel->begin(); it != multiSel->end(); it++) {
            const QSharedPointer<HSSSimpleSelection> & simpleSel = *it;
            for (const_iterator it2 = simpleSel->begin(); it2 != simpleSel->end(); ++it2) {
                this->add(*it2);
            }
        }
    }
}

HSSSimpleSelection::iterator HSSSimpleSelection::begin()
{
    return this->d->items.begin();
}

HSSSimpleSelection::const_iterator HSSSimpleSelection::begin() const
{
    return this->d->items.begin();
}

HSSSimpleSelection::iterator HSSSimpleSelection::end()
{
    return this->d->items.end();
}

HSSSimpleSelection::const_iterator HSSSimpleSelection::end() const
{
    return this->d->items.end();
}

HSSSimpleSelection::reverse_iterator HSSSimpleSelection::rbegin()
{
    return this->d->items.rbegin();
}

HSSSimpleSelection::const_reverse_iterator HSSSimpleSelection::rbegin() const
{
    return this->d->items.rbegin();
}

HSSSimpleSelection::reverse_iterator HSSSimpleSelection::rend()
{
    return this->d->items.rend();
}

HSSSimpleSelection::const_reverse_iterator HSSSimpleSelection::rend() const
{
    return this->d->items.rend();
}

size_t HSSSimpleSelection::size() const
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

QSharedPointer<HSSDisplayObject> HSSSimpleSelection::front() const
{
    return this->d->items.front();
}

QSharedPointer<HSSDisplayObject> HSSSimpleSelection::back() const
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

std::vector< QSharedPointer<HSSDisplayObject> > HSSSimpleSelection::getItems() const
{
    return this->d->items;
}

QSharedPointer<HSSSimpleSelection> HSSSimpleSelection::joinAll() const
{
    QSharedPointer<HSSSimpleSelection> ret(new HSSSimpleSelection(this->getController()));
    for (const_iterator it=this->d->items.begin(); it!=this->d->items.end(); it++) {
        ret->add(*it);
    }
    return ret;
}

QSharedPointer<HSSMultipleSelection> HSSSimpleSelection::splitAll()
{
    QSharedPointer<HSSMultipleSelection> ret(new HSSMultipleSelection(this->getController()));
    for (iterator it=this->d->items.begin(); it!=this->d->items.end(); it++) {
        QSharedPointer<HSSSimpleSelection> newSel(new HSSSimpleSelection(this->getController()));
        newSel->add(*it);
        ret->add(newSel);
    }
    return ret;
}

AXRString HSSSimpleSelection::toString()
{
    AXRString tempstr = AXRString("HSSSimpleSelection with the following nodes: \n");
    if (this->d->items.size() > 0)
    {
        for (iterator it = this->d->items.begin(); it != this->d->items.end(); ++it)
        {
            tempstr.append("   ").append((*it)->toString());
        }
    }
    else
    {
        tempstr.append("WARNING: selection is empty!\n");
    }
    return tempstr;
}

bool HSSSimpleSelection::equalTo(QSharedPointer<HSSObject> otherObj)
{
    //check wether pointers are the same
    if (this == otherObj.data()) return true;
    //other checks
    if ( ! HSSSelection::equalTo(otherObj)) return false;
    QSharedPointer<HSSSimpleSelection> castedObj = qSharedPointerCast<HSSSimpleSelection>(otherObj);
    if ( this->d->items.size() != castedObj->d->items.size() ) return false;
    std::vector<QSharedPointer<HSSDisplayObject> >::const_iterator it1, it2;
    it2 = castedObj->d->items.begin();
    for (it1=this->d->items.begin(); it1 != this->d->items.end(); ++it1)
    {
        if ( ! (*it1)->equalTo(*it2)) return false;
        ++it2;
    }
    return true;
}
