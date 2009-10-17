/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 * Copyright (C) 2005 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/geom/Envelope.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateFilter.h>

#include <cassert>
#include <algorithm>
#include <vector>

using namespace std;

namespace geos {
namespace geom { // geos::geom

CoordinateArraySequence::CoordinateArraySequence():
	vect(new vector<Coordinate>())
{
}

CoordinateArraySequence::CoordinateArraySequence(size_t n):
	vect(new vector<Coordinate>(n))
{
}

CoordinateArraySequence::CoordinateArraySequence(
	vector<Coordinate> *coords): vect(coords)
{
	if ( ! vect ) vect = new vector<Coordinate>();
}

CoordinateArraySequence::CoordinateArraySequence(
	const CoordinateArraySequence &c)
	:
	CoordinateSequence(c),
	vect(new vector<Coordinate>(*(c.vect)))
{
}

CoordinateSequence *
CoordinateArraySequence::clone() const
{
	return new CoordinateArraySequence(*this);
}

void
CoordinateArraySequence::setPoints(const vector<Coordinate> &v)
{
    assert(0 != vect);
	vect->assign(v.begin(), v.end());
}

const vector<Coordinate>*
CoordinateArraySequence::toVector() const
{
    assert(0 != vect);
	return vect; //new vector<Coordinate>(vect->begin(),vect->end());
}

void
CoordinateArraySequence::toVector(vector<Coordinate>& out) const
{
	assert(0 != vect);
	// TODO: can this be optimized ?
	out.insert(out.end(), vect->begin(), vect->end());
}

bool
CoordinateArraySequence::isEmpty() const
{
    assert(0 != vect);
	return vect->empty();
}

void
CoordinateArraySequence::add(const Coordinate& c)
{
    assert(0 != vect);
	vect->push_back(c);
}

void
CoordinateArraySequence::add(const Coordinate& c, bool allowRepeated)
{
    assert(0 != vect);
	if (!allowRepeated && ! vect->empty() )
	{
		const Coordinate& last=vect->back();
		if (last.equals2D(c)) return;
	}
	vect->push_back(c);
}

/*public*/
void
CoordinateArraySequence::add(size_t i, const Coordinate& coord,
                                       bool allowRepeated)
{
    // don't add duplicate coordinates
    if (! allowRepeated) {
      size_t sz = size();
      if (sz > 0) {
        if (i > 0) {
          const Coordinate& prev = getAt(i - 1);
          if (prev.equals2D(coord)) return;
        }
        if (i < sz) {
          const Coordinate& next = getAt(i);
          if (next.equals2D(coord)) return;
        }
      }
    }

    vect->insert(vect->begin()+i, coord);
}

size_t
CoordinateArraySequence::getSize() const
{
	return vect->size();
}

const Coordinate &
CoordinateArraySequence::getAt(size_t pos) const
{
    assert(0 != vect);
	assert(pos < vect->size());
	return (*vect)[pos];
}

void
CoordinateArraySequence::getAt(size_t pos, Coordinate &c) const
{
    assert(0 != vect);
	assert(pos<vect->size());
	c=(*vect)[pos];
}

void
CoordinateArraySequence::setAt(const Coordinate& c, size_t pos)
{
    assert(0 != vect);
	assert(pos<vect->size());
	(*vect)[pos]=c;
}

void
CoordinateArraySequence::deleteAt(size_t pos)
{
    assert(0 != vect);
	assert(pos<vect->size());
	vect->erase(vect->begin()+pos);
}

string
CoordinateArraySequence::toString() const
{
	string result("(");
	if (getSize()>0) {
		//char buffer[100];
		for (size_t i=0, n=vect->size(); i<n; i++)
		{
			Coordinate& c=(*vect)[i];
			if ( i ) result.append(", ");
			result.append(c.toString());
		}
	}
	result.append(")");

	return result;
}

CoordinateArraySequence::~CoordinateArraySequence()
{
	delete vect;
}

void
CoordinateArraySequence::expandEnvelope(Envelope &env) const
{
    assert(0 != vect);
	size_t size = vect->size();
	for (size_t i=0; i<size; i++) env.expandToInclude((*vect)[i]);
}

double
CoordinateArraySequence::getOrdinate(size_t index, size_t ordinateIndex) const
{
    assert(0 != vect);
	assert(index<vect->size());

	switch (ordinateIndex)
	{
		case CoordinateSequence::X:
			return (*vect)[index].x;
		case CoordinateSequence::Y:
			return (*vect)[index].y;
		case CoordinateSequence::Z:
			return (*vect)[index].z;
		default:
			return DoubleNotANumber;
	}
}

void
CoordinateArraySequence::setOrdinate(size_t index, size_t ordinateIndex,
	double value)
{

    assert(0 != vect);
	assert(index<vect->size());

    assert(ordinateIndex == CoordinateSequence::X
           || ordinateIndex == CoordinateSequence::Y
           || ordinateIndex == CoordinateSequence::Z);

	switch (ordinateIndex)
	{
		case CoordinateSequence::X:
			(*vect)[index].x = value;
			break;
		case CoordinateSequence::Y:
			(*vect)[index].y = value;
			break;
		case CoordinateSequence::Z:
			(*vect)[index].z = value;
			break;
		default:
			break;
	}
}

void
CoordinateArraySequence::apply_rw(const CoordinateFilter *filter)
{
    assert(0 != vect);
	for (vector<Coordinate>::iterator i=vect->begin(), e=vect->end(); i!=e; ++i)
	{
		filter->filter_rw(&(*i));
	}
}

void
CoordinateArraySequence::apply_ro(CoordinateFilter *filter) const
{
    assert(0 != vect);
	for (vector<Coordinate>::const_iterator i=vect->begin(), e=vect->end(); i!=e; ++i)
	{
		filter->filter_ro(&(*i));
	}
}

CoordinateSequence&
CoordinateArraySequence::removeRepeatedPoints()
{
    assert(0 != vect);
	// We use == operator, which is 2D only
	vector<Coordinate>::iterator new_end = \
		std::unique(vect->begin(), vect->end());
	
	vect->erase(new_end, vect->end());

	return *this;
}

} // namespace geos::geom
} //namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.10  2006/06/12 15:07:47  strk
 * explicitly invoked CoordinateSequence (copy) ctor - suggested by GCC warning.
 *
 * Revision 1.9  2006/06/12 10:10:39  strk
 * Fixed getGeometryN() to take size_t rather then int, changed unsigned int parameters to size_t.
 *
 * Revision 1.8  2006/05/03 08:58:34  strk
 * added new non-static CoordinateSequence::removeRepeatedPoints() mutator.
 *
 * Revision 1.7  2006/03/27 09:00:50  strk
 * Bug #79 - Small fix in CoordinateArraySequence::toString()
 *
 * Revision 1.6  2006/03/22 16:58:34  strk
 * Removed (almost) all inclusions of geom.h.
 * Removed obsoleted .cpp files.
 * Fixed a bug in WKTReader not using the provided CoordinateSequence
 * implementation, optimized out some memory allocations.
 *
 **********************************************************************/

