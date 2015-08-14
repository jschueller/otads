//                                               -*- C++ -*-
/**
 *  @file  AdaptiveDirectionalSampling.cxx
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
#include "AdaptiveDirectionalSampling.hxx"
#include "PersistentObjectFactory.hxx"
#include "QuadrantSampling.hxx"
#include "DirectionalSampling.hxx"

using namespace OT;

namespace OTADS
{

CLASSNAMEINIT(AdaptiveDirectionalSampling);

/* Default constructor */
AdaptiveDirectionalSampling::AdaptiveDirectionalSampling()
  : Simulation()
{
  // Nothing to do
}

/* Constructor with parameters */
AdaptiveDirectionalSampling::AdaptiveDirectionalSampling(const Event & event,
    const RootStrategy & rootStrategy,
    const SamplingStrategy & samplingStrategy)
  : Simulation(event)
  , standardEvent_(StandardEvent(event))
  , rootStrategy_(rootStrategy)
  , samplingStrategy_(samplingStrategy)
  , gamma_(NumericalPoint(2, 0.5))
  , partialStratification_(false)
  , maximumStratificationDimension_(3)
{
  samplingStrategy_.setDimension(getEvent().getImplementation()->getAntecedent()->getDimension());
}

/* Virtual constructor */
AdaptiveDirectionalSampling * AdaptiveDirectionalSampling::clone() const
{
  return new AdaptiveDirectionalSampling(*this);
}



void AdaptiveDirectionalSampling::run()
{

  // First, reset the convergence history
  convergenceStrategy_.clear();

  // dimension
  const UnsignedInteger dimension = standardEvent_.getImplementation()->getFunction().getInputDimension();

  // stratification dimension
  UnsignedInteger d = dimension;

  // stratified dimensions
  Indices strataIndices(d);
  strataIndices.fill();

  // number of subdivisions: 2^d
  UnsignedInteger m = (1 << d);

  // number of steps
  const UnsignedInteger L = gamma_.getDimension();

  // initial uniform allocation
  NumericalPoint w(m, 1.0 / m);

  // maximum directions budget
  const UnsignedInteger n0 = getMaximumOuterSampling();
  const UnsignedInteger blockSize = 1;

  // effective number of directions
  UnsignedInteger n = 0;

  // for each step
  for ( UnsignedInteger l = 0; l < L; ++ l )
  {
    const NumericalPoint w0(m, 1.0 / m);

    NumericalScalar probabilityEstimate = 0.0;
    NumericalScalar w0SigmaSum = 0.0;
    NumericalPoint sigma(m, 0.);

    NumericalSample T0(d, m);
    NumericalSample T1(d, m);

    // for each subdivision
    for ( UnsignedInteger i = 0; i < m; ++ i )
    {
      // budget for this subdivision
      const UnsignedInteger ni = static_cast<UnsignedInteger>(gamma_[l] * n0 * w[i]); // (28)
      n += ni;

      QuadrantSampling quadrantSampling ( samplingStrategy_, i );
      quadrantSampling.setQuadrantOrientation( quadrantOrientation_ );
      quadrantSampling.setStrataIndices( strataIndices );
      DirectionalSampling directionalSampling ( getEvent(), rootStrategy_, quadrantSampling );
      directionalSampling.setMaximumOuterSampling ( ni );
      directionalSampling.setBlockSize ( blockSize );
      directionalSampling.run();
      const SimulationResult result(directionalSampling.getResult());
      const NumericalScalar pf = result.getProbabilityEstimate();

      if ( pf > 0. )
      {
        probabilityEstimate += w0[i] * pf;
        sigma[i] = result.getStandardDeviation();
        w0SigmaSum += w0[i] * sigma[i];

        for ( UnsignedInteger k = 0; k < d; ++ k )
        {
          if ( (1 << k) & i )
            T0[k][i] = pf;
          else
            T1[k][i] = pf;
        }

      } // if pf > 0
      Log::Debug(OSS() << "AdaptiveDirectionalSampling::run n=" << n << " i=" << i << " ni=" << ni << " pf=" << pf << " sigma=" << sigma[i]);

    } // for i

    const NumericalScalar varianceEstimate = w0SigmaSum * w0SigmaSum / (gamma_[l] * n); // (33)

    // update result
    setResult(SimulationResult(getEvent(), probabilityEstimate, varianceEstimate, n, blockSize));

    // update weights
    for ( UnsignedInteger i = 0; i < m; ++ i )
    {
      w[i] = (w0SigmaSum > 0.) ? w0[i] * sigma[i] / w0SigmaSum : 0.; // (29)
    }

    if ( (l == 0) && partialStratification_ )
    {
      T_ = NumericalPoint(dimension);
      for ( UnsignedInteger k = 0; k < dimension; ++ k )
      {
        for ( UnsignedInteger i = 0; i < m; ++ i )
          if ( (1 << k) & i)
            T_[k] += fabs(T0[k][i] - T1[k][i]);

        Log::Debug(OSS() << "AdaptiveDirectionalSampling::run T[" << k << "]=" << T_[k]);
      }

      // sort variables according to T statistic
      Indices order(dimension);
      order.fill();
      for ( SignedInteger i = dimension - 1; i >= 0; -- i )
        for ( SignedInteger j = 0; j < i; ++ j )
          if ( T_[order[j]] < T_[order[j + 1]] )
          {
            UnsignedInteger swap = order[j];
            order[j] = order[j + 1];
            order[j + 1] = swap;
          }
      for ( UnsignedInteger k = 0; k < dimension; ++ k )
        Log::Debug(OSS() << "AdaptiveDirectionalSampling::run #" << k << " T[" << order[k] << "]=" << T_[order[k]] );

      // retrieve the p' variables contributing the most
      strataIndices = Indices();
      for ( UnsignedInteger k = 0; k < dimension; ++ k )
      {
        if (k < maximumStratificationDimension_)
          strataIndices.add( order[k] );
      }

      // new stratification dimension d' < d
      UnsignedInteger d2 = strataIndices.getSize();
      UnsignedInteger m2 = 1 << d2;

      // compute new weights using existing simulations
      NumericalPoint w2(m2, 0.);
      for ( UnsignedInteger i2 = 0; i2 < m2; ++ i2 )
      {
        for ( UnsignedInteger i = 0; i < m; ++ i )
        {
          UnsignedInteger sum = 0;
          for ( UnsignedInteger k = 0; k < d2; ++ k )
          {
            const UnsignedInteger bit = 1 << strataIndices[k];
            if ( bit & i )
              sum += 1 << k;
          }
          if ( sum == i2 )
          {
            w2[i2] += w[i];
          }
        }
      }

      // update some parameters
      m = m2;
      w = w2;
      d = d2;


    } // if partialStratification_




  } // for L

}

/* Root strategy accessor */
void AdaptiveDirectionalSampling::setRootStrategy(const RootStrategy & rootStrategy)
{
  rootStrategy_ = rootStrategy;
}

RootStrategy AdaptiveDirectionalSampling::getRootStrategy() const
{
  return rootStrategy_;
}

/* Sampling strategy */
void AdaptiveDirectionalSampling::setSamplingStrategy(const SamplingStrategy & samplingStrategy)
{
  UnsignedInteger dimension = getEvent().getImplementation()->getAntecedent()->getDistribution().getDimension();
  if (samplingStrategy.getDimension() != dimension)
    throw InvalidDimensionException(HERE) << "Error: the sampling strategy dimension (" << samplingStrategy.getDimension() << ") is not compatible with the antecedent dimension (" << dimension << ")";
  samplingStrategy_ = samplingStrategy;
}

SamplingStrategy AdaptiveDirectionalSampling::getSamplingStrategy() const
{
  return samplingStrategy_;
}

void AdaptiveDirectionalSampling::setGamma(const NumericalPoint& gamma)
{
  UnsignedInteger dimension = gamma.getDimension();
  if (dimension > 2) throw InvalidDimensionException(HERE) << "gamma dimension is " << dimension;
  NumericalScalar sum = 0.;
  for ( UnsignedInteger i = 0; i < dimension; ++ i )
  {
    if ( gamma[i] <= 0. ) throw InvalidArgumentException(HERE) << "gamma values should be positive";
    sum += gamma[i];
  }
  if ( fabs(sum - 1.0) > 1e-6 ) throw InvalidArgumentException(HERE) << "gamma components do not sum to 1";
  gamma_ = gamma;
}

NumericalPoint AdaptiveDirectionalSampling::getGamma() const
{
  return gamma_;
}


void AdaptiveDirectionalSampling::setQuadrantOrientation(const OT::NumericalPoint& quadrantOrientation)
{
  UnsignedInteger dimension = getEvent().getImplementation()->getAntecedent()->getDimension();
  if ((quadrantOrientation.getDimension() > 0) && (quadrantOrientation.getDimension() != dimension))
    throw InvalidDimensionException(HERE) << "Error: the quadrant orientation dimension (" << quadrantOrientation.getDimension() << ") is not compatible with the antecedent dimension (" << dimension << ")";
  quadrantOrientation_ = quadrantOrientation;
}

OT::NumericalPoint AdaptiveDirectionalSampling::getQuadrantOrientation() const
{
  return quadrantOrientation_;
}


NumericalSample AdaptiveDirectionalSampling::computeBlockSample()
{
  return NumericalSample();
}

/* String converter */
String AdaptiveDirectionalSampling::__repr__() const
{
  OSS oss;
  oss << "class=" << AdaptiveDirectionalSampling::GetClassName();
  return oss;
}


void AdaptiveDirectionalSampling::setPartialStratification(Bool partialStratification)
{
  partialStratification_ = partialStratification;
}


OT::Bool AdaptiveDirectionalSampling::getPartialStratification() const
{
  return partialStratification_;
}


void AdaptiveDirectionalSampling::setMaximumStratificationDimension(OT::UnsignedInteger maximumStratificationDimension)
{
  maximumStratificationDimension_ = maximumStratificationDimension;
}

OT::UnsignedInteger AdaptiveDirectionalSampling::getMaximumStratificationDimension() const
{
  return maximumStratificationDimension_;
}


OT::NumericalPoint AdaptiveDirectionalSampling::getTStatistic() const
{
  return T_;
}


} /* namespace OTADS */
