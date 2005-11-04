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
 **********************************************************************
 *
 * Last port: operation/valid/QuadtreeNestedRingTester.java rev. 1.11
 *
 **********************************************************************/


#include <geos/opValid.h>
#include <stdio.h>
#include <geos/util.h>

namespace geos {

QuadtreeNestedRingTester::QuadtreeNestedRingTester(GeometryGraph *newGraph):
	graph(newGraph),
	rings(new vector<LinearRing*>()),
	totalEnv(new Envelope()),
	qt(NULL),
	nestedPt(NULL)
{
}

QuadtreeNestedRingTester::~QuadtreeNestedRingTester()
{
	delete rings;
	delete totalEnv;
	delete qt;
}

Coordinate *
QuadtreeNestedRingTester::getNestedPoint()
{
	return nestedPt;
}

void
QuadtreeNestedRingTester::add(LinearRing *ring)
{
	rings->push_back(ring);
	const Envelope *envi=ring->getEnvelopeInternal();
	totalEnv->expandToInclude(envi);
}

bool
QuadtreeNestedRingTester::isNonNested()
{
	buildQuadtree();
	for(unsigned int i=0; i<rings->size(); ++i)
	{
		LinearRing *innerRing=(*rings)[i];
		const CoordinateSequence *innerRingPts=innerRing->getCoordinatesRO();
		const Envelope *envi=innerRing->getEnvelopeInternal();

		vector<void*> *results=qt->query(envi);
		for(unsigned int j=0; j<results->size(); ++j)
		{
			LinearRing *searchRing=(LinearRing*)(*results)[j];
			const CoordinateSequence *searchRingPts=searchRing->getCoordinatesRO();

			if (innerRing==searchRing) continue;

			const Envelope *e1=innerRing->getEnvelopeInternal();
			const Envelope *e2=searchRing->getEnvelopeInternal();
			if (!e1->intersects(e2)) continue;

			const Coordinate *innerRingPt=IsValidOp::findPtNotNode(innerRingPts, searchRing, graph);
			Assert::isTrue((innerRingPt!=NULL), "Unable to find a ring point not a node of the search ring");

			bool isInside=CGAlgorithms::isPointInRing(*innerRingPt,searchRingPts);
			if (isInside) {
				/*
				 * innerRingPt is const just because the input
				 * CoordinateSequence is const. If the input
				 * Polygon survives lifetime of this object
				 * we are safe.
				 */
				nestedPt=const_cast<Coordinate *>(innerRingPt);
				delete results;
				return false;
			}
		}
		delete results;
	}
	return true;
}

void
QuadtreeNestedRingTester::buildQuadtree()
{
	qt=new Quadtree();
	for(unsigned int i=0; i<rings->size(); ++i)
	{
		LinearRing *ring=(*rings)[i];
		const Envelope *env=ring->getEnvelopeInternal();
		qt->insert(env,ring);
	}
}

} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.14  2005/11/04 11:04:09  strk
 * Ported revision 1.38 of IsValidOp.java (adding closed Ring checks).
 * Changed NestedRingTester classes to use Coorinate pointers
 * rather then actual objects, to speedup NULL tests.
 * Added JTS port revision when applicable.
 *
 * Revision 1.13  2004/07/27 16:35:47  strk
 * Geometry::getEnvelopeInternal() changed to return a const Envelope *.
 * This should reduce object copies as once computed the envelope of a
 * geometry remains the same.
 *
 * Revision 1.12  2004/07/08 19:34:50  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added DefaultCoordinateSequenceFactory::instance() function.
 *
 * Revision 1.11  2004/07/02 13:28:29  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.10  2004/03/29 06:59:25  ybychkov
 * "noding/snapround" package ported (JTS 1.4);
 * "operation", "operation/valid", "operation/relate" and "operation/overlay" upgraded to JTS 1.4;
 * "geom" partially upgraded.
 *
 * Revision 1.9  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 * Revision 1.8  2003/10/16 08:50:00  strk
 * Memory leak fixes. Improved performance by mean of more 
 * calls to new getCoordinatesRO() when applicable.
 *
 **********************************************************************/

