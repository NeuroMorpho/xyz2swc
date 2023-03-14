//
//
//

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <stdio.h>

#include "Core/String.hpp"


using boost::phoenix::function;
using boost::phoenix::ref;

using boost::spirit::qi::unused_type;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;


///////////////////////////////////////////////////////////////////////////////
//  Our error handler
///////////////////////////////////////////////////////////////////////////////
struct error_handler_
{
    template <typename, typename, typename>
    struct result { typedef void type; };

    template <typename Iterator>
    void operator()(
        qi::info const& what
      , Iterator err_pos, Iterator last) const
    {
        std::cout
            << "Error! Expecting "
            << what                         // what failed?
            << " here: \""
            << String(err_pos, last)   // iterators to error-pos, end
            << "\""
            << std::endl
        ;
    }
};

function<error_handler_> const error_handler = error_handler_();


void print_point(const boost::fusion::vector4<float,float,float,float>& )
{

}

template<typename A, typename B, typename C>
void nothing(const A&, const B&, const C&);


template <typename Iterator>
struct hoc_parser : qi::grammar<Iterator, ascii::space_type>
{
	qi::rule<Iterator, ascii::space_type> start;

	qi::rule<Iterator, qi::locals<float, float, float, float>, ascii::space_type>  pt3dadd;

	qi::rule<Iterator, ascii::space_type>  
		command, 
		pt3dclear,
		connect, connect_AcB, connect_cAB, 
		section_ref,
		create, access, 
		assignment, 
		objref, function_call;

	qi::rule<Iterator> 
		variable,
		section_point, section_location,
		section_id, section_object, 
		section_single, section_array, 
		section_identifier,
		identifier;

	hoc_parser() : hoc_parser::base_type(start)
	{
		using qi::int_;
		using qi::float_;
		using qi::lit;
		using qi::double_;
		using qi::lexeme;
		using qi::alpha;
		using qi::alnum;
		using ascii::space;
		using ascii::char_;
		using qi::on_error;
		using qi::fail;
		using namespace qi::labels;

		start = *(command);

		command = ('{' >> 
			(create | 
			access | 
			connect | 
			pt3dadd | 
			pt3dclear | 
			assignment) >> '}');

		assignment =
			lexeme[
			variable >> space
			>> char_('=') >> space
			>> int_
			]
			;

		variable = 
			alpha >> *(alnum | '_');

		// functions
		pt3dadd = 
			(lit("pt3dadd") >> 
			'('				>> 
			float_[_a = _1] >> 
			','				>> 
			float_[_b = _1] >> 
			','				>> 
			float_[_c = _1] >> 
			','				>> 
			float_[_d = _1] >> 
			lit(')') /*[ ]*/)
			;

		pt3dclear = 
			lit("pt3dclear") >> char_('(') >> char_(')')
			;

		// connections
		connect =
			connect_AcB |
			connect_cAB;

		connect_AcB = 
			lexeme[
			section_id
			>> space >> lit("connect") >> space
			>> section_point
			>> char_(',') 
			>> space
			>> float_
			]
			;

		connect_cAB =
			lit("connect")
			>> section_point >> char_(')') >> section_point;

		// section creation, access
		create = 
			lit("create")
			>> section_object % ',';

		access =
			lit("access")
			>> section_object;

		section_ref = 
			lexeme[
				section_id
				>> space >> identifier >> space
				>> char_('=')
				>> space
				>> lit("new")
				>> space 
				>> lit("SectionRef()")
			]
		;


		// sections
		section_point = 
			section_id >> section_location;

		section_location = 
			char_('(') >> float_ >> char(')');

		section_id = 
			section_array | section_single;

		section_object = 
			section_array | section_single;

		section_single = 
			section_identifier.alias();

		section_array = 
			section_identifier >> '[' >> int_ >> ']';

		section_identifier = identifier.alias();

		//
		//command_list

		//keywords = 

		objref = 
			lit("objref")
			>> section_object % ',';

		//functions = 
		function_call = 
			(lit("define_shape") >> char_('(') >> char_(')'));


		identifier = 
			alpha >> *(alnum | '_');



		//section_var = 
		//	nseg, diam, L, ...


		command.name("command");
		assignment.name("assignment");
		variable.name("variable");
		pt3dadd.name("pt3dadd"); 
		pt3dclear.name("pt3dclear");
		connect.name("connect"); 
		connect_AcB.name("connect_AcB"); 
		connect_cAB.name("connect_cAB");
		create.name("create"); 
		access.name("access");
		section_point.name("section_point"); 
		section_location.name("section_location");
		section_id.name("section_id"); 
		section_object.name("section_object"); 
		section_single.name("section_single"); 
		section_array.name("section_array"); 
		section_identifier.name("section_identifier");

		on_error<fail>(start, error_handler(_4, _3, _2));

		/*keywords = 
			"break",	"else",	"insert",	"setpointer",
			"continue", "em",	"local",	"stop",
			"debug",	"eqn",	"localobj", "strdef",
			"delete",	"for",	"print",	"uninsert",
			"depvar",	"help",	"read",		"while",
			"double",	"if",	"return";*/

		/*math_functions = 
			"abs", "atan", "atan2", "cos", "erf", "erfc", 
			"exp", "int", "log", "log10", "sin", "sqrt", "tanh"
		*/

	}

};




