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
 * File:    SpreadCandidate.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFTrading
 * Created on May 25, 2019, 1:25 PM
 */

#include "SpreadCandidate.h"

namespace ou {
namespace tf {

SpreadCandidate::SpreadCandidate()
: m_nUnDesired {}, m_nDesired {}, m_nConsecutiveSpreadOk {},
  m_dblSpread {}, m_dblMinimum( 0.10 )
{}

SpreadCandidate::SpreadCandidate( pWatch_t pWatch )
: SpreadCandidate( pWatch, 0.10 )
{
}

SpreadCandidate::SpreadCandidate( pWatch_t pWatch, double dblSpread )
: m_nUnDesired {}, m_nDesired {}, m_nConsecutiveSpreadOk {},
  m_dblSpread( dblSpread ), m_dblMinimum( 0.10 )
{
  SetWatch( pWatch );
}

SpreadCandidate::SpreadCandidate( const SpreadCandidate& rhs )
: m_nUnDesired( rhs.m_nUnDesired ), m_nDesired( rhs.m_nDesired ),
  m_nConsecutiveSpreadOk( rhs.m_nConsecutiveSpreadOk ),
  m_dblSpread( rhs.m_dblSpread ), m_dblMinimum( rhs.m_dblMinimum ),
  m_pWatch( rhs.m_pWatch )
{}

SpreadCandidate::SpreadCandidate( const SpreadCandidate&& rhs )
: m_nUnDesired( rhs.m_nUnDesired ), m_nDesired( rhs.m_nDesired ),
  m_nConsecutiveSpreadOk( rhs.m_nConsecutiveSpreadOk ),
  m_dblSpread( rhs.m_dblSpread ), m_dblMinimum( rhs.m_dblMinimum ),
  m_pWatch( std::move( rhs.m_pWatch ) )
{}

SpreadCandidate::~SpreadCandidate() {
  Clear();
}

void SpreadCandidate::Clear() {
  if ( m_pWatch ) {
    m_pWatch->StopWatch();
    m_pWatch->OnQuote.Remove( MakeDelegate( this, &SpreadCandidate::UpdateQuote ) );
    m_pWatch.reset();
  }
}

void SpreadCandidate::SetWatch( pWatch_t pWatch, double dblSpread ) {
  m_dblSpread = dblSpread;
  SetWatch( pWatch );
}

void SpreadCandidate::SetWatch( pWatch_t pWatch ) {
  assert( pWatch );
  Clear();
  m_pWatch = pWatch;
  if ( m_pWatch ) {
    m_nDesired = m_nUnDesired = m_nConsecutiveSpreadOk = 0;
    m_pWatch->OnQuote.Add( MakeDelegate( this, &SpreadCandidate::UpdateQuote ) );
    m_pWatch->StartWatch();
  }
}

SpreadCandidate::pWatch_t SpreadCandidate::GetWatch() { return m_pWatch; }

bool SpreadCandidate::Validate( size_t nDuration ) {
  bool bOk( false );
  if ( m_pWatch ) {
    if ( ( 5 * m_nUnDesired ) < ( m_nUnDesired + m_nDesired ) ) { // undesired is < 20% of total
      // TODO: may want to enforce some sort of ratio
      //   will need some statistics on what a good number might be
      m_nConsecutiveSpreadOk++;
    }
    else {
      m_nConsecutiveSpreadOk = 0;
    }
    bOk = ( nDuration <= m_nConsecutiveSpreadOk );
  }
  return bOk;
}

void SpreadCandidate::UpdateQuote( const ou::tf::Quote& quote ) {
  double spread( quote.Spread() );
  if ( spread < 0.005 ) {
    // ignore crossed/crossing quote
  }
  else {
    if ( ( 0.005 <= spread ) && ( spread < m_dblSpread )
      && ( m_dblMinimum < quote.Ask() ) && ( m_dblMinimum < quote.Bid() ) ) {
      m_nDesired++;
    }
    else {
      m_nUnDesired++;
    }
  }
}

} // namespace tf
} // namespace ou
