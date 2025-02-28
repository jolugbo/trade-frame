/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    Underlying.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on 2021/06/19 19:41
 */

#pragma once

#include <string>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFOptions/Aggregate.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Portfolio.h>

#include <TFBitsNPieces/BollingerTransitions.h>

//#include "PivotCrossing.h"

 // contains:
 //  1) will contain active strategies,
 //  2) gex calcs
 //  3) structures common to assigned strategies

class Underlying {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;
  using fGatherOptionDefinitions_t = ou::tf::option::fGatherOptionDefinitions_t;

  Underlying( pWatch_t, pPortfolio_t );
  ~Underlying();

  void ReadDailyBars( const std::string& sDailyBarPath );
  void SetPivots( double dblR2, double dblR1, double dblPV, double dblS1, double dblS2 );
  void PopulateChartDataView( pChartDataView_t ); // share data sets
  void PopulateChains( fGatherOptionDefinitions_t& );

  void UpdateChart( boost::posix_time::ptime );

  pWatch_t GetWatch() { return m_pWatch; }
  pPortfolio_t GetPortfolio() { return m_pPortfolio; }
  pChartDataView_t GetChartDataView() { return m_pChartDataView; }

  void SaveSeries( const std::string& sPrefix );

  // TODO: will need two mapChain types:
  //   1) basic for passing to strategy
  //   2) gex calcs

protected:
private:

  enum EChartSlot { Price, Volume, PL, Tick };

  using gex_t = ou::tf::option::Aggregate;

  pWatch_t m_pWatch;
  pPortfolio_t m_pPortfolio; // aggregates strategies associated with underlying

  gex_t m_GexCalc;

  pChartDataView_t m_pChartDataView;

  ou::tf::BarFactory m_bfTrades06Sec; // charting

  ou::ChartEntryBars m_cePrice;
  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryMark m_cePivots;

  ou::ChartEntryIndicator m_cePLCurrent;
  ou::ChartEntryIndicator m_cePLUnRealized;
  ou::ChartEntryIndicator m_cePLRealized;
  ou::ChartEntryIndicator m_ceCommissionPaid;

  ou::tf::BollingerTransitions m_BollingerTransitions;

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );
  void HandleBarTrades06Sec( const ou::tf::Bar& bar );

};
