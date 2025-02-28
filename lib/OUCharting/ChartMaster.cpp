/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

//#include <boost/lexical_cast.hpp>
#include <sstream>

#include "ChartDataView.h"
#include "ChartMaster.h"
#include "OUCommon/Colour.h"

namespace ou { // One Unified

ChartMaster::ChartMaster( unsigned int width, unsigned int height )
: m_pCdv( nullptr), m_pDA( nullptr ),
  m_nChartWidth( width ), m_nChartHeight( height ),
  m_intCrossHairX {}, m_intCrossHairY {}, m_bCrossHair( false ),
  m_bHasData( false )
{
  Initialize();
}

ChartMaster::ChartMaster(): ChartMaster( 600, 900 ) {}

ChartMaster::~ChartMaster() {
}

void ChartMaster::Initialize() {
  char code[2048];
  //bool b = Chart::setLicenseCode( "DEVP-2G22-4QPN-HDS6-925A-95C1", &code );
  //bool b = Chart::setLicenseCode( "UDEV-23FP-5DWS-X22U-BBBD-FBD8", &code );
  bool b = Chart::setLicenseCode( "DEVP-2G22-4QPN-HDS6-925A-95C1 UDEV-23FP-5DWS-X22U-BBBD-FBD8", code );
  //Chart::getLicenseAttr()
  assert( b );
}

void ChartMaster::SetChartDataView( ChartDataView* pcdv ) {

  //ResetDynamicLayer();

  m_pCdv = pcdv;

  m_pChart = std::make_unique<MultiChart>( m_nChartWidth, m_nChartHeight );
  ChartStructure();

  //if ( nullptr != m_pCdv ) m_pCdv->SetChanged();
};

void ChartMaster::SetChartDimensions(unsigned int width, unsigned int height) {

  //ResetDynamicLayer();

  m_nChartWidth = width;
  m_nChartHeight = height;

  //MultiChart multi( m_nChartWidth, m_nChartHeight, Chart::goldColor );
  m_pChart = std::make_unique<MultiChart>( m_nChartWidth, m_nChartHeight );
  ChartStructure();

  //if ( nullptr != m_pCdv ) m_pCdv->SetChanged();
}

bool ChartMaster::GetChartDataViewChanged() { // flag is reset during call
  return ( nullptr == m_pCdv ) ? false : m_pCdv->GetChanged();
}

/*
void ChartMaster::SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd ) {
  m_dblViewPortXBegin =
    ( boost::posix_time::not_a_date_time != dtBegin )
    ?
    Chart::chartTime(
      dtBegin.date().year(), dtBegin.date().month(), dtBegin.date().day(),
      dtBegin.time_of_day().hours(), dtBegin.time_of_day().minutes(), dtBegin.time_of_day().seconds() )
    :
    0;
  m_dblViewPortXEnd =
    ( boost::posix_time::not_a_date_time != dtEnd )
    ?
    Chart::chartTime(
      dtEnd.date().year(), dtEnd.date().month(), dtEnd.date().day(),
      dtEnd.time_of_day().hours(), dtEnd.time_of_day().minutes(), dtEnd.time_of_day().seconds() )
    :
    0;
}
*/

void ChartMaster::SetBarWidth( boost::posix_time::time_duration tdBarWidth ) {
  m_tdBarWidth = tdBarWidth;
}

void ChartMaster::ChartStructure() {

  std::string sTitle( m_pCdv->GetName() + " - " + m_pCdv->GetDescription() );
  m_pChart->addTitle( sTitle.c_str() );

  // chart 0 (main chart) is x, chrt 1 (volume chart) is 1/4x, ChartN (indicator charts) are 1/3x
  // calc assumes chart 0 and chart 1 are always present
  size_t n = m_pCdv->GetChartCount();
  int heightChart0 = ( 12 * ( m_nChartHeight - 25 ) ) / ( 15 + ( 4 * ( n - 2 ) ) );
  int heightChart1 = heightChart0 / 4;
  int heightChartN = heightChart0 / 3;

  m_vSubCharts.resize( n );  // this is the number of sub-charts we are working with (move to class def so not redone all the time?)

  size_t ix = 0;
  int y = 15;  // was 25
  int x = 50;
  int xAxisHeight = 50;
  XYChart* pXY;  // used for each sub-chart

  while ( ix < n ) {
    switch ( ix ) {
      case 0:  // main chart
        m_pXY0 = pXY = new XYChart( m_nChartWidth, heightChart0 );
        pXY->setPlotArea( x, xAxisHeight, m_nChartWidth - 2 * x, heightChart0 - xAxisHeight )->setGridColor(0xc0c0c0, 0xc0c0c0);
        pXY->xAxis()->setColors(Chart::LineColor, Chart::LineColor);
        pXY->setClipping();
        pXY->setXAxisOnTop( true );
        pXY->xAxis()->setWidth( 2 );
        pXY->yAxis()->setWidth( 2 );
        pXY->yAxis()->setMargin( 2, 5 );
        pXY->addLegend( x, xAxisHeight, true, 0, 9.0 );
        m_pChart->addChart( 0, y, pXY );
        m_pChart->setMainChart( pXY );
        y += heightChart0;
        break;
      case 1: // volume chart
        pXY = new XYChart( m_nChartWidth, heightChart1 );
        pXY->setPlotArea( x, 0, m_nChartWidth - 2 * x, heightChart1 )->setGridColor(0xc0c0c0, 0xc0c0c0);
        pXY->setClipping();
        //pXY->xAxis()->setColors(Chart::LineColor, Chart::Transparent);  // turn off axis
        pXY->xAxis()->setColors(Chart::LineColor, Chart::LineColor);
        pXY->xAxis()->copyAxis( m_pXY0->xAxis() ); // use settings from main subchart
        pXY->xAxis()->setWidth( 2 );
        pXY->yAxis()->setWidth( 2 );
        pXY->yAxis()->setMargin( 5, 5 );
        pXY->addLegend( x, 0, true, 0, 9.0 );
        m_pChart->addChart( 0, y, pXY );
        y += heightChart1;
        break;
      default:  // secondary indicator charts
        pXY = new XYChart( m_nChartWidth, heightChartN );
        pXY->setPlotArea( x, 0, m_nChartWidth - 2 * x, heightChartN )->setGridColor(0xc0c0c0, 0xc0c0c0);
        pXY->setClipping();
        //pXY->xAxis()->setColors(Chart::LineColor, Chart::Transparent);  // turn off axis
        pXY->xAxis()->setColors(Chart::LineColor, Chart::LineColor);
        pXY->xAxis()->copyAxis( m_pXY0->xAxis() ); // use settings from main subchart
        pXY->xAxis()->setWidth( 2 );
        pXY->yAxis()->setWidth( 2 );
        pXY->yAxis()->setMargin( 5, 5 );
        pXY->addLegend( x, 0, true, 0, 9.0 );
        m_pChart->addChart( 0, y, pXY );
        y += heightChartN;
        break;
    }
    m_vSubCharts[ ix ] = std::unique_ptr<XYChart>( pXY );
    ++ix;
  }
}

void ChartMaster::ChartData( XYChart* pXY0 ) {

  // determine XAxis min/max while adding chart data
  double dblXBegin {};
  double dblXEnd {};
  m_pCdv->EachChartEntryCarrier(
    [this,&dblXBegin,&dblXEnd]( ou::ChartEntryCarrier& carrier ){
      size_t ixChart = carrier.GetActualChartId();
      ChartEntryBase::structChartAttributes Attributes;
      if ( carrier.GetChartEntry()->AddEntryToChart( m_vSubCharts[ ixChart ].get(), &Attributes ) ) {
        // following assumes values are always > 0
        dblXBegin = ( 0 == dblXBegin )
          ? Attributes.dblXMin
          : std::min<double>( dblXBegin, Attributes.dblXMin );
        dblXEnd   = ( 0 == dblXEnd   )
          ? Attributes.dblXMax
          : std::max<double>( dblXEnd,   Attributes.dblXMax );
      }
    } );

  // time axis scales
  if ( dblXBegin != dblXEnd ) {
    pXY0->xAxis()->setDateScale( dblXBegin, dblXEnd, 0, 0 );
    m_bHasData = true;
  }
  else {
    m_bHasData = false;
    //std::cout << "Time Scales match" << std::endl;
  }

}

void ChartMaster::DrawChart() {

  if ( m_pChart ) {
    if ( m_pCdv ) { // DataView has something to draw

      ChartData( m_pXY0 );
      RenderChart();

    }
  }
}

void ChartMaster::RenderChart() {
  if ( m_bHasData ) {
    if ( m_bCrossHair ) DrawDynamicLayer();
    MemBlock m = m_pChart->makeChart( BMP );
    if ( m_fOnDrawChart ) m_fOnDrawChart( m );
  }
}

bool ChartMaster::ResetDynamicLayer() {
  bool bExisted { false };
  if ( nullptr != m_pDA ) {
    bExisted = true;
    m_pChart->removeDynamicLayer();
    m_pDA = nullptr;
  }
  return bExisted;
}

void ChartMaster::CrossHairPosition( int x, int y ) {
  m_intCrossHairX = x;
  m_intCrossHairY = y;
}

void ChartMaster::CrossHairDraw( bool bDraw ) {
  m_bCrossHair = bDraw;
}

void ChartMaster::DrawDynamicLayer() {
  // TODO: maybe trigger more often than data update happens?
  //   trigger a call back for sync?
  if ( m_pChart ) {

    m_pDA = m_pChart->initDynamicLayer(); // new/clear

    //std::stringstream ss;
    //ss << "x=" << m_intCrossHairX << ",y=" << m_intCrossHairY;
    //m_pDA->text( ss.str().c_str(), "normal", 10, 10, 10, Colour::Black );

    int n = m_pChart->getChartCount();
    assert( 0 < n );

    BaseChart* p;

    p = m_pChart->getChart( 0 );
    XYChart* pChart0 = dynamic_cast<XYChart*>( p );
    PlotArea* pArea0 = pChart0->getPlotArea();

    p = m_pChart->getChart( n - 1 );
    XYChart* pChartN = dynamic_cast<XYChart*>( p );
    PlotArea* pAreaN = pChartN->getPlotArea();

    const int top    = pChart0->getAbsOffsetY() + pArea0->getTopY();
    const int bottom = pChartN->getAbsOffsetY() + pAreaN->getBottomY();
    const int left   = pChart0->getAbsOffsetX() + pArea0->getLeftX();
    const int right  = pChart0->getAbsOffsetX() + pArea0->getRightX();

    if (
      ( top <= m_intCrossHairY  ) &&
      ( bottom >= m_intCrossHairY ) &&
      ( left <= m_intCrossHairX ) &&
      ( right >= m_intCrossHairX )
    ) {
      m_pDA->hline( left, right, m_intCrossHairY, Colour::Gray );
      m_pDA->vline( top, bottom, m_intCrossHairX, Colour::Gray );
    }

  }
}

} // namespace ou
