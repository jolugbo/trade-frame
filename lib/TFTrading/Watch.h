/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#pragma once

#include <memory>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <OUCommon/Delegate.h>

#include <TFTimeSeries/TimeSeries.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderInterface.h>

#include <TFIQFeed/IQFeedSymbol.h>

// 20151228 convert the delegate to a signal?  a little slower maybe.

// 20160122 will want to set signals on provider so watch/unwatch as provider transitions connection states

namespace ou { // One Unified
namespace tf { // TradeFrame

class Watch {
public:

  using pWatch_t = std::shared_ptr<Watch>;
  using pInstrument_t = Instrument::pInstrument_t;
  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;

  struct Fundamentals_t {
    double dblHistoricalVolatility;
    int nShortInterest;
    double dblPriceEarnings;
    double dbl52WkHi;
    double dbl52WkLo;
    double dblDividendAmount;
    double dblDividendRate;
    double dblDividendYield;
    boost::gregorian::date dateExDividend;
  };

  struct Summary_t {
    int nOpenInterest;
    int nTotalVolume;
    double dblOpen;
    Summary_t(): nOpenInterest( 0 ), nTotalVolume( 0 ), dblOpen( 0.0 ) {}
  };

  Watch( pInstrument_t pInstrument, pProvider_t pDataProvider );
  Watch( const Watch& rhs );
  virtual ~Watch();

  virtual Watch& operator=( const Watch& rhs );

  bool operator< ( const Watch& rhs ) const { return m_pInstrument->GetInstrumentName() <  rhs.m_pInstrument->GetInstrumentName(); };
  bool operator<=( const Watch& rhs ) const { return m_pInstrument->GetInstrumentName() <= rhs.m_pInstrument->GetInstrumentName(); };

  pInstrument_t GetInstrument() { return m_pInstrument; };

  void SetProvider( pProvider_t pDataProvider );
  pProvider_t GetProvider() { return m_pDataProvider; };

  bool Watching() const { return 0 != m_cntWatching; };

  // TODO: these need spinlocks
  inline const Quote& LastQuote() const { return m_quote; };  // may have thread sync issue
  inline const Trade& LastTrade() const { return m_trade; };  // may have thread sync issue

  const Fundamentals_t& Fundamentals() const { return m_fundamentals; };
  const Summary_t& Summary() const { return m_summary; };

  const Quotes& GetQuotes() const { return m_quotes; };
  const Trades& GetTrades() const { return m_trades; };

  ou::Delegate<const Quote&> OnQuote;
  ou::Delegate<const Trade&> OnTrade;

  //typedef std::pair<size_t,size_t> stateTimeSeries_t;
  //ou::Delegate<const stateTimeSeries_t&> OnPossibleResizeBegin;
  //ou::Delegate<const stateTimeSeries_t&> OnPossibleResizeEnd;

  virtual bool StartWatch();
  virtual bool StopWatch();

  virtual void EmitValues( bool bEmitName = true ) const;

  void RecordSeries( bool bRecord ) { m_bRecordSeries = bRecord; }
  bool RecordingSeries() const { return m_bRecordSeries; }

  virtual void SaveSeries( const std::string& sPrefix );
  virtual void SaveSeries( const std::string& sPrefix, const std::string& sDaily );

  virtual void ClearSeries();

protected:

  // use an iterator instead?  or keep as is as it facilitates multi-thread append and access operations
  // or will the stuff in TBB help with this type of access?

  bool m_bRecordSeries;

  ou::tf::Quote m_quote;
  ou::tf::Trade m_trade;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  pInstrument_t m_pInstrument;

  pProvider_t m_pDataProvider;

  unsigned int m_cntWatching;

private:

  bool m_bWatchingEnabled;
  bool m_bWatching; // in/out of connected state
  bool m_bEventsAttached; // code validation

  Fundamentals_t m_fundamentals;
  Summary_t m_summary;

  ou::tf::Trade::price_t m_PriceMax;
  ou::tf::Trade::price_t m_PriceMin;
  ou::tf::Trade::volume_t m_VolumeTotal;

  void Initialize();

  void AddEvents();
  void RemoveEvents();

  void HandleConnecting( int );
  void HandleConnected( int );
  void HandleDisconnecting( int );
  void HandleDisconnected( int );

  void EnableWatch();
  void DisableWatch();

  void HandleQuote( const Quote& quote );
  void HandleTrade( const Trade& trade );

  void HandleIQFeedFundamentalMessage( ou::tf::IQFeedSymbol& symbol );
  void HandleIQFeedSummaryMessage( ou::tf::IQFeedSymbol& symbol );

  void HandleTimeSeriesAllocation( Trades::size_type count );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const InstrumentInfo>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<InstrumentInfo>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

} // namespace tf
} // namespace ou
