/*
 * Copyright (c) 2003 Stephen Williams (steve@icarus.com)
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
#ident "$Id: words.cc,v 1.8 2007/01/16 05:44:17 steve Exp $"
#endif

# include  "compile.h"
# include  "vpi_priv.h"
# include  "array.h"
# include  <stdio.h>
# include  <stdlib.h>
# include  <string.h>
# include  <iostream>
#ifdef HAVE_MALLOC_H
# include  <malloc.h>
#endif
# include  <assert.h>

void compile_var_real(char*label, char*name, int msb, int lsb)
{
      vvp_fun_signal_real*fun = new vvp_fun_signal_real;
      vvp_net_t*net = new vvp_net_t;
      net->fun = fun;
      define_functor_symbol(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      free(name);

      compile_vpi_symbol(label, obj);
      free(label);

      vpip_attach_to_current_scope(obj);
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
static void __compile_var(char*label, char*name, char*array_label,
			  int msb, int lsb, char signed_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_fun_signal*vsig = new vvp_fun_signal(wid);
      vvp_net_t*node = new vvp_net_t;

      vvp_array_t array = array_label? array_find(array_label) : 0;
      assert(array_label? array!=0 : true);

      node->fun = vsig;
      define_functor_symbol(label, node);

	/* Make the vpiHandle for the reg. */
      vpiHandle obj = (signed_flag > 1) ?
			vpip_make_int(name, msb, lsb, node) :
			vpip_make_reg(name, msb, lsb, signed_flag!=0, node);
      compile_vpi_symbol(label, obj);
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      if (name) {
	    assert(!array);
	    vpip_attach_to_current_scope(obj);
      }
	// If this is an array word, then it does not have a name, and
	// it is attached to the addressed array.
      if (array) {
	    assert(!name);
	    array_attach_word(array, obj);
      }
      free(label);
      if (name) free(name);
      if (array_label) free(array_label);
}

void compile_variable(char*label, char*name,
		      int msb, int lsb, char signed_flag)
{
      __compile_var(label, name, 0, msb, lsb, signed_flag);
}

void compile_variablew(char*label, char*array_label,
		       int msb, int lsb, char signed_flag)
{
      __compile_var(label, 0, array_label, msb, lsb, signed_flag);
}

/*
 * Here we handle .net records from the vvp source:
 *
 *    <label> .net   <name>, <msb>, <lsb>, <input> ;
 *    <label> .net/s <name>, <msb>, <lsb>, <input> ;
 *    <label> .net8   <name>, <msb>, <lsb>, <input> ;
 *    <label> .net8/s <name>, <msb>, <lsb>, <input> ;
 *
 * Create a VPI handle to represent it, and fill that handle in with
 * references into the net.
 */
static void __compile_net(char*label, char*name, char*array_label,
			  int msb, int lsb,
			  bool signed_flag, bool net8_flag,
			  unsigned argc, struct symb_s*argv)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*node = new vvp_net_t;

      vvp_array_t array = array_label? array_find(array_label) : 0;
      assert(array_label? array!=0 : true);

      vvp_fun_signal_base*vsig = net8_flag
	    ? dynamic_cast<vvp_fun_signal_base*>(new vvp_fun_signal8(wid))
	    : dynamic_cast<vvp_fun_signal_base*>(new vvp_fun_signal(wid));
      node->fun = vsig;

	/* Add the label into the functor symbol table. */
      define_functor_symbol(label, node);

      assert(argc == 1);

	/* Connect the source to my input. */
      inputs_connect(node, 1, argv);

	/* Make the vpiHandle for the reg. */
      vpiHandle obj = vpip_make_net(name, msb, lsb, signed_flag, node);
      compile_vpi_symbol(label, obj);
      vpip_attach_to_current_scope(obj);
      if (array)
	    array_attach_word(array, obj);

      free(label);
      if (name) free(name);
      if (array_label) free(array_label);
      free(argv);
}

void compile_net(char*label, char*name,
		 int msb, int lsb,
		 bool signed_flag, bool net8_flag,
		 unsigned argc, struct symb_s*argv)
{
      __compile_net(label, name, 0,
		    msb, lsb, signed_flag, net8_flag,
		    argc, argv);
}

void compile_netw(char*label, char*array_label,
		 int msb, int lsb,
		 bool signed_flag, bool net8_flag,
		 unsigned argc, struct symb_s*argv)
{
      __compile_net(label, 0, array_label,
		    msb, lsb, signed_flag, net8_flag,
		    argc, argv);
}

void compile_net_real(char*label, char*name, int msb, int lsb,
		      unsigned argc, struct symb_s*argv)
{
      vvp_net_t*net = new vvp_net_t;

      vvp_fun_signal_real*fun = new vvp_fun_signal_real;
      net->fun = fun;

	/* Add the label into the functor symbol table. */
      define_functor_symbol(label, net);

      assert(argc == 1);

	/* Connect the source to my input. */
      inputs_connect(net, 1, argv);

	/* Make the vpiHandle for the reg. */
      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_symbol(label, obj);
      vpip_attach_to_current_scope(obj);

      free(label);
      free(name);
      free(argv);
}

void compile_alias(char*label, char*name, int msb, int lsb, bool signed_flag,
		 unsigned argc, struct symb_s*argv)
{
      assert(argc == 1);

      vvp_net_t*node = vvp_net_lookup(argv[0].text);

	/* Add the label into the functor symbol table. */
      define_functor_symbol(label, node);


	/* Make the vpiHandle for the reg. */
      vpiHandle obj = vpip_make_net(name, msb, lsb, signed_flag, node);
      compile_vpi_symbol(label, obj);
      vpip_attach_to_current_scope(obj);

      free(label);
      free(name);
      free(argv[0].text);
      free(argv);
}

void compile_alias_real(char*label, char*name, int msb, int lsb,
		      unsigned argc, struct symb_s*argv)
{
      assert(argc == 1);

      vvp_net_t*node = vvp_net_lookup(argv[0].text);

	/* Add the label into the functor symbol table. */
      define_functor_symbol(label, node);


	/* Make the vpiHandle for the reg. */
      vpiHandle obj = vpip_make_real_var(name, node);
      compile_vpi_symbol(label, obj);
      vpip_attach_to_current_scope(obj);

      free(label);
      free(name);
      free(argv[0].text);
      free(argv);
}

/*
 * $Log: words.cc,v $
 * Revision 1.8  2007/01/16 05:44:17  steve
 *  Major rework of array handling. Memories are replaced with the
 *  more general concept of arrays. The NetMemory and NetEMemory
 *  classes are removed from the ivl core program, and the IVL_LPM_RAM
 *  lpm type is removed from the ivl_target API.
 *
 * Revision 1.7  2005/11/27 16:47:14  steve
 *  Fix type safety warning from gcc.
 *
 * Revision 1.6  2005/11/25 17:55:26  steve
 *  Put vec8 and vec4 nets into seperate net classes.
 *
 * Revision 1.5  2005/10/12 17:28:07  steve
 *  Fix compile of net/real aliases.
 *
 * Revision 1.4  2005/10/12 17:23:16  steve
 *  Add alias nodes.
 *
 * Revision 1.3  2005/07/06 04:29:25  steve
 *  Implement real valued signals and arith nodes.
 *
 * Revision 1.2  2003/02/11 05:20:45  steve
 *  Include vpiRealVar objects in vpiVariables scan.
 *
 * Revision 1.1  2003/01/25 23:48:06  steve
 *  Add thread word array, and add the instructions,
 *  %add/wr, %cmp/wr, %load/wr, %mul/wr and %set/wr.
 *
 */

