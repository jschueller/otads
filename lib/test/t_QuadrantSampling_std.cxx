#include <iostream>
// OT includes
#include <OT.hxx>
#include "RandomDirection.hxx"
#include "QuadrantSampling.hxx"

using namespace OT;
using namespace OTADS;

int main(int argc, char **argv)
{
//   Log::Show(Log::ALL);

  OStream fullprint(std::cout);

  for (UnsignedInteger d = 3; d < 4; ++ d)
  {
    RandomDirection rd(d);
    for (UnsignedInteger q = 0; q < static_cast<UnsignedInteger>(1 << d); ++ q)
    {
      QuadrantSampling qs(rd, q);
      fullprint << "quadrant=" << q << std::endl;
      for (UnsignedInteger i = 0; i < 10; ++ i)
      {
        fullprint << "  sample=" << qs.generate()[0].getCollection() << std::endl;
      }
    }

    // lets chose (-1,...,-1) for first quadrant bisector
    NumericalPoint bisector(d, -1.);
    for (UnsignedInteger q = 0; q < static_cast<UnsignedInteger>(1 << d); ++ q)
    {

      QuadrantSampling qs(rd, q, bisector);
      fullprint << "quadrant=" << q << " bisector=" << bisector.getCollection() << std::endl;
      for (UnsignedInteger i = 0; i < 10; ++ i)
      {
        fullprint << "  sample=" << qs.generate()[0].getCollection() << std::endl;
      }
    }

    // let's stratify only the 2 last dims
    UnsignedInteger sDim = 2;
    Indices strataIndices(sDim);
    strataIndices.fill();

    for (UnsignedInteger q = 0; q < static_cast<UnsignedInteger>(1 << sDim); ++ q)
    {
      QuadrantSampling qs(rd, q);
      qs.setStrataIndices(strataIndices);
      fullprint << "quadrant=" << q << " strataIndices=" << strataIndices << std::endl;
      for (UnsignedInteger i = 0; i < 10; ++ i)
      {
        fullprint << "  sample=" << qs.generate()[0].getCollection() << std::endl;
      }
    }

  }
  return 0;
}

