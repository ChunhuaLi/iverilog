/*
 * Copyright (c) 2000 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#if !defined(WINNT) && !defined(macintosh)
#ident "$Id: vvm_signal.cc,v 1.2 2000/03/17 20:21:14 steve Exp $"
#endif

# include  "vvm_signal.h"

vvm_signal_t::vvm_signal_t(vpip_bit_t*b, unsigned nb)
{
      bits = b;
      nbits = nb;
}

vvm_signal_t::~vvm_signal_t()
{
}

void vvm_signal_t::init_P(unsigned idx, vpip_bit_t val)
{
      assert(idx < nbits);
      bits[idx] = val;
}

void vvm_signal_t::take_value(unsigned key, vpip_bit_t val)
{
      bits[key] = val;
      vpip_run_value_changes(this);
}

vvm_ram_callback::vvm_ram_callback()
{
}

vvm_ram_callback::~vvm_ram_callback()
{
}

/*
 * $Log: vvm_signal.cc,v $
 * Revision 1.2  2000/03/17 20:21:14  steve
 *  Detemplatize the vvm_signal_t class.
 *
 * Revision 1.1  2000/03/16 19:03:04  steve
 *  Revise the VVM backend to use nexus objects so that
 *  drivers and resolution functions can be used, and
 *  the t-vvm module doesn't need to write a zillion
 *  output functions.
 *
 */

