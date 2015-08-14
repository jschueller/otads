//                                               -*- C++ -*-
/**
 *  @file  QuadrantSampling.hxx
 *  @brief Sampling in standard space quadrants
 *
 *  Copyright 2005-2013 EDF
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTADS_QUADRANTSAMPLING_HXX
#define OTADS_QUADRANTSAMPLING_HXX

#include "SamplingStrategy.hxx"
#include "RandomDirection.hxx"
#include "OTADSprivate.hxx"

namespace OTADS
{

/**
 * @class RandomDirection
 */

class OTADS_API QuadrantSampling :
  public OT::SamplingStrategyImplementation
{

  CLASSNAME;

public:

  /** Default constructor */
  explicit QuadrantSampling(const OT::SamplingStrategy & samplingStrategy = OT::RandomDirection(),
                            const OT::UnsignedInteger quadrantIndex = 0,
                            const OT::NumericalPoint & quadrantOrientation = OT::NumericalPoint());

  /** Virtual constructor */
  virtual QuadrantSampling * clone() const;

  /** Generate a set of directions */
  virtual OT::NumericalSample generate() const;

  /** Quadrant index accessor */
  void setQuadrantIndex(const OT::UnsignedInteger quadrantIndex);
  OT::UnsignedInteger getQuadrantIndex() const;

  /** Quadrant orientation accessor */
  void setQuadrantOrientation(const OT::NumericalPoint & quadrantOrientation);
  OT::NumericalPoint getQuadrantOrientation() const;

  /** Strata indices accessor */
  void setStrataIndices(const OT::Indices & strataIndices);
  OT::Indices getStrataIndices() const;

  /** String converter */
  OT::String __repr__() const;

protected:
  /** Update rotation parameters */
  void updateRotation();

  // underlying generator
  OT::SamplingStrategy samplingStrategy_;

  // quadrant index in [0; 2^d[
  OT::UnsignedInteger quadrantIndex_;

  // first quadrant bisector
  OT::NumericalPoint quadrantOrientation_;

  // rotation matrix
  OT::SquareMatrix R_;

  // stratification indices
  OT::Indices stataIndices_;

} ; /* class QuadrantSampling */

}

#endif /* OTADS_QUADRANTSAMPLING_HXX */
