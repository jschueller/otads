// SWIG file AdaptiveDirectionalSampling.i

%{
#include "AdaptiveDirectionalSampling.hxx"
%}

%include AdaptiveDirectionalSampling.hxx
namespace OTADS { %extend AdaptiveDirectionalSampling { AdaptiveDirectionalSampling(const AdaptiveDirectionalSampling & other) { return new OTADS::AdaptiveDirectionalSampling(other); } } }
