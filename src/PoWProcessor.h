/**
 * Template for processing Gapcoins PoW.
 *
 * Copyright (C)  2014  Jonny Frey  <j0nn9.fr39@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __POWPROCESSOR_H__
#define __POWPROCESSOR_H__
#include <gmp.h>
#include "PoW.h"


class PoWProcessor {
  
  public :

  PoWProcessor() { }
  virtual ~PoWProcessor() { }

  /**
   * should process a given PoW (e.g validate it and so on)
   * should return whether to continue calculating or not
   */
  virtual bool process(PoW *pow) = 0;

};
#endif /* __POWPROCESSOR_H__ */
