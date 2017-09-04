/*------------------------------------------------------------------------*/
/*  Copyright 2014 National Renewable Energy Laboratory.                  */
/*  This software is released under the license detailed                  */
/*  in the file, LICENSE, which is located in the top-level Nalu          */
/*  directory structure                                                   */
/*------------------------------------------------------------------------*/

#ifndef KERNEL_H
#define KERNEL_H

#include "KokkosInterface.h"
#include "SimdInterface.h"
#include "ScratchViews.h"
#include "AlgTraits.h"

#include <stk_mesh/base/Entity.hpp>

#include <array>

namespace sierra {
namespace nalu {

class TimeIntegrator;
class SolutionOptions;

template<typename AlgTraits, typename LambdaFunction, typename ViewType>
void get_scv_shape_fn_data(LambdaFunction lambdaFunction, ViewType& shape_fn_view)
{
  static_assert(ViewType::Rank == 2u, "2D View");
  ThrowRequireMsg(shape_fn_view.extent_int(0) == AlgTraits::numScvIp_, "Inconsistent number of scv ips");
  ThrowRequireMsg(shape_fn_view.extent_int(1) == AlgTraits::nodesPerElement_, "Inconsistent number of of nodes");

  double tmp_data[AlgTraits::numScvIp_*AlgTraits::nodesPerElement_];
  lambdaFunction(tmp_data);

  DoubleType* data = &shape_fn_view(0,0);
  for(int i=0; i<AlgTraits::numScvIp_*AlgTraits::nodesPerElement_; ++i) {
    data[i] = tmp_data[i];
  }
}

template<typename AlgTraits, typename LambdaFunction, typename ViewType>
void get_scs_shape_fn_data(LambdaFunction lambdaFunction, ViewType& shape_fn_view)
{
  static_assert(ViewType::Rank == 2u, "2D View");
  ThrowRequireMsg(shape_fn_view.extent_int(0) == AlgTraits::numScsIp_, "Inconsistent number of scs ips");
  ThrowRequireMsg(shape_fn_view.extent_int(1) == AlgTraits::nodesPerElement_, "Inconsistent number of of nodes");

  double tmp_data[AlgTraits::numScsIp_*AlgTraits::nodesPerElement_];
  lambdaFunction(tmp_data);

  DoubleType* data = &shape_fn_view(0,0);
  for(int i=0; i<AlgTraits::numScsIp_*AlgTraits::nodesPerElement_; ++i) {
    data[i] = tmp_data[i];
  }
}

/** Base class for computational kernels in Nalu
 *
 * A kernel represents an atomic unit of computation applied over a given set of
 * nodes, elements, etc. using STK and Kokkos looping constructs.
 */
class Kernel
{
public:
  Kernel() = default;

  virtual ~Kernel() {}

  /** Perform pre-timestep work for the computational kernel
   */
  virtual void setup(const TimeIntegrator&) {}

  /** Execute the kernel within a Kokkos loop and populate the LHS and RHS for
   *  the linear solve
   */
  virtual void execute(
    SharedMemView<DoubleType**>&,
    SharedMemView<DoubleType*>&,
    ScratchViews<DoubleType>&)
  {}
};

}  // nalu
}  // sierra

#endif /* KERNEL_H */
