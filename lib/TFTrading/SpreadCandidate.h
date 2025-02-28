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
 * File:    SpreadCandidate.h
 * Author:  raymond@burkholder.net
 * Project: lib/TFTrading
 * Created on May 25, 2019, 1:25 PM
 */

#ifndef SPREADCANDIDATE_H
#define SPREADCANDIDATE_H

#include <TFTrading/Watch.h>

namespace ou {
namespace tf {

// Confirm that Quote Spread seems reasonable
class SpreadCandidate {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  SpreadCandidate();
  SpreadCandidate( const SpreadCandidate& );
  SpreadCandidate( const SpreadCandidate&& );
  SpreadCandidate( pWatch_t pWatch );
  SpreadCandidate( pWatch_t pWatch, double dblSpread );
  ~SpreadCandidate();

  void Clear();
  void SetWatch( pWatch_t pWatch, double dblSpread );
  pWatch_t GetWatch();
  bool Validate( size_t nDuration );
  bool IsActive() const { return 0 != m_pWatch.use_count(); };

private:

  size_t m_nDesired;
  size_t m_nUnDesired;
  size_t m_nConsecutiveSpreadOk;

  double m_dblSpread;
  const double m_dblMinimum;

  pWatch_t m_pWatch;

  void SetWatch( pWatch_t pWatch );
  void UpdateQuote( const ou::tf::Quote& quote );
};

} // namespace tf
} // namespace ou

#endif /* SPREADCANDIDATE_H */
