/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    SpreadValidation.h
 * Author:  raymond@burkholder.net
 * Project: lib/TFTrading
 * Created on May 27, 2019, 8:59 AM
 */

#ifndef SPREADCANDIDATETWOLEGS_H
#define SPREADCANDIDATETWOLEGS_H

#include <vector>

#include <TFTrading/Watch.h>

#include "SpreadCandidate.h"

namespace ou {
namespace tf {

// Validate Quote Spread on a collection of Options
class SpreadValidation {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  SpreadValidation();
  SpreadValidation( size_t nLegs );
  SpreadValidation( const SpreadValidation& );
  SpreadValidation( const SpreadValidation&& );
  ~SpreadValidation();

  void SetLegCount( size_t nLegs );

  void SetWatch( size_t ixLeg, pWatch_t pWatch, double dblSpread );
  pWatch_t GetWatch( size_t ixLeg );
  void ClearWatch( size_t ixLeg );

  bool IsActive() const;
  bool Validate( size_t nDuration );
  void ClearCandidates();

protected:
private:

  using vSpreadCandidate_t = std::vector<SpreadCandidate>;
  vSpreadCandidate_t m_vSpreadCandidate;

};

} // namespace tf
} // namespace ou

#endif /* SPREADCANDIDATETWOLEGS_H */

