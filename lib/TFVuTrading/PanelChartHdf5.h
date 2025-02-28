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

// Started 2013/10/06

// used by Hdf5Chart

// TODO: refactor to use PanelFinancialChart when complete?

#include <wx/treectrl.h>

#include "ModelChartHdf5.h"

#include "WinChartView.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_CHARTHDF5_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_CHARTHDF5_TITLE _("Panel Chart HDF5 Data")
#define SYMBOL_PANEL_CHARTHDF5_IDNAME ID_PANEL_CHARTHDF5
#define SYMBOL_PANEL_CHARTHDF5_SIZE wxSize(400, 300)
#define SYMBOL_PANEL_CHARTHDF5_POSITION wxDefaultPosition

class PanelChartHdf5: public wxPanel {
public:

  PanelChartHdf5(void);
  PanelChartHdf5(
    wxWindow* parent, wxWindowID id = SYMBOL_PANEL_CHARTHDF5_IDNAME,
    const wxPoint& pos = SYMBOL_PANEL_CHARTHDF5_POSITION,
    const wxSize& size = SYMBOL_PANEL_CHARTHDF5_SIZE,
    long style = SYMBOL_PANEL_CHARTHDF5_STYLE );
  virtual ~PanelChartHdf5(void);

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_PANEL_CHARTHDF5_IDNAME,
    const wxPoint& pos = SYMBOL_PANEL_CHARTHDF5_POSITION,
    const wxSize& size = SYMBOL_PANEL_CHARTHDF5_SIZE,
    long style = SYMBOL_PANEL_CHARTHDF5_STYLE );

  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

protected:

  void Init();
  void CreateControls();

private:

  enum { ID_Null=wxID_HIGHEST, ID_PANEL_CHARTHDF5 };

  class CustomItemData: public wxTreeItemData { // wxTreeCtrl node/leaf info
  public:
    enum enumNodeType { Root, Group, Object } m_eNodeType;
    enum enumDatumType { Quotes, Trades, Bars, Greeks, AtmIV, NoDatum } m_eDatumType;
    CustomItemData( enumNodeType eNodeType, enumDatumType eDatumType )
      : m_eNodeType( eNodeType ), m_eDatumType( eDatumType ) {};
  };

  ou::tf::HDF5DataManager* m_pdm;

  ou::ChartDataView* m_pChartDataView;
  WinChartView* m_pWinChartView;

  ModelChartHdf5 m_ModelChartHdf5;

  std::string m_sCurrentPath;  // used while traversing and building tree
  wxTreeItemId m_curTreeItem; // used while traversing and building tree
  CustomItemData::enumDatumType m_eLatestDatumType;  // need this until all timeseries have a signature attribute associated

  wxTreeCtrl* m_pHdf5Root;  // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html

  void OnClose( wxCloseEvent& event );

  size_t LoadDataAndGenerateChart( CustomItemData::enumDatumType, const std::string& sPath );

  void HandleLoadTreeHdf5Group( const std::string& s1, const std::string& s2 );
  void HandleLoadTreeHdf5Object( const std::string& s1, const std::string& s2 );

  void HandleBuildTreePathParts( const std::string& sPath );

  void HandleTreeEventItemActivated( wxTreeEvent& event );

};

} // namespace tf
} // namespace ou
