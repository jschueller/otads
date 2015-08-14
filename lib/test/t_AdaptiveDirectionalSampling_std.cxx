#include <iostream>
// OT includes
#include <OT.hxx>
#include "AdaptiveDirectionalSampling.hxx"
#include "RandomDirection.hxx"

using namespace OT;
using namespace OTADS;

int main(int argc, char **argv)
{
//   Log::Show(Log::ALL);

  OStream fullprint(std::cout);

  UnsignedInteger dim = 4;
  Description input(dim);
  input[0] = "E";
  input[1] = "F";
  input[2] = "L";
  input[3] = "I";

  /* We create a numerical math function */
  NumericalMathFunction function(input, Description(1, "d"), Description(1, "F*L^3/(3.*E*I)"));

  /* We create a normal distribution point of dimension 1 */
  NumericalPoint mean(dim, 0.0);
  mean[0] = 50.0; // E
  mean[1] =  1.0; // F
  mean[2] = 10.0; // L
  mean[3] =  5.0; // I
  NumericalPoint sigma(dim, 1.0);
  IdentityMatrix R(dim);
  Normal myDistribution(mean, sigma, R);

  /* We create a 'usual' RandomVector from the Distribution */
  RandomVector vect(myDistribution);

  /* We create a composite random vector */
  RandomVector output(function, vect);

  /* We create an Event from this RandomVector */
  Event myEvent(output, Less(), -3.0);

  UnsignedInteger n = static_cast<UnsignedInteger>(1e3);

  {
    /* We create a Directional Sampling algorithm */
    MonteCarlo myAlgo(myEvent);
    myAlgo.setMaximumOuterSampling(100 * n);
    myAlgo.setBlockSize(1);

    /* Perform the simulation */
    myAlgo.run();

    /* Stream out the result */
    fullprint << "result=" << myAlgo.getResult() << std::endl;
  }


  /* We create a Directional Sampling algorithm */
  AdaptiveDirectionalSampling myAlgo(myEvent);
//   myAlgo.setPartialStratification(true);
//   myAlgo.setMaximumStratificationDimension(2);
  myAlgo.setMaximumOuterSampling(n);
  myAlgo.setBlockSize(1);
  UnsignedInteger calls0 = function.getEvaluationCallsNumber();
  /* Perform the simulation */
  myAlgo.run();

  UnsignedInteger calls = function.getEvaluationCallsNumber() - calls0;

  /* Stream out the result */
  fullprint << "result=" << myAlgo.getResult() << std::endl;
  fullprint << "callsNumber=" << calls << std::endl;


  return 0;
}

