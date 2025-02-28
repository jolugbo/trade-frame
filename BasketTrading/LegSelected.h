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
 * File:    LegSelected.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on July 4, 2019, 10:08 PM
 */

#ifndef LEGSELECTED_H
#define LEGSELECTED_H

#include <string>

#include <TFOptions/Option.h>

class LegSelected { // specific definition of each leg for trading
public:

  using pOption_t = ou::tf::option::Option::pOption_t;

  LegSelected();
  LegSelected( const LegSelected& ) = delete;
  LegSelected( const LegSelected&& );

  // return 0 if not changed, 1 if changed
  unsigned int Update( double spread, double strike, boost::gregorian::date dateExpiry, const std::string& sIQFeedOptionName );

  bool Changed() { bool bTemp = m_bChanged; m_bChanged = false; return bTemp; }
  double Spread() const { return m_dblSpread; }
  double Strike() const { return m_dblStrike; }
  boost::gregorian::date Expiry() const { return m_dateExpiry; }
  const std::string& IQFeedOptionName() const { return m_sIQFeedOptionName; }
  pOption_t& Option() { return m_pOption; } // cheating by returning a reference

  void Clear();

private:
  bool m_bChanged;
  double m_dblSpread;
  double m_dblStrike;
  boost::gregorian::date m_dateExpiry;
  std::string m_sIQFeedOptionName;
  pOption_t m_pOption; // set after initial construction
};

#endif /* LEGSELECTED_H */
