//                                               -*- C++ -*-
/**
 *  @file  QuadrantSampling.cxx
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
#include "QuadrantSampling.hxx"
#include "Log.hxx"

using namespace OT;

namespace OTADS
{

/**
 * @class QuadrantSampling
 */

CLASSNAMEINIT(QuadrantSampling);


/* Constructor with parameters */
QuadrantSampling::QuadrantSampling (const SamplingStrategy & samplingStrategy,
                                    const UnsignedInteger quadrantIndex,
                                    const NumericalPoint & quadrantOrientation)

  : SamplingStrategyImplementation(samplingStrategy.getDimension())
  , samplingStrategy_(samplingStrategy)
  , stataIndices_(samplingStrategy.getDimension())
{
  stataIndices_.fill();
  setQuadrantIndex(quadrantIndex);
  setQuadrantOrientation(quadrantOrientation);
}

/* Virtual constructor */
QuadrantSampling * QuadrantSampling::clone() const
{
  return new QuadrantSampling(*this);
}




/* Generate a set of directions */
NumericalSample QuadrantSampling::generate() const
{
  NumericalSample result(samplingStrategy_.generate());
  const UnsignedInteger size = result.getSize();
  const UnsignedInteger strataDimension = stataIndices_.getSize();

  for ( UnsignedInteger i = 0; i < size; ++ i )
  {
    for ( UnsignedInteger j = 0; j < strataDimension; ++ j )
    {
      result[i][stataIndices_[j]] = fabs( result[i][stataIndices_[j]] );
      if ( (1 << j) & quadrantIndex_ )
      {
        result[i][stataIndices_[j]] *= -1.0;
      }
    }
  }

  if ( quadrantOrientation_.getDimension() > 1 )
  {
    for ( UnsignedInteger i = 0; i < size; ++ i )
    {
      NumericalPoint inP(strataDimension);
      for ( UnsignedInteger j = 0; j < strataDimension; ++ j )
      {
        inP[j] = result[i][stataIndices_[j]];
      }
      NumericalPoint rotP(R_ * inP);
      for ( UnsignedInteger j = 0; j < strataDimension; ++ j )
      {
        result[i][stataIndices_[j]] = rotP[j];
      }
    }

  }
  return result;
}


void QuadrantSampling::setQuadrantIndex(const UnsignedInteger quadrantIndex)
{
  UnsignedInteger m = (1 << getStrataIndices().getSize());

  if (quadrantIndex >= m) throw InvalidArgumentException(HERE) << "Quadrant index (" << quadrantIndex << " ) should be < " << m;
  quadrantIndex_ = quadrantIndex;
}

UnsignedInteger QuadrantSampling::getQuadrantIndex() const
{
  return quadrantIndex_;
}

void QuadrantSampling::setQuadrantOrientation(const NumericalPoint & quadrantOrientation)
{
  if ((quadrantOrientation.getDimension() > 0) && (quadrantOrientation.getDimension() != getDimension())) throw InvalidDimensionException(HERE) << "Quadrant orientation dimension (" << quadrantOrientation.getDimension() << " ) should be " << getDimension();
  quadrantOrientation_ = quadrantOrientation;
  updateRotation();
}

NumericalPoint QuadrantSampling::getQuadrantOrientation() const
{
  return quadrantOrientation_;
}

void QuadrantSampling::setStrataIndices(const OT::Indices & strataIndices)
{
  UnsignedInteger m = (1 << strataIndices.getSize());
  if (quadrantIndex_ >= m) throw InvalidArgumentException(HERE) << "Quadrant index (" << quadrantIndex_ << " ) should be < " << m;
  if (strataIndices.getSize() > samplingStrategy_.getDimension()) throw InvalidDimensionException(HERE) << "Strata indices (" << strataIndices.getSize() << ") should be < " << samplingStrategy_.getDimension();
  stataIndices_ = strataIndices;
  updateRotation();
}


OT::Indices QuadrantSampling::getStrataIndices() const
{
  return stataIndices_;
}

/* String converter */
String QuadrantSampling::__repr__() const
{
  OSS oss;
  oss << "class=" << QuadrantSampling::GetClassName()
      << " derived from " << SamplingStrategyImplementation::__repr__();
  return oss;
}


void QuadrantSampling::updateRotation()
{

  if ( quadrantOrientation_.getDimension() > 0 )
  {
    UnsignedInteger p = getStrataIndices().getSize();

    NumericalPoint u(p);
    for ( UnsignedInteger j = 0; j < p; ++ j )
    {
      u[j] = quadrantOrientation_[stataIndices_[j]];
    }

    // Gram-Schmidt algorithm
    NumericalSample f(p, p);
    // f0 = u* / ||u*||
    f[0] = u / u.norm();
    for ( UnsignedInteger k = 1; k < p; ++ k )
    {
      // fk = ek
      f[k][k] = 1.0;
      // fk -= SUM <ek, fi> fi
      for ( UnsignedInteger i = 0; i < k; ++ i )
      {
        f[k] -= dot(f[k], f[i]) * f[i];
      }
      // fk = fk / ||fk||
      f[k] /= NumericalPoint(f[k]).norm();
    }

    // H is the transformation E -> F, columns = fk
    SquareMatrix H(p);
    for ( UnsignedInteger i = 0; i < p; ++ i )
      for ( UnsignedInteger j = 0; j < p; ++ j )
        H(j, i) = f[i][j];

    // P is the transformation G -> F
    SquareMatrix P(p);
    for ( UnsignedInteger j = 0; j < p; ++ j )
    {
      P(0, j) = 1. / sqrt(1.0 * p);
    }
    for ( UnsignedInteger k = 1; k < p; ++ k )
    {
      P(k, k) = (p - k) / sqrt(1.0 * (p - k) * (p - k + 1));
      for ( UnsignedInteger i = 0; i < p; ++ i )
      {
        if ((i < 1) || (i > k))
          P(k, i) -= 1. / sqrt(1.0 * (p - k) * (p - k + 1));
      }
    }

    // R is the tranformation E -> G
    R_ = P * H;


  }
}



}

