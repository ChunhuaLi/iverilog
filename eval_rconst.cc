/*
 * Copyright (c) 2001 Stephen Williams (steve@icarus.com)
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
#ifdef HAVE_CVS_IDENT
#ident "$Id: eval_rconst.cc,v 1.6 2003/02/01 23:37:34 steve Exp $"
#endif

# include "config.h"

# include  "PExpr.h"
# include  "verireal.h"

verireal* PExpr::eval_rconst(const Design*, const NetScope*) const
{
      return 0;
}

verireal* PEFNumber::eval_rconst(const Design*, const NetScope*) const
{
      verireal*res = new verireal;
      *res = *value_;
      return res;
}

verireal* PENumber::eval_rconst(const Design*, const NetScope*) const
{
      verireal*res = new verireal(value_->as_long());
      return res;
}

verireal* PEBinary::eval_rconst(const Design*des, const NetScope*scope) const
{
      verireal*lef = left_->eval_rconst(des, scope);
      verireal*rig = right_->eval_rconst(des, scope);
      verireal*res = 0;

      switch (op_) {
	  case '*':
	    if (lef == 0)
		  break;
	    if (rig == 0)
		  break;
	    res = new verireal;
	    *res = (*lef) * (*rig);
	    break;

	  default:
	    break;
      }

      delete lef;
      delete rig;
      return res;
}


verireal* PEIdent::eval_rconst(const Design*des, const NetScope*scope) const
{
      assert(scope);
      const NetExpr*expr = des->find_parameter(scope, path_);
      if (expr == 0)
	    return 0;

	/* Is this a real valued parameter? If so, then evaluate it
	   as a real expression and return the resulting verireal. */
      if (expr->expr_type() == NetExpr::ET_REAL) {
	    const NetECReal*ereal = dynamic_cast<const NetECReal*>(expr);
	    if (ereal == 0) {
		  cerr << get_line() << ": internal error: "
		       << "Unable to evaluate real constant expression "
		       << "(parameter=" << path_ << "): " << *expr << endl;
		  return 0;
	    }

	    return new verireal(ereal->value());
      }

      verinum* val = eval_const(des, scope);
      if (val == 0)
	    return 0;

      verireal*res = new verireal(val->as_long());
      delete val;
      return res;
}

/*
 * $Log: eval_rconst.cc,v $
 * Revision 1.6  2003/02/01 23:37:34  steve
 *  Allow parameter expressions to be type real.
 *
 * Revision 1.5  2002/08/12 01:34:59  steve
 *  conditional ident string using autoconfig.
 *
 * Revision 1.4  2001/11/07 04:01:59  steve
 *  eval_const uses scope instead of a string path.
 *
 * Revision 1.3  2001/11/06 06:11:55  steve
 *  Support more real arithmetic in delay constants.
 *
 * Revision 1.2  2001/07/25 03:10:49  steve
 *  Create a config.h.in file to hold all the config
 *  junk, and support gcc 3.0. (Stephan Boettcher)
 *
 * Revision 1.1  2001/01/14 23:04:56  steve
 *  Generalize the evaluation of floating point delays, and
 *  get it working with delay assignment statements.
 *
 *  Allow parameters to be referenced by hierarchical name.
 *
 */

