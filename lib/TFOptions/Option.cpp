/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <sstream>

#include <OUCommon/TimeSource.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Option.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Option::Option( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )
: Watch( pInstrument, pDataProvider ),
  m_pGreekProvider( pGreekProvider ),
  m_dblStrike( pInstrument->GetStrike() ),
  m_sSide( "-" )
{
  //std::cout << "Option::Option construction 1: " << pInstrument->GetInstrumentName() << std::endl;
  Initialize();
}

Option::Option( pInstrument_t pInstrument, pProvider_t pDataProvider )
: Watch( pInstrument, pDataProvider ),
  m_dblStrike( pInstrument->GetStrike() ),
  m_sSide( "-" )
{
  //std::cout << "Option::Option construction 2: " << pInstrument->GetInstrumentName() << std::endl;
  Initialize();
}

Option::Option( const Option& rhs ) :
  Watch( rhs ),
  m_dblStrike( rhs.m_dblStrike ),
  m_greek( rhs.m_greek ),
  m_sSide( rhs.m_sSide ),
  m_pGreekProvider( rhs.m_pGreekProvider )
{
  //std::cout << "Option::Option construction 3: " << m_pInstrument->GetInstrumentName() << std::endl;
  Initialize();
}

Option::~Option( void ) {
  //std::cout << "Option::~Option destruction: " << m_pInstrument->GetInstrumentName() << std::endl;
//  StopWatch();  // issues here
}

Option& Option::operator=( const Option& rhs ) {
  Watch::operator=( rhs );
  m_dblStrike = rhs.m_dblStrike;
  m_greek = rhs.m_greek;
  m_sSide = rhs.m_sSide;
  m_pGreekProvider = rhs.m_pGreekProvider;
  Initialize();
  return *this;
}

void Option::Initialize( void ) {
  assert( Watch::m_pInstrument->IsOption() || Watch::m_pInstrument->IsFuturesOption() );
  if ( m_pGreekProvider )
    assert( m_pGreekProvider->ProvidesGreeks() );
  m_greeks.Reserve( 1024 );  // reduce startup allocations
}

bool Option::StartWatch( void ) {
  bool b = Watch::StartWatch();
  if ( b ) {
    if ( m_pGreekProvider )
      m_pGreekProvider->AddGreekHandler( m_pInstrument, MakeDelegate( this, &Option::HandleGreek ) );
  }
  return b;
}

void Option::CalcRate( // version 1, called by version 2, updates input
  ou::tf::option::binomial::structInput& input,
  const ou::tf::LiborFromIQFeed& libor,
  boost::posix_time::ptime dtUtcNow, boost::posix_time::ptime dtUtcExpiry
) {

  assert( boost::posix_time::not_a_date_time != dtUtcNow );
  assert( boost::posix_time::not_a_date_time != dtUtcExpiry );
  assert( dtUtcNow < dtUtcExpiry );

  static time_duration tdurOneYear( 365 * 24, 0, 0 );  // should generalize to calc for current year (leap year, etc)
//    time_duration tdurOneYear( 360 * 24, 0, 0 );  // https://www.interactivebrokers.com/en/index.php?f=interest&p=schedule
//    time_duration tdurOneYear( 250 * 24, 0, 0 );
  static long lSecForOneYear = tdurOneYear.total_seconds();
  //long lSecToExpiry = ( m_dtExpiry - now ).total_seconds();

  boost::posix_time::time_duration durToExpiry = dtUtcExpiry - dtUtcNow;
  int lSecToExpiry = durToExpiry.total_seconds();
  double ratioToExpiry = (double) lSecToExpiry / (double) lSecForOneYear;
  input.T = ratioToExpiry;

  double rate = libor.ValueAt( durToExpiry ) / 100.0;
  input.r = rate;
  input.b = rate; // is this correct?
}

void Option::CalcRate( // version 2, calls version 1, uses instrument expiry date
  ou::tf::option::binomial::structInput& input,
        const boost::posix_time::ptime dtUtcNow, const ou::tf::LiborFromIQFeed& libor ) {

  assert( boost::posix_time::not_a_date_time != dtUtcNow );

  // system time is already utc
//  boost::posix_time::ptime dtUtcNow =
//          ou::TimeSource::Instance().
//              ConvertRegionalToUtc( dtUtcNow.date(), dtUtcNow.time_of_day(), "America/New_York", true );

  ptime dtUtcExpiry( m_pInstrument->GetExpiryUtc() );
  if ( dtUtcNow < dtUtcExpiry ) {
  }
  else {
    std::stringstream s;
    s << "Option::CalcRate - " << "now=" << dtUtcNow << "," << "expiry=" << dtUtcExpiry;
    throw std::runtime_error( s.str().c_str() );
  }

  CalcRate( input, libor, dtUtcNow, dtUtcExpiry );
}

void Option::CalcGreeks(
  ou::tf::option::binomial::structInput& input, ptime dtUtcNow, bool bNeedsGuess ) {
  // example caller: void ExpiryBundle::CalcGreeksAtStrike

  // needs CalcRate before entering here
  // needs input.S (underlying price)

  if ( !Watching() ) return;  // not watching so no active data

  input.X = m_dblStrike;
  //input.S = underlying

  // todo: use the haskell book to get an estimator
  // Manaster and Koehler Start Value, Option Pricing Formulas, pg 454
  if ( bNeedsGuess ) {
//  double dblVolatilityGuess = dblVolHistorical / 100.0;
//  input.v = dblVolatilityGuess;
    double dblVolatilityGuess = std::sqrt( std::abs( std::log( input.S / input.X ) + input.r * input.T ) * 2.0 / input.T );
    input.v = dblVolatilityGuess;
  }

//  std::cout << "Guess " << input.v << std::endl;

  try {
    input.optionSide = m_pInstrument->GetOptionSide();
    input.Check();
    ou::tf::option::binomial::structOutput output;
    ou::tf::option::binomial::CalcImpliedVolatility( input, LastQuote().Midpoint(), output );
    ou::tf::Greek greek( dtUtcNow, output.iv, output.delta, output.gamma, output.theta, output.vega, output.rho );
    AppendGreek( greek );
  }
  catch (...) {
//      std::cout << iter->second.Put()->GetInstrument()->GetInstrumentName() << ": IV Calc problem" << std::endl;
  }
}

bool Option::StopWatch( void ) {
  bool b = Watch::StopWatch();
  if ( b ) {
    if ( m_pGreekProvider )
      m_pGreekProvider->RemoveGreekHandler( m_pInstrument, MakeDelegate( this, &Option::HandleGreek ) );
  }
  return b;
}

void Option::EmitValues( bool bEmitName ) {
  Watch::EmitValues( bEmitName );
  std::cout
    << ","
    << "IV:" << m_greek.ImpliedVolatility() << ","
    << "D:" << m_greek.Delta() << ","
    << "G:" << m_greek.Gamma() << ","
    << "T:" << m_greek.Theta() << ","
    << "V:" << m_greek.Vega() << ","
    << "R:" << m_greek.Rho()
    //<< std::endl
    ;
}

void Option::HandleGreek( const Greek& greek ) {
  m_greek = greek;
  m_greeks.Append( greek );
  OnGreek( greek );
}

void Option::AppendGreek( const ou::tf::Greek& greek ) {
  HandleGreek( greek );
}

void Option::SaveSeries( const std::string& sPrefix ) {

  std::string sPathName;

  HDF5Attributes::structOption option(
    m_dblStrike, m_pInstrument->GetExpiryYear(), m_pInstrument->GetExpiryMonth(), m_pInstrument->GetExpiryDay(), m_pInstrument->GetOptionSide() );

  Watch::SaveSeries( sPrefix );

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  // add in option attributes to the already written quotes and trades.
  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
    HDF5Attributes attrGreeks( dm, sPathName, option );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
    HDF5Attributes attrGreeks( dm, sPathName, option );
  }

  if ( 0 != m_greeks.Size() ) {
    sPathName = sPrefix + "/greeks/" + m_pInstrument->GetInstrumentName();
    HDF5WriteTimeSeries<ou::tf::Greeks> wtsGreeks( dm, true, true, 5, 256 );
    wtsGreeks.Write( sPathName, &m_greeks );
    HDF5Attributes attrGreeks( dm, sPathName, option );
    attrGreeks.SetSignature( ou::tf::Greek::Signature() );
    attrGreeks.SetMultiplier( m_pInstrument->GetMultiplier() );
    attrGreeks.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
    if ( m_pGreekProvider ) {
      attrGreeks.SetProviderType( m_pGreekProvider->ID() );
    }
    else {
      attrGreeks.SetProviderType( ou::tf::keytypes::EProviderCalc );
    }
  }

}

//
// ==================^
//

Call::Call( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )
: Option( pInstrument, pDataProvider,pGreekProvider )
{
  // assert instrument is a call
  assert( ou::tf::OptionSide::Call == pInstrument->GetOptionSide() );
  m_sSide = "C";
}

Call::Call( pInstrument_t pInstrument, pProvider_t pDataProvider )
: Option( pInstrument, pDataProvider )
{
  // assert instrument is a call
  assert( ou::tf::OptionSide::Call == pInstrument->GetOptionSide() );
  m_sSide = "C";
}

//
// ==================
//

Put::Put( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )
: Option( pInstrument, pDataProvider,pGreekProvider )
{
  // assert instrument is a put
  assert( ou::tf::OptionSide::Put == pInstrument->GetOptionSide() );
  m_sSide = "P";
}

Put::Put( pInstrument_t pInstrument, pProvider_t pDataProvider )
: Option( pInstrument, pDataProvider )
{
  // assert instrument is a put
  assert( ou::tf::OptionSide::Put == pInstrument->GetOptionSide() );
  m_sSide = "P";
}


//
// ==================
//

} // namespace option
} // namespace tf
} // namespace ou
