//                                               -*- C++ -*-
/**
 *  @file  AdaptiveDirectionalSampling.hxx
 *  @brief ADS simulation algorithm
 *
 *  Copyright 2005-2013 EDF
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
#ifndef OTADS_ADAPTATIVEDIRECTIONALSAMPLING_HXX
#define OTADS_ADAPTATIVEDIRECTIONALSAMPLING_HXX

#include "Simulation.hxx"
#include "StorageManager.hxx"
#include "NumericalPoint.hxx"
#include "StandardEvent.hxx"
#include "RootStrategy.hxx"
#include "SamplingStrategy.hxx"
#include "OTADSprivate.hxx"

namespace OTADS
{

/**
 * @class AdaptiveDirectionalSampling
 *
 * AdaptiveDirectionalSampling
 */
class OTADS_API AdaptiveDirectionalSampling
  : public OT::Simulation
{
  CLASSNAME;

public:

  /** Default constructor */
  AdaptiveDirectionalSampling();

  /** Constructor with parameters */
  AdaptiveDirectionalSampling(const OT::Event & event,
                                const OT::RootStrategy & rootStrategy = OT::RootStrategy(),
                                const OT::SamplingStrategy & samplingStrategy = OT::SamplingStrategy());

  /** Virtual constructor */
  virtual AdaptiveDirectionalSampling * clone() const;

  /** String converter */
  OT::String __repr__() const;

  /** Root strategy accessor */
  void setRootStrategy(const OT::RootStrategy & rootStrategy);
  OT::RootStrategy getRootStrategy() const;

  /** Sampling strategy */
  void setSamplingStrategy(const OT::SamplingStrategy & samplingStrategy);
  OT::SamplingStrategy getSamplingStrategy() const;

  /** Gamma accessor */
  void setGamma(const OT::NumericalPoint & gamma);
  OT::NumericalPoint getGamma() const;

  /** Quadrant orientation_ accessor */
  void setQuadrantOrientation(const OT::NumericalPoint & quadrantOrientation);
  OT::NumericalPoint getQuadrantOrientation() const;

  /** Use partial stratification */
  void setPartialStratification(OT::Bool partialStratification);
  OT::Bool getPartialStratification() const;

  /** Set maximum stratification dimension */
  void setMaximumStratificationDimension(OT::UnsignedInteger maximumStratificationDimension);
  OT::UnsignedInteger getMaximumStratificationDimension() const;

  /** T statistic accessor */
  OT::NumericalPoint getTStatistic() const;

  /** Performs the actual computation. */
  virtual void run();

private:
  /** Compute the block sample and the points that realized the event */
  virtual OT::NumericalSample computeBlockSample();

  OT::StandardEvent standardEvent_;

  OT::RootStrategy rootStrategy_;
  OT::SamplingStrategy samplingStrategy_;

  // Repartition of points in learning/simulation steps
  OT::NumericalPoint gamma_;

  // Quadrant orientation
  OT::NumericalPoint quadrantOrientation_;

  // Use partial stratification
  OT::Bool partialStratification_;

  // Maximum number of stratified vars
  OT::UnsignedInteger maximumStratificationDimension_;

  // T statistic
  OT::NumericalPoint T_;

}; /* class AdaptiveDirectionalSampling */

} /* namespace OTADS */

#endif /* OTADS_ADAPTATIVEDIRECTIONALSAMPLING_HXX */
