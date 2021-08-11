/*=============================================================================
    Copyright (c) 2002-2004 Joel de Guzman
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <STX/STX.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

#include <string>
#include <streambuf>

#if defined(_DEBUG)
#define BOOST_SPIRIT_DEBUG
#endif // defined(_DEBUG)

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_utility.hpp>
#include <boost/spirit/include/classic_symbols.hpp>

using namespace boost::spirit::classic;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// parsing helper function

//  Here's our comment rule
struct skip_vsgrammar : public grammar<skip_vsgrammar>
{
    template <typename ScannerT>
    struct definition
    {
        definition(skip_vsgrammar const& /*self*/)
        {
#if defined(BOOST_MSVC) && (BOOST_MSVC <= 1200)

            skip
                =   space_p
                |   "//" >> *(anychar_p - '\n') >> '\n'     // C++ comment
                |   "/*" >> *(anychar_p - "*/") >> '\n'     // C comment
                |   "#line" >> *(anychar_p - '\n') >> '\n'  // added for correctly
                                                            // handling preprocessed
                |   "#pragma" >> *(anychar_p - '\n') >> '\n'// files from Intel V5.0.1
                                                            // on W2K
                ;
#else
            skip
                =   space_p
                |   comment_p("//")                 // C++ comment
                |   comment_p("/*", "*/")           // C comment
                |   comment_p("#line")              // added for correctly
                                                    // handling preprocessed
                |   comment_p("#pragma")            // files from Intel V5.0.1
                                                    // on W2K
                ;
#endif
        }

        rule<ScannerT> skip;

        rule<ScannerT> const&
        start() const { return skip; }
    };
};
namespace boost { namespace spirit { namespace classic { namespace repository
{
    // vs_grammar, as its name suggests, is a very simple lexer that
    // recognises c++ lexical entities such as keywords, identifiers,
    // preprocessor directives, etc., and calls user supplied semantic actions
    // per recognized entity. vs_grammar works on the character level.
    // Be sure to wrap the grammar in a lexeme_d when parsing with a skip
    // parser. See cpp_to_html and quickdoc for applications using this
    // grammar.

    template <typename Actions>
    struct vs_grammar
        : grammar<vs_grammar<Actions> >
    {
        vs_grammar(Actions& actions)
            : actions(actions) {}

        template <typename ScannerT>
        struct definition
        {
#if 0
            definition(vs_grammar const& self)
            {
                Actions& actions = self.actions;

			program
                    =  *space_p >>
                       *(   preprocessor    [actions.tostring]
                        |   comment         [actions.tostring]
                        |   keyword         [actions.tostring]
                        |   identifier      [actions.identifier]
                        |   special         [actions.tostring]

                        |   string          [actions.tostring]
                        |   literal         [actions.tostring]
                        |   number          [actions.tostring]
                        |   anychar_p       [actions.unexpected]
                        )
                    ;

                preprocessor
                    =   '#' >> *space_p >> identifier
                    ;

                comment
                    =   +((comment_p("//") | comment_p("/*", "*/"))
                        >> *space_p)
                    ;

                keyword
                    =   keywords >> (eps_p - (alnum_p | '_')) >> *space_p
                    ;   // make sure we recognize whole words only
#if 0
static const char * const yytname[] = {   "$","error","$undefined.","ATTRIBUTE",
"CONST_QUAL","BOOL_TYPE","FLOAT_TYPE","INT_TYPE","STRING_TYPE","BREAK","CONTINUE",
"DO","ELSE","FOR","IF","DISCARD","RETURN","BVEC2","BVEC3","BVEC4","IVEC2","IVEC3",
"IVEC4","VEC2","VEC3","VEC4","MATRIX2","MATRIX3","MATRIX4","IN_QUAL","OUT_QUAL",
"INOUT_QUAL","UNIFORM","VARYING","STRUCT","VOID_TYPE","WHILE","SAMPLER1D","SAMPLER2D",
"SAMPLER3D","SAMPLERCUBE","SAMPLER1DSHADOW","SAMPLER2DSHADOW","SAMPLERGENERIC",
"VECTOR","MATRIX","REGISTER","TEXTURE","SAMPLERSTATE","IDENTIFIER","TYPE_NAME",
"FLOATCONSTANT","INTCONSTANT","BOOLCONSTANT","STRINGCONSTANT","FIELD_SELECTION",
"LEFT_OP","RIGHT_OP","INC_OP","DEC_OP","LE_OP","GE_OP","EQ_OP","NE_OP","AND_OP",
"OR_OP","XOR_OP","MUL_ASSIGN","DIV_ASSIGN","ADD_ASSIGN","MOD_ASSIGN","LEFT_ASSIGN",
"RIGHT_ASSIGN","AND_ASSIGN","XOR_ASSIGN","OR_ASSIGN","SUB_ASSIGN","LEFT_PAREN",
"RIGHT_PAREN","LEFT_BRACKET","RIGHT_BRACKET","LEFT_BRACE","RIGHT_BRACE","DOT",
"COMMA","COLON","EQUAL","SEMICOLON","BANG","DASH","TILDE","PLUS","STAR","SLASH",
"PERCENT","LEFT_ANGLE","RIGHT_ANGLE","VERTICAL_BAR","CARET","AMPERSAND","QUESTION",
"variable_identifier","primary_expression","postfix_expression","integer_expression",
"function_call","function_call_or_method","function_call_generic","function_call_header_no_parameters",
"function_call_header_with_parameters","function_call_header","function_identifier",
"unary_expression","unary_operator","multiplicative_expression","additive_expression",
"shift_expression","relational_expression","equality_expression","and_expression",
"exclusive_or_expression","inclusive_or_expression","logical_and_expression",
"logical_xor_expression","logical_or_expression","conditional_expression","assignment_expression",
"assignment_operator","expression","constant_expression","declaration","function_prototype",
"function_declarator","function_header_with_parameters","function_header","parameter_declarator",
"parameter_declaration","parameter_qualifier","parameter_type_specifier","init_declarator_list",
"single_declaration","fully_specified_type","type_qualifier","type_specifier",
"type_specifier_nonarray","struct_specifier","struct_declaration_list","struct_declaration",
"struct_declarator_list","struct_declarator","initializer","declaration_statement",
"statement","simple_statement","compound_statement","@1","@2","statement_no_new_scope",
"compound_statement_no_new_scope","statement_list","expression_statement","selection_statement",
"selection_rest_statement","condition","iteration_statement","@3","@4","@5",
"for_init_statement","conditionopt","for_rest_statement","jump_statement","translation_unit",
"external_declaration","function_definition","@6","initialization_list","initializer_list",
"annotation","annotation_list","annotation_item","ann_type","ann_literal","ann_numerical_constant",
"ann_literal_constructor","ann_value_list","ann_literal_init_list","register_specifier",
"semantic","type_info","sampler_initializer","sampler_init_list","sampler_init_item",
""
#endif
                keywords
                    = "bool", "break",

					//"bvec2","bvec3","bvec4",

					"continue",
					"discard", "do", "else" "float",
					"float2","float3","float4","float4x4",
					"for", "if", "in", "inout",
					"int",

					//"ivec2","ivec3","ivec4",
					"matrix",
					//"matrix2","matrix3",
					//"matrix4",

					"out", "register", "return", "sampler1d",
					"sampler2d", "sampler3d","samplercube","sampler1dshadow",
					"sampler2dshadow","samplergeneric", "samplerstate", "string",
					"struct", "texture", "uniform",
					//"varying",
					"vector",

					//"vec2","vec3","vec4",

					"void", "while"
                    ;

                special
                    =   +chset_p("~!%^&*()+={[}]:;,<.>?/|\\-") >> *space_p
                    ;

                string
                    =   !as_lower_d['l'] >> confix_p('"', *c_escape_ch_p, '"')
                        >> *space_p
                    ;

                literal
                    =   !as_lower_d['l'] >> confix_p('\'', *c_escape_ch_p, '\'')
                        >> *space_p
                    ;

                number
                    =   (   real_p
                        |   as_lower_d["0x"] >> hex_p
                        |   '0' >> oct_p
                        )
                        >>  *as_lower_d[chset_p("ldfu")]
                        >>  *space_p
                    ;

                identifier
                    =   ((alpha_p | '_') >> *(alnum_p | '_'))
                        >> *space_p
                    ;
            }

            rule<ScannerT>
                program, preprocessor, comment, special, string, literal,
                number, identifier, keyword;

            symbols<>
                keywords;

            rule<ScannerT> const&
            start() const
            {
                return program;
            }
#else
        definition(vs_grammar const& self) :
            ELLIPSIS("..."), RIGHT_ASSIGN(">>="), LEFT_ASSIGN("<<="),
            ADD_ASSIGN("+="), SUB_ASSIGN("-="), MUL_ASSIGN("*="),
            DIV_ASSIGN("/="), MOD_ASSIGN("%="), AND_ASSIGN("&="),
            XOR_ASSIGN("^="), OR_ASSIGN("|="), RIGHT_OP(">>"), LEFT_OP("<<"),
            INC_OP("++"), DEC_OP("--"), PTR_OP("->"), AND_OP("&&"),
            OR_OP("||"), LE_OP("<="), GE_OP(">="), EQ_OP("=="), NE_OP("!="),
            SEMICOLON(';'),
            COMMA(','), COLON(':'), ASSIGN('='), LEFT_PAREN('('),
            RIGHT_PAREN(')'), DOT('.'), ADDROF('&'), BANG('!'), TILDE('~'),
            MINUS('-'), PLUS('+'), STAR('*'), SLASH('/'), PERCENT('%'),
            LT_OP('<'), GT_OP('>'), XOR('^'), OR('|'), QUEST('?')
        {
			Actions& actions = self.actions;
        
        // C keywords
            keywords = 

                "break", "continue", "discard", "do", "for", "if", "switch", "while", 
		"int", "return", "struct", 
/*
POSITION,
NORMAL
COLOR
TEXCOORD0

SV_Position
Normal
Color
Texcoord0
*/
/*
Vertex Shader Semantics

Input	Description	Type
BINORMAL[n]	Binormal	float4
BLENDINDICES[n]	Blend indices	uint
BLENDWEIGHT[n]	Blend weights	float
COLOR[n]	Diffuse and specular color	float4
NORMAL[n]	Normal vector	float4
POSITION[n]	Vertex position in object space.	float4
POSITIONT	Transformed vertex position.	float4
PSIZE[n]	Point size	float
TANGENT[n]	Tangent	float4
TEXCOORD[n]	Texture coordinates	float4
Output	Description	Type
COLOR[n]	Diffuse or specular color 	float4
FOG	Vertex fog	float
POSITION[n]	Position of a vertex in homogenous space. Compute position in screen-space by dividing (x,y,z) by w. Every vertex shader must write out a parameter with this semantic.	float4
PSIZE	Point size	float
TESSFACTOR[n]	Tessellation factor	float
TEXCOORD[n]	Texture coordinates	float4
*/
/*
Pixel Shader Semantics
 
	float
VPOS	The pixel location (x,y) in screen space. To convert a Direct3D 9 shader (that uses this semantic) to a Direct3D 10 and later shader, see Direct3D 9 VPOS and Direct3D 10 SV_Position)	float2
Output	Description	Type
COLOR[n]	Output color 	float4
DEPTH[n]	Output depth	float
*/
		"uniform", "bool", "float",

		"float4x4", "float2", "float3", "float4", "technique", "pass", "compile", "VertexShader", "PixelShader", "sampler1D", "sampler2D", "sampler3D", "samplerCUBE", "sampler_state", "sampler", "texture", "texture1D", "texture2D", "texture3D", "textureCube", "Texture", "MipFilter", "LINEAR", "MinFilter", "MagFilter", "vs_2_0", "ps_2_0", "vs_3_0", "ps_3_0",
//1:
"abs", "acos", "all", "any", "asin", "atan", "atan2", "ceil", "clamp", "clip", "cos", "cosh", "cross", "degrees", "determinant", "D3DCOLORtoUBYTE4", "distance", "dot", "exp", "exp2", "faceforward", "floor", "fmod", "frac", "isfinite", "isinf", "isnan", "ldexp", "length", "lerp", "lit", "log", "log10", "log2", "max", "min", "modf", "mul", "noise", "normalize", "pow", "radians", "reflect", "refract", "round", "rsqrt", "saturate", "sign", "sin", "sincos", "sinh", "smoothstep", "sqrt", "step", "tan", "tanh", "tex1D", "tex2D", "tex3D", "texCUBE", "transpose", "trunc",
//2:
"ddx", "ddy", "frexp", "fwidth", "tex1Dbias", "tex1Dgrad", "tex1Dproj", "tex2Dbias", "tex2Dgrad", "tex2Dproj", "tex3Dbias", "tex3Dgrad", "tex3Dproj", "texCUBEbias", "texCUBEgrad", "texCUBEproj",
//3:
"tex1Dlod", "tex2Dlod", "tex3Dlod", "texCUBElod",
//4:
"abort", "asfloat", "asuint", "errorf", "GetRenderTargetSampleCount", "GetRenderTargetSamplePosition", "LOG_PRINT",
//5:
"AllMemoryBarrier", "AllMemoryBarrierWithGroupSync", "asdouble", "asuint", "CheckAccessFullyMapped", "countbits", "ddx_coarse", "ddx_fine", "ddy_coarse", "ddy_fine", "DeviceMemoryBarrier", "DeviceMemoryBarrierWithGroupSync", "dst", "EvaluateAttributeAtCentroid", "EvaluateAttributeAtSample", "EvaluateAttributeSnapped", "f16tof32", "f32tof16", "firstbithigh", "firstbitlow", "fma", "GroupMemoryBarrier", "GroupMemoryBarrierWithGroupSync", "InterlockedAdd", "InterlockedAnd", "InterlockedCompareExchange", "InterlockedCompareStore", "InterlockedExchange", "InterlockedMax", "InterlockedMin", "InterlockedOr", "InterlockedXor", "mad", "msad4", "Process2DQuadTessFactorsAvg", "Process2DQuadTessFactorsMax", "Process2DQuadTessFactorsMin", "ProcessIsolineTessFactors", "ProcessQuadTessFactorsAvg", "ProcessQuadTessFactorsMax", "ProcessQuadTessFactorsMin", "ProcessTriTessFactorsAvg", "ProcessTriTessFactorsMax", "ProcessTriTessFactorsMin", "rcp", "reversebits";
					
        // C operators
            LEFT_BRACE = chlit<>('{') | strlit<>("<%");
            RIGHT_BRACE = chlit<>('}') | strlit<>("%>");
            LEFT_BRACKET = chlit<>('[') | strlit<>("<:");
            RIGHT_BRACKET = chlit<>(']') | strlit<>(":>");

        // Tokens
		UNIFORM =      strlit<>("uniform");

            BREAK =      strlit<>("break");
            CONTINUE =   strlit<>("continue");
            DISCARD =    strlit<>("discard");
            DO =         strlit<>("do");
	    FOR =        strlit<>("for");
	    IF =         strlit<>("if");
	    SWITCH =     strlit<>("switch");
	    WHILE =      strlit<>("while");

            INT =        strlit<>("int");
            BOOL =        strlit<>("bool");
            RETURN =     strlit<>("return");
            STRUCT =     strlit<>("struct");

            FLOAT =       strlit<>("float");
            FLOAT2 =      strlit<>("float2");
            FLOAT3 =      strlit<>("float3");
            FLOAT4 =      strlit<>("float4");
            FLOAT4x4 =    strlit<>("float4x4");

	    TECHNIQUE=    strlit<>("technique");
	    PASS=      	  strlit<>("pass");
	    COMPILE=      strlit<>("compile");
	    VERTEXSHADER= strlit<>("VertexShader");
	    PIXELSHADER=  strlit<>("PixelShader");

	    SAMPLER=      strlit<>("sampler");
	    SAMPLER1D=      strlit<>("sampler1D");
	    SAMPLER2D=      strlit<>("sampler2D");
	    SAMPLER3D=      strlit<>("sampler3D");
	    SAMPLERCUBE=      strlit<>("samplerCUBE");

	    SAMPLER_STATE=      strlit<>("sampler_state");
	    TEXTURE=      strlit<>("texture");

	    TEXTURE1D=      strlit<>("texture1D");
	    TEXTURE2D=      strlit<>("texture2D");
	    TEXTURE3D=      strlit<>("texture3D");
	    TEXTURECUBE=      strlit<>("textureCube");

	    tEXTURE=      strlit<>("Texture");
	    MIPFILTER=      strlit<>("MipFilter");
	    LINEAR=      strlit<>("LINEAR");
	    MINFILTER=      strlit<>("MinFilter");
	    MAGFILTER=      strlit<>("MagFilter");
	    VS_2_0=      strlit<>("vs_2_0");
	    PS_2_0=      strlit<>("ps_2_0");
	    VS_3_0=      strlit<>("vs_3_0");
	    PS_3_0=      strlit<>("ps_3_0");
	    
//1:
ABS =		strlit<>("abs");
ACOS =		strlit<>("acos");
ALL =		strlit<>("all");
ANY =		strlit<>("any");
ASIN =		strlit<>("asin");
ATAN =		strlit<>("atan");
ATAN2 =		strlit<>("atan2");
CEIL =		strlit<>("ceil");
CLAMP =		strlit<>("clamp");
CLIP =		strlit<>("clip");
COS =		strlit<>("cos");
COSH =		strlit<>("cosh");
CROSS =		strlit<>("cross");
DEGREES =		strlit<>("degrees");
DETERMINANT =		strlit<>("determinant");
D3DCOLORTOUBYTE4 =		strlit<>("D3DCOLORtoUBYTE4");
DISTANCE =		strlit<>("distance");
DOT_ =		strlit<>("dot");
EXP =		strlit<>("exp");
EXP2 =		strlit<>("exp2");
FACEFORWARD =		strlit<>("faceforward");
FLOOR =		strlit<>("floor");
FMOD =		strlit<>("fmod");
FRAC =		strlit<>("frac");
ISFINITE =		strlit<>("isfinite");
ISINF =		strlit<>("isinf");
ISNAN =		strlit<>("isnan");
LDEXP =		strlit<>("ldexp");
LENGTH =		strlit<>("length");
LERP =		strlit<>("lerp");
LIT =		strlit<>("lit");
LOG =		strlit<>("log");
LOG10 =		strlit<>("log10");
LOG2 =		strlit<>("log2");
MAX =		strlit<>("max");
MIN =		strlit<>("min");
MODF =		strlit<>("modf");
MUL =		strlit<>("mul");
NOISE =		strlit<>("noise");
NORMALIZE =		strlit<>("normalize");
POW =		strlit<>("pow");
RADIANS =		strlit<>("radians");
REFLECT =		strlit<>("reflect");
REFRACT =		strlit<>("refract");
ROUND =		strlit<>("round");
RSQRT =		strlit<>("rsqrt");
SATURATE =		strlit<>("saturate");
SIGN =		strlit<>("sign");
SIN =		strlit<>("sin");
SINCOS =		strlit<>("sincos");
SINH =		strlit<>("sinh");
SMOOTHSTEP =		strlit<>("smoothstep");
SQRT =		strlit<>("sqrt");
STEP =		strlit<>("step");
TAN =		strlit<>("tan");
TANH =		strlit<>("tanh");
TEX1D =		strlit<>("tex1D");
TEX2D =		strlit<>("tex2D");
TEX3D =		strlit<>("tex3D");
TEXCUBE =		strlit<>("texCUBE");
TRANSPOSE =		strlit<>("transpose");
TRUNC =		strlit<>("trunc");
//2:
DDX =		strlit<>("ddx");
DDY =		strlit<>("ddy");
FREXP =		strlit<>("frexp");
FWIDTH =		strlit<>("fwidth");
TEX1DBIAS =		strlit<>("tex1Dbias");
TEX1DGRAD =		strlit<>("tex1Dgrad");
TEX1DPROJ =		strlit<>("tex1Dproj");
TEX2DBIAS =		strlit<>("tex2Dbias");
TEX2DGRAD =		strlit<>("tex2Dgrad");
TEX2DPROJ =		strlit<>("tex2Dproj");
TEX3DBIAS =		strlit<>("tex3Dbias");
TEX3DGRAD =		strlit<>("tex3Dgrad");
TEX3DPROJ =		strlit<>("tex3Dproj");
TEXCUBEBIAS =		strlit<>("texCUBEbias");
TEXCUBEGRAD =		strlit<>("texCUBEgrad");
TEXCUBEPROJ =		strlit<>("texCUBEproj");
//3:
TEX1DLOD =		strlit<>("tex1Dlod");
TEX2DLOD =		strlit<>("tex2Dlod");
TEX3DLOD =		strlit<>("tex3Dlod");
TEXCUBELOD =		strlit<>("texCUBElod");
//4:
ABORT =		strlit<>("abort");
ASFLOAT =		strlit<>("asfloat");
ASUINT =		strlit<>("asuint");
ERRORF =		strlit<>("errorf");
GETRENDERTARGETSAMPLECOUNT =		strlit<>("GetRenderTargetSampleCount");
GETRENDERTARGETSAMPLEPOSITION =		strlit<>("GetRenderTargetSamplePosition");
PRINTF =		strlit<>("LOG_PRINT");
//5:
ALLMEMORYBARRIER =		strlit<>("AllMemoryBarrier");
ALLMEMORYBARRIERWITHGROUPSYNC =		strlit<>("AllMemoryBarrierWithGroupSync");
ASDOUBLE =		strlit<>("asdouble");
CHECKACCESSFULLYMAPPED =		strlit<>("CheckAccessFullyMapped");
COUNTBITS =		strlit<>("countbits");
DDX_COARSE =		strlit<>("ddx_coarse");
DDX_FINE =		strlit<>("ddx_fine");
DDY_COARSE =		strlit<>("ddy_coarse");
DDY_FINE =		strlit<>("ddy_fine");
DEVICEMEMORYBARRIER =		strlit<>("DeviceMemoryBarrier");
DEVICEMEMORYBARRIERWITHGROUPSYNC =		strlit<>("DeviceMemoryBarrierWithGroupSync");
DST =		strlit<>("dst");
EVALUATEATTRIBUTEATCENTROID =		strlit<>("EvaluateAttributeAtCentroid");
EVALUATEATTRIBUTEATSAMPLE =		strlit<>("EvaluateAttributeAtSample");
EVALUATEATTRIBUTESNAPPED =		strlit<>("EvaluateAttributeSnapped");
F16TOF32 =		strlit<>("f16tof32");
F32TOF16 =		strlit<>("f32tof16");
FIRSTBITHIGH =		strlit<>("firstbithigh");
FIRSTBITLOW =		strlit<>("firstbitlow");
FMA =		strlit<>("fma");
GROUPMEMORYBARRIER =		strlit<>("GroupMemoryBarrier");
GROUPMEMORYBARRIERWITHGROUPSYNC =		strlit<>("GroupMemoryBarrierWithGroupSync");
INTERLOCKEDADD =		strlit<>("InterlockedAdd");
INTERLOCKEDAND =		strlit<>("InterlockedAnd");
INTERLOCKEDCOMPAREEXCHANGE =		strlit<>("InterlockedCompareExchange");
INTERLOCKEDCOMPARESTORE =		strlit<>("InterlockedCompareStore");
INTERLOCKEDEXCHANGE =		strlit<>("InterlockedExchange");
INTERLOCKEDMAX =		strlit<>("InterlockedMax");
INTERLOCKEDMIN =		strlit<>("InterlockedMin");
INTERLOCKEDOR =		strlit<>("InterlockedOr");
INTERLOCKEDXOR =		strlit<>("InterlockedXor");
MAD =		strlit<>("mad");
MSAD4 =		strlit<>("msad4");
PROCESS2DQUADTESSFACTORSAVG =		strlit<>("Process2DQuadTessFactorsAvg");
PROCESS2DQUADTESSFACTORSMAX =		strlit<>("Process2DQuadTessFactorsMax");
PROCESS2DQUADTESSFACTORSMIN =		strlit<>("Process2DQuadTessFactorsMin");
PROCESSISOLINETESSFACTORS =		strlit<>("ProcessIsolineTessFactors");
PROCESSQUADTESSFACTORSAVG =		strlit<>("ProcessQuadTessFactorsAvg");
PROCESSQUADTESSFACTORSMAX =		strlit<>("ProcessQuadTessFactorsMax");
PROCESSQUADTESSFACTORSMIN =		strlit<>("ProcessQuadTessFactorsMin");
PROCESSTRITESSFACTORSAVG =		strlit<>("ProcessTriTessFactorsAvg");
PROCESSTRITESSFACTORSMAX =		strlit<>("ProcessTriTessFactorsMax");
PROCESSTRITESSFACTORSMIN =		strlit<>("ProcessTriTessFactorsMin");
RCP =		strlit<>("rcp");
REVERSEBITS =		strlit<>("reversebits");

/*
technique10
SetVertexShader
CompileShader
vs_4_0
SetGeometryShader
gs_4_0
SetPixelShader

Shader Model 4:
texture1DArray
texture2DArray
texture2DMS
texture2DMSArray
textureCubeArray
*/

/*
cs_4_0 cs_4_1 cs_5_0 ds_5_0 fx_2_0 fx_4_0 fx_4_1 fx_5_0 gs_4_0
gs_4_1 gs_5_0 hs_5_0 ps_2_0 ps_2_a ps_2_b ps_2_sw ps_3_0 ps_3_sw ps_4_0
ps_4_0_level_9_1 ps_4_0_level_9_3 ps_4_0_level_9_0 ps_4_1 ps_5_0 tx_1_0
vs_1_1 vs_2_0 vs_2_a vs_2_sw vs_3_0 vs_3_sw vs_4_0 vs_4_0_level_9_1
vs_4_0_level_9_3 vs_4_0_level_9_0 vs_4_1 vs_5_0
*/
        // C identifiers
                lexeme_d[
                    ((alpha_p | '_' | '$') >> *(alnum_p | '_' | '$'))
                    - (keywords >> anychar_p - (alnum_p | '_' | '$'))
                ]
                ;

        // string literals
            STRING_LITERAL_PART =
                lexeme_d[
                    !chlit<>('L') >> chlit<>('\"') >>
                    *( strlit<>("\\\"") | anychar_p - chlit<>('\"') ) >>
                    chlit<>('\"')
                ]
                ;

            STRING_LITERAL = +STRING_LITERAL_PART;

        // integer constants
            INT_CONSTANT_HEX
                = lexeme_d[
                    chlit<>('0')
                    >> as_lower_d[chlit<>('x')]
                    >> +xdigit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                ]
                ;

            INT_CONSTANT_OCT
                = lexeme_d[
                    chlit<>('0')
                    >> +range<>('0', '7')
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                ]
                ;

            INT_CONSTANT_DEC
                = lexeme_d[
                    +digit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                ]
                ;

            INT_CONSTANT_CHAR
                = lexeme_d[
                    !chlit<>('L') >> chlit<>('\'') >>
                    longest_d[
                            anychar_p
                        |   (   chlit<>('/')
                                >> chlit<>('0')
                                >> repeat_p(0, 2)[range<>('0', '7')]
                            )
                        |   (chlit<>('/') >> anychar_p)
                    ] >>
                    chlit<>('\'')
                ]
                ;

            INT_CONSTANT =
                    INT_CONSTANT_HEX
                |   INT_CONSTANT_OCT
                |   INT_CONSTANT_DEC
                |   INT_CONSTANT_CHAR
                ;

        // float constants
            FLOAT_CONSTANT_1    // 12345[eE][+-]123[lLfF]?
                = lexeme_d[
                    +digit_p
                    >> (chlit<>('e') | chlit<>('E'))
                    >> !(chlit<>('+') | chlit<>('-'))
                    >> +digit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                ]
                ;

            FLOAT_CONSTANT_2    // .123([[eE][+-]123)?[lLfF]?
                = lexeme_d[
                    *digit_p
                    >> chlit<>('.')
                    >> +digit_p
                    >> !(   (chlit<>('e') | chlit<>('E'))
                            >> !(chlit<>('+') | chlit<>('-'))
                            >> +digit_p
                        )
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                ]
                ;

            FLOAT_CONSTANT_3    // 12345.([[eE][+-]123)?[lLfF]?
                = lexeme_d[
                    +digit_p
                    >> chlit<>('.')
                    >> *digit_p
                    >> !(   (chlit<>('e') | chlit<>('E'))
                            >> !(chlit<>('+') | chlit<>('-'))
                            >> +digit_p
                        )
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                ]
                ;

            FLOAT_CONSTANT =
                    FLOAT_CONSTANT_1
                |   FLOAT_CONSTANT_2
                |   FLOAT_CONSTANT_3
                ;
	    
            CONSTANT = longest_d[FLOAT_CONSTANT | INT_CONSTANT];

        // debug support for terminals
	    BOOST_SPIRIT_DEBUG_RULE(BREAK);
	    BOOST_SPIRIT_DEBUG_RULE(CONTINUE);
	    BOOST_SPIRIT_DEBUG_RULE(DISCARD);
	    BOOST_SPIRIT_DEBUG_RULE(DO);
	    BOOST_SPIRIT_DEBUG_RULE(FOR);
	    BOOST_SPIRIT_DEBUG_RULE(IF);
	    BOOST_SPIRIT_DEBUG_RULE(SWITCH);
	    BOOST_SPIRIT_DEBUG_RULE(WHILE);
	    BOOST_SPIRIT_DEBUG_RULE(INT);
	    BOOST_SPIRIT_DEBUG_RULE(RETURN);
	    BOOST_SPIRIT_DEBUG_RULE(STRUCT);
	    BOOST_SPIRIT_DEBUG_RULE(UNIFORM);
	    BOOST_SPIRIT_DEBUG_RULE(BOOL);
	    BOOST_SPIRIT_DEBUG_RULE(FLOAT);
	    BOOST_SPIRIT_DEBUG_RULE(FLOAT2);
	    BOOST_SPIRIT_DEBUG_RULE(FLOAT3);
	    BOOST_SPIRIT_DEBUG_RULE(FLOAT4);
	    //BOOST_SPIRIT_DEBUG_RULE(FLOAT4X4);
	    BOOST_SPIRIT_DEBUG_RULE(TECHNIQUE);
	    BOOST_SPIRIT_DEBUG_RULE(PASS);
	    BOOST_SPIRIT_DEBUG_RULE(COMPILE);
	    BOOST_SPIRIT_DEBUG_RULE(VERTEXSHADER);
	    BOOST_SPIRIT_DEBUG_RULE(PIXELSHADER);
	    BOOST_SPIRIT_DEBUG_RULE(SAMPLER);
	    BOOST_SPIRIT_DEBUG_RULE(SAMPLER1D);
	    BOOST_SPIRIT_DEBUG_RULE(SAMPLER2D);
	    BOOST_SPIRIT_DEBUG_RULE(SAMPLER3D);
	    BOOST_SPIRIT_DEBUG_RULE(SAMPLERCUBE);
	    BOOST_SPIRIT_DEBUG_RULE(SAMPLER_STATE);
	    BOOST_SPIRIT_DEBUG_RULE(TEXTURE);
	    BOOST_SPIRIT_DEBUG_RULE(TEXTURE1D);
	    BOOST_SPIRIT_DEBUG_RULE(TEXTURE2D);
	    BOOST_SPIRIT_DEBUG_RULE(TEXTURE3D);
	    BOOST_SPIRIT_DEBUG_RULE(TEXTURECUBE);
	    BOOST_SPIRIT_DEBUG_RULE(TEXTURE);
	    BOOST_SPIRIT_DEBUG_RULE(MIPFILTER);
	    BOOST_SPIRIT_DEBUG_RULE(LINEAR);
	    BOOST_SPIRIT_DEBUG_RULE(MINFILTER);
	    BOOST_SPIRIT_DEBUG_RULE(MAGFILTER);
	    BOOST_SPIRIT_DEBUG_RULE(VS_2_0);
	    BOOST_SPIRIT_DEBUG_RULE(PS_2_0);
	    BOOST_SPIRIT_DEBUG_RULE(VS_3_0);
	    BOOST_SPIRIT_DEBUG_RULE(PS_3_0);
	    BOOST_SPIRIT_DEBUG_RULE(ABS);
	    BOOST_SPIRIT_DEBUG_RULE(ACOS);
	    BOOST_SPIRIT_DEBUG_RULE(ALL);
	    BOOST_SPIRIT_DEBUG_RULE(ANY);
	    BOOST_SPIRIT_DEBUG_RULE(ASIN);
	    BOOST_SPIRIT_DEBUG_RULE(ATAN);
	    BOOST_SPIRIT_DEBUG_RULE(ATAN2);
	    BOOST_SPIRIT_DEBUG_RULE(CEIL);
	    BOOST_SPIRIT_DEBUG_RULE(CLAMP);
	    BOOST_SPIRIT_DEBUG_RULE(CLIP);
	    BOOST_SPIRIT_DEBUG_RULE(COS);
	    BOOST_SPIRIT_DEBUG_RULE(COSH);
	    BOOST_SPIRIT_DEBUG_RULE(CROSS);
	    BOOST_SPIRIT_DEBUG_RULE(DEGREES);
	    BOOST_SPIRIT_DEBUG_RULE(DETERMINANT);
	    BOOST_SPIRIT_DEBUG_RULE(D3DCOLORTOUBYTE4);
	    BOOST_SPIRIT_DEBUG_RULE(DISTANCE);
	    BOOST_SPIRIT_DEBUG_RULE(DOT);
	    BOOST_SPIRIT_DEBUG_RULE(EXP);
	    BOOST_SPIRIT_DEBUG_RULE(EXP2);
	    BOOST_SPIRIT_DEBUG_RULE(FACEFORWARD);
	    BOOST_SPIRIT_DEBUG_RULE(FLOOR);
	    BOOST_SPIRIT_DEBUG_RULE(FMOD);
	    BOOST_SPIRIT_DEBUG_RULE(FRAC);
	    BOOST_SPIRIT_DEBUG_RULE(ISFINITE);
	    BOOST_SPIRIT_DEBUG_RULE(ISINF);
	    BOOST_SPIRIT_DEBUG_RULE(ISNAN);
	    BOOST_SPIRIT_DEBUG_RULE(LDEXP);
	    BOOST_SPIRIT_DEBUG_RULE(LENGTH);
	    BOOST_SPIRIT_DEBUG_RULE(LERP);
	    BOOST_SPIRIT_DEBUG_RULE(LIT);
	    BOOST_SPIRIT_DEBUG_RULE(LOG);
	    BOOST_SPIRIT_DEBUG_RULE(LOG10);
	    BOOST_SPIRIT_DEBUG_RULE(LOG2);
	    BOOST_SPIRIT_DEBUG_RULE(MAX);
	    BOOST_SPIRIT_DEBUG_RULE(MIN);
	    BOOST_SPIRIT_DEBUG_RULE(MODF);
	    BOOST_SPIRIT_DEBUG_RULE(MUL);
	    BOOST_SPIRIT_DEBUG_RULE(NOISE);
	    BOOST_SPIRIT_DEBUG_RULE(NORMALIZE);
	    BOOST_SPIRIT_DEBUG_RULE(POW);
	    BOOST_SPIRIT_DEBUG_RULE(RADIANS);
	    BOOST_SPIRIT_DEBUG_RULE(REFLECT);
	    BOOST_SPIRIT_DEBUG_RULE(REFRACT);
	    BOOST_SPIRIT_DEBUG_RULE(ROUND);
	    BOOST_SPIRIT_DEBUG_RULE(RSQRT);
	    BOOST_SPIRIT_DEBUG_RULE(SATURATE);
	    BOOST_SPIRIT_DEBUG_RULE(SIGN);
	    BOOST_SPIRIT_DEBUG_RULE(SIN);
	    BOOST_SPIRIT_DEBUG_RULE(SINCOS);
	    BOOST_SPIRIT_DEBUG_RULE(SINH);
	    BOOST_SPIRIT_DEBUG_RULE(SMOOTHSTEP);
	    BOOST_SPIRIT_DEBUG_RULE(SQRT);
	    BOOST_SPIRIT_DEBUG_RULE(STEP);
	    BOOST_SPIRIT_DEBUG_RULE(TAN);
	    BOOST_SPIRIT_DEBUG_RULE(TANH);
	    BOOST_SPIRIT_DEBUG_RULE(TEX1D);
	    BOOST_SPIRIT_DEBUG_RULE(TEX2D);
	    BOOST_SPIRIT_DEBUG_RULE(TEX3D);
	    BOOST_SPIRIT_DEBUG_RULE(TEXCUBE);
	    BOOST_SPIRIT_DEBUG_RULE(TRANSPOSE);
	    BOOST_SPIRIT_DEBUG_RULE(TRUNC);
	    BOOST_SPIRIT_DEBUG_RULE(DDX);
	    BOOST_SPIRIT_DEBUG_RULE(DDY);
	    BOOST_SPIRIT_DEBUG_RULE(FREXP);
	    BOOST_SPIRIT_DEBUG_RULE(FWIDTH);
	    BOOST_SPIRIT_DEBUG_RULE(TEX1DBIAS);
	    BOOST_SPIRIT_DEBUG_RULE(TEX1DGRAD);
	    BOOST_SPIRIT_DEBUG_RULE(TEX1DPROJ);
	    BOOST_SPIRIT_DEBUG_RULE(TEX2DBIAS);
	    BOOST_SPIRIT_DEBUG_RULE(TEX2DGRAD);
	    BOOST_SPIRIT_DEBUG_RULE(TEX2DPROJ);
	    BOOST_SPIRIT_DEBUG_RULE(TEX3DBIAS);
	    BOOST_SPIRIT_DEBUG_RULE(TEX3DGRAD);
	    BOOST_SPIRIT_DEBUG_RULE(TEX3DPROJ);
	    BOOST_SPIRIT_DEBUG_RULE(TEXCUBEBIAS);
	    BOOST_SPIRIT_DEBUG_RULE(TEXCUBEGRAD);
	    BOOST_SPIRIT_DEBUG_RULE(TEXCUBEPROJ);
	    BOOST_SPIRIT_DEBUG_RULE(TEX1DLOD);
	    BOOST_SPIRIT_DEBUG_RULE(TEX2DLOD);
	    BOOST_SPIRIT_DEBUG_RULE(TEX3DLOD);
	    BOOST_SPIRIT_DEBUG_RULE(TEXCUBELOD);
	    BOOST_SPIRIT_DEBUG_RULE(ABORT);
	    BOOST_SPIRIT_DEBUG_RULE(ASFLOAT);
	    BOOST_SPIRIT_DEBUG_RULE(ASUINT);
	    BOOST_SPIRIT_DEBUG_RULE(ERRORF);
	    BOOST_SPIRIT_DEBUG_RULE(GETRENDERTARGETSAMPLECOUNT);
	    BOOST_SPIRIT_DEBUG_RULE(GETRENDERTARGETSAMPLEPOSITION);
	    BOOST_SPIRIT_DEBUG_RULE(PRINTF);
	    BOOST_SPIRIT_DEBUG_RULE(ALLMEMORYBARRIER);
	    BOOST_SPIRIT_DEBUG_RULE(ALLMEMORYBARRIERWITHGROUPSYNC);
	    BOOST_SPIRIT_DEBUG_RULE(ASDOUBLE);
	    BOOST_SPIRIT_DEBUG_RULE(ASUINT);
	    BOOST_SPIRIT_DEBUG_RULE(CHECKACCESSFULLYMAPPED);
	    BOOST_SPIRIT_DEBUG_RULE(COUNTBITS);
	    BOOST_SPIRIT_DEBUG_RULE(DDX_COARSE);
	    BOOST_SPIRIT_DEBUG_RULE(DDX_FINE);
	    BOOST_SPIRIT_DEBUG_RULE(DDY_COARSE);
	    BOOST_SPIRIT_DEBUG_RULE(DDY_FINE);
	    BOOST_SPIRIT_DEBUG_RULE(DEVICEMEMORYBARRIER);
	    BOOST_SPIRIT_DEBUG_RULE(DEVICEMEMORYBARRIERWITHGROUPSYNC);
	    BOOST_SPIRIT_DEBUG_RULE(DST);
	    BOOST_SPIRIT_DEBUG_RULE(EVALUATEATTRIBUTEATCENTROID);
	    BOOST_SPIRIT_DEBUG_RULE(EVALUATEATTRIBUTEATSAMPLE);
	    BOOST_SPIRIT_DEBUG_RULE(EVALUATEATTRIBUTESNAPPED);
	    BOOST_SPIRIT_DEBUG_RULE(F16TOF32);
	    BOOST_SPIRIT_DEBUG_RULE(F32TOF16);
	    BOOST_SPIRIT_DEBUG_RULE(FIRSTBITHIGH);
	    BOOST_SPIRIT_DEBUG_RULE(FIRSTBITLOW);
	    BOOST_SPIRIT_DEBUG_RULE(FMA);
	    BOOST_SPIRIT_DEBUG_RULE(GROUPMEMORYBARRIER);
	    BOOST_SPIRIT_DEBUG_RULE(GROUPMEMORYBARRIERWITHGROUPSYNC);
	    BOOST_SPIRIT_DEBUG_RULE(INTERLOCKEDADD);
	    BOOST_SPIRIT_DEBUG_RULE(INTERLOCKEDAND);
	    BOOST_SPIRIT_DEBUG_RULE(INTERLOCKEDCOMPAREEXCHANGE);
	    BOOST_SPIRIT_DEBUG_RULE(INTERLOCKEDCOMPARESTORE);
	    BOOST_SPIRIT_DEBUG_RULE(INTERLOCKEDEXCHANGE);
	    BOOST_SPIRIT_DEBUG_RULE(INTERLOCKEDMAX);
	    BOOST_SPIRIT_DEBUG_RULE(INTERLOCKEDMIN);
	    BOOST_SPIRIT_DEBUG_RULE(INTERLOCKEDOR);
	    BOOST_SPIRIT_DEBUG_RULE(INTERLOCKEDXOR);
	    BOOST_SPIRIT_DEBUG_RULE(MAD);
	    BOOST_SPIRIT_DEBUG_RULE(MSAD4);
	    BOOST_SPIRIT_DEBUG_RULE(PROCESS2DQUADTESSFACTORSAVG);
	    BOOST_SPIRIT_DEBUG_RULE(PROCESS2DQUADTESSFACTORSMAX);
	    BOOST_SPIRIT_DEBUG_RULE(PROCESS2DQUADTESSFACTORSMIN);
	    BOOST_SPIRIT_DEBUG_RULE(PROCESSISOLINETESSFACTORS);
	    BOOST_SPIRIT_DEBUG_RULE(PROCESSQUADTESSFACTORSAVG);
	    BOOST_SPIRIT_DEBUG_RULE(PROCESSQUADTESSFACTORSMAX);
	    BOOST_SPIRIT_DEBUG_RULE(PROCESSQUADTESSFACTORSMIN);
	    BOOST_SPIRIT_DEBUG_RULE(PROCESSTRITESSFACTORSAVG);
	    BOOST_SPIRIT_DEBUG_RULE(PROCESSTRITESSFACTORSMAX);
	    BOOST_SPIRIT_DEBUG_RULE(PROCESSTRITESSFACTORSMIN);
	    BOOST_SPIRIT_DEBUG_RULE(RCP);
	    BOOST_SPIRIT_DEBUG_RULE(REVERSEBITS);
        // Rules
#if 0
			/*
1>c:\boost-trunk\boost\spirit\home\classic\core\scanner\scanner.hpp(132) :
error C2780: '
void `anonymous-namespace'::processtostring::operator ()(IteratorT,IteratorT) const'
: expects 2 arguments - 1 provided
			*/
            primary_expression
                = IDENTIFIER [actions.identifier]
                | CONSTANT    [actions.tostring]
                | STRING_LITERAL    [actions.tostring]
                | LEFT_PAREN     [actions.tostring]>> expression     [actions.tostring]>> RIGHT_PAREN    [actions.tostring]
                ;
#else
            primary_expression
                = IDENTIFIER
                | CONSTANT
                | STRING_LITERAL
                | LEFT_PAREN     >> expression     >> RIGHT_PAREN
                ;
#endif
/*
			program
                    =  *space_p >>
                       *(   preprocessor    [actions.tostring]
                        |   comment         [actions.tostring]
                        |   keyword         [actions.tostring]
                        |   identifier      [actions.identifier]
                        |   special         [actions.tostring]
                        |   string          [actions.tostring]
                        |   literal         [actions.tostring]
                        |   number          [actions.tostring]
                        |   anychar_p       [actions.unexpected]
                        )
                    ;
*/
        // left recursion --> right recursion
        //    postfix_expression
        //        = primary_expression
        //        | postfix_expression >>
        //            (
        //                LEFT_BRACKET >> expression >> RIGHT_BRACKET
        //            |   LEFT_PAREN >> !argument_expression_list >> RIGHT_PAREN
        //            |   DOT >> IDENTIFIER
        //            |   PTR_OP >> IDENTIFIER
        //            |   INC_OP
        //            |   DEC_OP
        //            )
        //        ;
            postfix_expression
                = primary_expression >> postfix_expression_helper
                ;

            postfix_expression_helper
                =   (
                        LEFT_BRACKET >> expression >> RIGHT_BRACKET
                    |   LEFT_PAREN >> !argument_expression_list >> RIGHT_PAREN
                    |   DOT >> IDENTIFIER
                    |   PTR_OP >> IDENTIFIER
                    |   INC_OP
                    |   DEC_OP
                    ) >>
                    postfix_expression_helper
                | epsilon_p
                ;

            argument_expression_list
                = assignment_expression >> *(COMMA >> assignment_expression)
                ;

            unary_expression
                = postfix_expression
                | INC_OP >> unary_expression
                | DEC_OP >> unary_expression
                | unary_operator >> cast_expression >>
                    (
                        unary_expression
                    |   LEFT_PAREN >> type_name >> RIGHT_PAREN
                    )
                ;

            unary_operator

                = ADDROF
                | STAR
                | PLUS
                | MINUS
                | TILDE
                | BANG
                ;

            cast_expression
                = LEFT_PAREN >> type_name >> RIGHT_PAREN >> cast_expression
                | unary_expression
                ;

        // left recursion --> right recursion
        //    multiplicative_expression
        //        = cast_expression
        //        | multiplicative_expression >>
        //        (
        //                STAR >> cast_expression
        //            |   SLASH >> cast_expression
        //            |   PERCENT >> cast_expression
        //        )
        //        ;
            multiplicative_expression
                = cast_expression >> multiplicative_expression_helper
                ;

            multiplicative_expression_helper

                =   (
                        STAR >> cast_expression
                    |   SLASH >> cast_expression
                    |   PERCENT >> cast_expression
                    ) >>
                    multiplicative_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    additive_expression
        //        = multiplicative_expression
        //        | additive_expression >>
        //        (
        //                PLUS >> multiplicative_expression
        //            |   MINUS >> multiplicative_expression
        //        )
        //        ;
            additive_expression
                = multiplicative_expression >> additive_expression_helper
                ;

            additive_expression_helper
                =   (
                        PLUS >> multiplicative_expression
                    |   MINUS >> multiplicative_expression
                    ) >>
                    additive_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    shift_expression
        //        = additive_expression
        //        | shift_expression >>
        //        (
        //                LEFT_OP >> additive_expression
        //            |   RIGHT_OP >> additive_expression
        //        )
        //        ;
            shift_expression
                = additive_expression >> shift_expression_helper
                ;

            shift_expression_helper
                =   (
                        LEFT_OP >> additive_expression
                    |   RIGHT_OP >> additive_expression
                    ) >>
                    shift_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    relational_expression
        //        = shift_expression
        //        | relational_expression >>
        //        (
        //                LT_OP >> shift_expression
        //            |   GT_OP >> shift_expression
        //            |   LE_OP >> shift_expression
        //            |   GE_OP >> shift_expression
        //        )
        //        ;
            relational_expression
                = shift_expression >> relational_expression_helper
                ;

            relational_expression_helper
                =   (
                        LT_OP >> shift_expression
                    |   GT_OP >> shift_expression
                    |   LE_OP >> shift_expression
                    |   GE_OP >> shift_expression
                    ) >>
                    relational_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    equality_expression
        //        = relational_expression
        //        | equality_expression >>
        //        (
        //                EQ_OP >> relational_expression
        //            |   NE_OP >> relational_expression
        //        )
        //        ;
            equality_expression
                = relational_expression >> equality_expression_helper
                ;

            equality_expression_helper
                =   (
                        EQ_OP >> relational_expression
                    |   NE_OP >> relational_expression
                    ) >>
                    equality_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    and_expression
        //        = equality_expression
        //        | and_expression >> ADDROF >> equality_expression
        //        ;
            and_expression
                = equality_expression >> and_expression_helper
                ;

            and_expression_helper
                = ADDROF >> equality_expression >> and_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    exclusive_or_expression
        //        = and_expression
        //        | exclusive_or_expression >> XOR >> and_expression
        //        ;
            exclusive_or_expression
                = and_expression >> exclusive_or_expression_helper
                ;

            exclusive_or_expression_helper
                = XOR >> and_expression >> exclusive_or_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    inclusive_or_expression
        //        = exclusive_or_expression
        //        | inclusive_or_expression >> OR >> exclusive_or_expression
        //        ;
            inclusive_or_expression
                = exclusive_or_expression >> inclusive_or_expression_helper
                ;

            inclusive_or_expression_helper
                = OR >> exclusive_or_expression >> inclusive_or_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    logical_and_expression
        //        = inclusive_or_expression
        //        | logical_and_expression >> AND_OP >> inclusive_or_expression
        //        ;
            logical_and_expression
                = inclusive_or_expression >> logical_and_expression_helper
                ;

            logical_and_expression_helper
                = AND_OP >> inclusive_or_expression >> logical_and_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    logical_or_expression
        //        = logical_and_expression
        //        | logical_or_expression >> OR_OP >> logical_and_expression
        //        ;
            logical_or_expression
                = logical_and_expression >> logical_or_expression_helper
                ;

            logical_or_expression_helper
                = OR_OP >> logical_and_expression >> logical_or_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    conditional_expression
        //        = logical_or_expression
        //        | logical_or_expression >> QUEST >> expression >> COLON >>
        //        conditional_expression
        //        ;
            conditional_expression
                = logical_or_expression >> conditional_expression_helper
                ;

            conditional_expression_helper
                = QUEST >> expression >> COLON
                    >> conditional_expression >> conditional_expression_helper
                | epsilon_p
                ;

            assignment_expression
                = unary_expression >> assignment_operator >> assignment_expression
                | conditional_expression
                ;

            assignment_operator
                = ASSIGN
                | MUL_ASSIGN
                | DIV_ASSIGN
                | MOD_ASSIGN
                | ADD_ASSIGN
                | SUB_ASSIGN
                | LEFT_ASSIGN
                | RIGHT_ASSIGN
                | AND_ASSIGN
                | XOR_ASSIGN
                | OR_ASSIGN
                ;

        // left recursion --> right recursion
        //    expression
        //        = assignment_expression
        //        | expression >> COMMA >> assignment_expression
        //        ;
            expression
                = assignment_expression >> expression_helper
                ;

            expression_helper
                = COMMA >> assignment_expression >> expression_helper
                | epsilon_p
                ;

            constant_expression
                = conditional_expression
                ;

            declaration
                = declaration_specifiers >> !init_declarator_list >> SEMICOLON
                ;

            declaration_specifiers
                =   (
                        storage_class_specifier
                    |   type_specifier
                    //|   type_qualifier
                    ) >>
                    !declaration_specifiers
                ;

            init_declarator_list
                = init_declarator >> *(COMMA >> init_declarator)
                ;

            init_declarator
                = declarator >> !(ASSIGN >> initializer)
                ;

            storage_class_specifier
                = UNIFORM
                ;


            type_specifier
                = INT
		| BOOL
		| FLOAT4x4
		| FLOAT4
		| FLOAT3
		| FLOAT2
                | FLOAT
                | struct_or_union_specifier
        //        | TYPE_NAME
                ;

		texture_declaration
                = (TEXTURE1D
		| TEXTURE2D
		| TEXTURE3D
		| TEXTURECUBE
		| TEXTURE )
		 >> IDENTIFIER >> SEMICOLON
                ;

		sampler_declaration_list =
	    	  (tEXTURE >> ASSIGN >> LT_OP >> IDENTIFIER >> GT_OP >> SEMICOLON) 
		| (MIPFILTER >> ASSIGN >> (strlit<>("None")|strlit<>("Point")|strlit<>("Linear")|strlit<>("NONE")|strlit<>("POINT")|strlit<>("LINEAR")) >> SEMICOLON)
		| (MINFILTER >> ASSIGN >> (strlit<>("Point")|strlit<>("Linear")|strlit<>("Anisotropic")|strlit<>("POINT")|strlit<>("LINEAR")|strlit<>("ANISOTROPIC")) >> SEMICOLON)
		| (MAGFILTER >> ASSIGN >> (strlit<>("Point")|strlit<>("Linear")|strlit<>("POINT")|strlit<>("LINEAR")) >> SEMICOLON)
		| (strlit<>("AddressU")|strlit<>("AddressV")|strlit<>("ADDRESSU")|strlit<>("ADDRESSV")) >> ASSIGN >> (strlit<>("Wrap")|strlit<>("Clamp")|strlit<>("WRAP")|strlit<>("CLAMP")) >> SEMICOLON
                ;

		sampler_definition
                =(SAMPLERCUBE
		| SAMPLER1D
		| SAMPLER2D
		| SAMPLER3D
		| SAMPLER )
		>> IDENTIFIER
		>> ASSIGN
		>> SAMPLER_STATE
		>> LEFT_BRACE 
		>> sampler_declaration_list 
		>> RIGHT_BRACE >> SEMICOLON
                ;

            struct_or_union_specifier
                = struct_or_union >>
                    (
                        IDENTIFIER 
			   >> *( ASSIGN >> SAMPLER_STATE) 
			   >> !(LEFT_BRACE >> struct_declaration_list
                           >> RIGHT_BRACE)

                    |   LEFT_BRACE >> struct_declaration_list >> RIGHT_BRACE
                    )
                ;

            struct_or_union
                = STRUCT
                ;

            struct_declaration_list
                = +struct_declaration
                ;

            struct_declaration
                = specifier_qualifier_list >> struct_declarator_list >> 
		(SEMICOLON | (COMMA >> IDENTIFIER >> ( COLON >> 
(strlit<>("POSITION")|strlit<>("NORMAL")|strlit<>("COLOR")|strlit<>("TEXCOORD")>>*(digit_p))
)) >> SEMICOLON)
                ;

            specifier_qualifier_list
                =   (
                        type_specifier
                    //|   type_qualifier
                    ) >>
                    !specifier_qualifier_list
                ;

            struct_declarator_list
                = struct_declarator >> *(COMMA >> struct_declarator)
                ;

            struct_declarator
                = declarator || (COLON >> constant_expression)
                ;



            enumerator_list
                = enumerator >> *(COMMA >> enumerator)
                ;

            enumerator
                = IDENTIFIER >> !(ASSIGN >> constant_expression)
                ;
/*
            type_qualifier
                = CONST

                ;*/

            declarator
                = !pointer >> direct_declarator
                ;

        // left recursion --> right recursion
        //    direct_declarator
        //        = IDENTIFIER
        //        | LEFT_PAREN >> declarator >> RIGHT_PAREN
        //        | direct_declarator >>
        //            (
        //                LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
        //            |   LEFT_PAREN >>
        //                (
        //                    parameter_type_list >> RIGHT_PAREN
        //                |   identifier_list >> RIGHT_PAREN
        //                |   RIGHT_PAREN
        //                )
        //            )
        //        ;
            direct_declarator
                =   (
                        IDENTIFIER
                    |   LEFT_PAREN >> declarator >> RIGHT_PAREN
                    ) >>
                    direct_declarator_helper
                ;

            direct_declarator_helper
                =   (
                        LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                    |   LEFT_PAREN >>
                    !   (
                            parameter_type_list
                        |   identifier_list
                        ) >> RIGHT_PAREN
                    ) >> direct_declarator_helper
                | epsilon_p
                ;

            pointer
                = STAR >> !(//type_qualifier_list || 
		pointer)
                ;
/*
            type_qualifier_list
                = +type_qualifier
                ;
*/
            parameter_type_list
                = parameter_list >> !(COMMA >> ELLIPSIS)
                ;

            parameter_list
                = parameter_declaration >> *(COMMA >> parameter_declaration)
                ;

            parameter_declaration
                = declaration_specifiers >>
                   !(
                        declarator
                    |   abstract_declarator
                    )
                ;

            identifier_list
                = IDENTIFIER >> *(COMMA >> IDENTIFIER)
                ;

            type_name
                = specifier_qualifier_list >> !abstract_declarator
                ;

            abstract_declarator
                = pointer || direct_abstract_declarator
                ;

        // left recursion --> right recursion
        //    direct_abstract_declarator
        //        = LEFT_PAREN >>
        //            (
        //                abstract_declarator >> RIGHT_PAREN
        //            |   !parameter_type_list >> RIGHT_PAREN
        //            )
        //        | LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
        //        | direct_abstract_declarator >>
        //            (
        //                LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
        //            |   LEFT_PAREN >> !parameter_type_list >> RIGHT_PAREN
        //            )
        //        ;
            direct_abstract_declarator
                =   (
                        LEFT_PAREN >>
                        (
                            abstract_declarator >> RIGHT_PAREN
                        |   !parameter_type_list >> RIGHT_PAREN
                        )
                    |    LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                    ) >>
                    direct_abstract_declarator_helper
                ;

            direct_abstract_declarator_helper
                =   (
                        LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                    |   LEFT_PAREN >> !parameter_type_list >> RIGHT_PAREN
                    ) >>
                    direct_abstract_declarator_helper
                | epsilon_p
                ;

            initializer
                = assignment_expression
                | LEFT_BRACE >> initializer_list >> !COMMA >> RIGHT_BRACE
                ;

            initializer_list
                = initializer >> *(COMMA >> initializer)
                ;

            statement
                = labeled_statement
                | compound_statement
                | expression_statement
                | selection_statement
                | iteration_statement
                | jump_statement
                ;

            labeled_statement
                = IDENTIFIER >> COLON >> statement
                | /*CASE >> */ constant_expression >> COLON >> statement
                //| DEFAULT >> COLON >> statement
                ;

            compound_statement
                = LEFT_BRACE >> !(declaration_list || statement_list) >> RIGHT_BRACE
                ;

            declaration_list
                = +declaration
                ;

            statement_list
                = +statement
                ;

            expression_statement
                = !expression >> SEMICOLON
                ;

            selection_statement
                = IF >> LEFT_PAREN >> expression >> RIGHT_PAREN >> statement
                   // >> !(ELSE >> statement)
                // | SWITCH >> LEFT_PAREN >> expression >> RIGHT_PAREN >> statement
                ;

            iteration_statement
                = WHILE >> LEFT_PAREN >> expression >> RIGHT_PAREN >>  statement
                | DO >> statement >> WHILE >> LEFT_PAREN >> expression
                    >> RIGHT_PAREN >> SEMICOLON
                | FOR >> LEFT_PAREN >> expression_statement >> expression_statement
                    >> !expression >> RIGHT_PAREN >> statement
                ;

            jump_statement
                = //GOTO >> IDENTIFIER >> SEMICOLON
                //| 
		CONTINUE >> SEMICOLON
                | BREAK >> SEMICOLON
                | RETURN >> !expression >> SEMICOLON
                ;

            function_definition
                =  !declaration_specifiers
                >>  declarator
                >> !declaration_list
                >>  compound_statement
                ;

            external_declaration
                = function_definition
                | declaration
		| texture_declaration
		| sampler_definition
                ;

        // parser start symbol
            translation_unit
                = *external_declaration
                ;
        }

    // keywords
        symbols<> keywords;

    // operators
        strlit<>
                ELLIPSIS, RIGHT_ASSIGN, LEFT_ASSIGN, ADD_ASSIGN, SUB_ASSIGN,
                MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, AND_ASSIGN, XOR_ASSIGN,
                OR_ASSIGN, RIGHT_OP, LEFT_OP, INC_OP, DEC_OP, PTR_OP, AND_OP,
                OR_OP, LE_OP, GE_OP, EQ_OP, NE_OP;
        chlit<>
                SEMICOLON, COMMA, COLON, ASSIGN, LEFT_PAREN, RIGHT_PAREN,
                DOT, ADDROF, BANG, TILDE, MINUS, PLUS, STAR, SLASH, PERCENT,
                LT_OP, GT_OP, XOR, OR, QUEST;

        rule<ScannerT>
                LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET;

    // terminals
        rule<ScannerT>
            	BREAK, CONTINUE, DISCARD, DO, FOR, IF, SWITCH, WHILE, 
		INT, RETURN, STRUCT, 
		UNIFORM, BOOL, FLOAT,
		FLOAT2, FLOAT3, FLOAT4, FLOAT4x4,
		TECHNIQUE, PASS, COMPILE, VERTEXSHADER, PIXELSHADER,
		SAMPLER, SAMPLER1D, SAMPLER2D, SAMPLER3D, SAMPLERCUBE, SAMPLER_STATE,
		tEXTURE, TEXTURE1D, TEXTURE2D, TEXTURE3D, TEXTURECUBE, TEXTURE,
		MIPFILTER, LINEAR, MINFILTER, MAGFILTER,
		VS_2_0, PS_2_0, VS_3_0, PS_3_0,

//1:
ABS, ACOS, ALL, ANY, ASIN, ATAN, ATAN2, CEIL, CLAMP, CLIP, COS, COSH, CROSS, DEGREES, DETERMINANT, D3DCOLORTOUBYTE4, DISTANCE, DOT_, EXP, EXP2, FACEFORWARD, FLOOR, FMOD, FRAC, ISFINITE, ISINF, ISNAN, LDEXP, LENGTH, LERP, LIT, LOG, LOG10, LOG2, MAX, MIN, MODF, MUL, NOISE, NORMALIZE, POW, RADIANS, REFLECT, REFRACT, ROUND, RSQRT, SATURATE, SIGN, SIN, SINCOS, SINH, SMOOTHSTEP, SQRT, STEP, TAN, TANH, TEX1D, TEX2D, TEX3D, TEXCUBE, TRANSPOSE, TRUNC,
//2:
DDX, DDY, FREXP, FWIDTH, TEX1DBIAS, TEX1DGRAD, TEX1DPROJ, TEX2DBIAS, TEX2DGRAD, TEX2DPROJ, TEX3DBIAS, TEX3DGRAD, TEX3DPROJ, TEXCUBEBIAS, TEXCUBEGRAD, TEXCUBEPROJ,
//3:
TEX1DLOD, TEX2DLOD, TEX3DLOD, TEXCUBELOD,
//4:
ABORT, ASFLOAT, ASUINT, ERRORF, GETRENDERTARGETSAMPLECOUNT, GETRENDERTARGETSAMPLEPOSITION, PRINTF,
//5:
ALLMEMORYBARRIER, ALLMEMORYBARRIERWITHGROUPSYNC, ASDOUBLE, CHECKACCESSFULLYMAPPED, COUNTBITS, DDX_COARSE, DDX_FINE, DDY_COARSE, DDY_FINE, DEVICEMEMORYBARRIER, DEVICEMEMORYBARRIERWITHGROUPSYNC, DST, EVALUATEATTRIBUTEATCENTROID, EVALUATEATTRIBUTEATSAMPLE, EVALUATEATTRIBUTESNAPPED, F16TOF32, F32TOF16, FIRSTBITHIGH, FIRSTBITLOW, FMA, GROUPMEMORYBARRIER, GROUPMEMORYBARRIERWITHGROUPSYNC, INTERLOCKEDADD, INTERLOCKEDAND, INTERLOCKEDCOMPAREEXCHANGE, INTERLOCKEDCOMPARESTORE, INTERLOCKEDEXCHANGE, INTERLOCKEDMAX, INTERLOCKEDMIN, INTERLOCKEDOR, INTERLOCKEDXOR, MAD, MSAD4, PROCESS2DQUADTESSFACTORSAVG, PROCESS2DQUADTESSFACTORSMAX, PROCESS2DQUADTESSFACTORSMIN, PROCESSISOLINETESSFACTORS, PROCESSQUADTESSFACTORSAVG, PROCESSQUADTESSFACTORSMAX, PROCESSQUADTESSFACTORSMIN, PROCESSTRITESSFACTORSAVG, PROCESSTRITESSFACTORSMAX, PROCESSTRITESSFACTORSMIN, RCP, REVERSEBITS, 

		IDENTIFIER,
                STRING_LITERAL_PART, STRING_LITERAL, INT_CONSTANT_HEX, INT_CONSTANT,
                INT_CONSTANT_OCT, INT_CONSTANT_DEC, INT_CONSTANT_CHAR,
                FLOAT_CONSTANT,FLOAT_CONSTANT_1, FLOAT_CONSTANT_2, FLOAT_CONSTANT_3,
                CONSTANT;

    // nonterminals
        rule<ScannerT>
                primary_expression, postfix_expression, postfix_expression_helper,
                argument_expression_list, unary_expression, unary_operator,
                cast_expression,
                multiplicative_expression, multiplicative_expression_helper,
                additive_expression, additive_expression_helper,
                shift_expression, shift_expression_helper,
                relational_expression, relational_expression_helper,
                equality_expression, equality_expression_helper,
                and_expression, and_expression_helper,
                exclusive_or_expression, exclusive_or_expression_helper,
                inclusive_or_expression, inclusive_or_expression_helper,
                logical_and_expression, logical_and_expression_helper,
                logical_or_expression, logical_or_expression_helper,
                conditional_expression, conditional_expression_helper,
                assignment_expression, assignment_operator,
                expression, expression_helper, constant_expression, declaration,
                declaration_specifiers, init_declarator_list, init_declarator,
                 type_specifier, struct_or_union_specifier,
                struct_or_union, struct_declaration_list, struct_declaration,
                specifier_qualifier_list, struct_declarator_list,
                struct_declarator, enum_specifier, enumerator_list, enumerator,
                type_qualifier, declarator,
                direct_declarator, direct_declarator_helper, pointer,
		parameter_type_list, parameter_list,
                parameter_declaration, identifier_list, type_name,
                abstract_declarator,
                direct_abstract_declarator, direct_abstract_declarator_helper,
                initializer, initializer_list, statement, labeled_statement,
                compound_statement, declaration_list, statement_list,
                expression_statement, selection_statement, iteration_statement,
                jump_statement, translation_unit, external_declaration,
                function_definition, texture_declaration, sampler_definition,
		sampler_declaration_list,storage_class_specifier;

        rule<ScannerT> const&
        start() const { return translation_unit; }
#endif
        };
        Actions& actions;
    };
}}}} // namespace boost::spirit::repository

using namespace std;
using namespace boost::spirit::classic;
using namespace boost::spirit::classic::repository;

namespace
{
    template <typename CharT>
    void print_char(CharT ch, std::string& out)
    {
        switch (ch)
        {
			/*
            case '<': out << "&lt;";    break;
            case '>': out << "&gt;";    break;
            case '&': out << "&amp;";   break;
            case '"': out << "&quot;";  break;
				*/
			char buf[2];
			buf[0]=ch;
			buf[1]='\0';
            default:  out.append( buf);        break;
        }
    }

    struct process
    {
        process(char const* name, std::string& out)
            : name(name), out(out) {}

        template <typename IteratorT>
        void operator()(IteratorT first, IteratorT last) const
        {
            //out << "<span class=" << name << ">";
			IteratorT first0=first;
#if 0
			while (first0 != last)
                print_char(*first0++, out);
#else
			std::string buf;
			//IteratorT first0=first;
            while (first0 != last)
			{
			char buf2[2];
			buf2[0]=*first0++;
			buf2[1]='\0';
                buf.append(buf2);
			}
            if(strcmp(buf.c_str(),"float2")==0)
				buf="vec2";
            if(strcmp(buf.c_str(),"float3")==0)
				buf="vec3";
            else if(strcmp(buf.c_str(),"float4")==0)
				buf="vec4";
            else if(strcmp(buf.c_str(),"float4x4")==0)
				buf="mat4";


            else if(strcmp(buf.c_str(),"row_major")==0)
				buf="";
            else if(strcmp(buf.c_str(),"matrix")==0)
				buf="mat4";


            else if(strcmp(buf.c_str(),"mod")==0)
				buf="fmod";
            else if(strcmp(buf.c_str(),"inversesqrt")==0)
				buf="rsqrt";
            else if(strcmp(buf.c_str(),"fract")==0)
				buf="frac";
            else if(strcmp(buf.c_str(),"mix")==0)
				buf="lerp";
            else if(strcmp(buf.c_str(),"*")==0)
				buf="mul";
            else if(strcmp(buf.c_str(),"dFdx")==0)
				buf="ddx";
            else if(strcmp(buf.c_str(),"dFdy")==0)
				buf="ddy";

#endif
			//out << "</span>";
			out.append(buf);
        }

        char const* name;
        std::string& out;
    };

    struct processtostring
    {
        processtostring(char const* name, std::string& out)
            : name(name), out(out) {}

        template <typename IteratorT>
        void operator()(IteratorT first, IteratorT last) const
        {
            //out << "<span class=" << name << ">";
			IteratorT first0=first;
#if 0
			while (first0 != last)
                print_char(*first0++, out);
#else
			std::string buf;
			//IteratorT first0=first;
            while (first0 != last)
			{
			char buf2[2];
			buf2[0]=*first0++;
			buf2[1]='\0';
                buf.append(buf2);
			}
#endif
			//out << "</span>";
			out.append(buf);
        }

        char const* name;
        std::string& out;
    };


	struct unexpected_char
    {
        unexpected_char(std::string& out)
        : out(out) {}

        template <typename CharT>
        void operator()(CharT) const
        {
            out.append("#"); // print out an unexpected_char character
        }

        std::string& out;
    };

    struct cpp_to_html_actions
    {
        cpp_to_html_actions(std::string& out)
            : preprocessor("preprocessor", out)
            , comment("comment", out)
            , keyword("keyword", out)
            , identifier("identifier", out)
            , special("special", out)
            , string("string", out)
            , literal("literal", out)
            , number("number", out)
            , unexpected(out)
			, tostring("tostring", out)
        {}

		processtostring tostring;
        process
            preprocessor, comment, keyword, identifier,
            special, string, literal, number;
        unexpected_char unexpected;
    };
}

using namespace boost::spirit::classic::repository;
void
parsefs(char const* filename,std::string& out
	  )
{
	vector<char> vec;
    //vec.resize(strlen(filename)+1);
	unsigned int i=0;
	while(1)
	{
		if(!filename[i])
			break;
		vec.push_back(filename[i++]);
	}
	vec[i]='\0';

    vector<char>::const_iterator start = vec.begin();
    vector<char>::const_iterator end = vec.end();

    skip_vsgrammar skip;

    cpp_to_html_actions actions(out);
	::vs_grammar<cpp_to_html_actions> p(actions);
    parse_info<vector<char>::const_iterator> result =
        parse(start, end, p
		, skip);

    if (result.full)
        LOG_PRINT("\nParses OK\n");
	else 
        LOG_PRINT("\nFails Parsing\n");

		LOG_PRINT("\n\n\n%s\n\n\n",string(start,result.stop).c_str());
		LOG_PRINT("\n\n\n%s\n\n\n",string(result.stop, end).c_str());
    }

void
parsevs(char const* filename,std::string& out
	  )
{
	vector<char> vec;
    //vec.resize(strlen(filename)+1);
	unsigned int i=0;
	while(1)
	{
		if(!filename[i])
			break;
		vec.push_back(filename[i++]);
	}

	vec[i]='\0';

    vector<char>::const_iterator start = vec.begin();
    vector<char>::const_iterator end = vec.end();

    skip_vsgrammar skip;

    cpp_to_html_actions actions(out);
	::vs_grammar<cpp_to_html_actions> p(actions);
    parse_info<vector<char>::const_iterator> result =
        parse(start, end, p
		, skip);

    if (result.full)
        LOG_PRINT("\n%s\n", /*filename ,*/ " Parses OK");
    else {
        LOG_PRINT("\n%s\n", /*filename ,*/ " Fails Parsing");

		LOG_PRINT("\n\n\n%s\n\n\n",string(start,result.stop).c_str());
		LOG_PRINT("\n\n\n%s\n\n\n",string(result.stop, end).c_str());
    }
}
int ApplicationLogic()
{
	std::string vsout, fsout, out3;
    if (2 == argc)
	{
		char buf[256];
		snprintf(buf, 256, "%s.vs", argv[1]);
		std::ifstream t(buf);
		std::string str1((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		parsevs(str1.c_str(), vsout);
		t.close();
		snprintf(buf, 256, "%s.ps", argv[1]);
		std::ifstream t2(buf);
		std::string str2((std::istreambuf_iterator<char>(t2)), std::istreambuf_iterator<char>());
		parsefs(str2.c_str(), fsout);
		t2.close();
#if 0
		const char str3="int main(int argc)"
				"{"
				"	return 0;"
				"}";
		parsevs(str3, out3);
#endif
		LOG_PRINT("vsout:\n%s\n", vsout.c_str());
		LOG_PRINT("fsout:\n%s\n", fsout.c_str());
		LOG_PRINT("out3:\n%s\n", out3.c_str());
	}
    else
        LOG_PRINT("No filename given\n");
    return 0;
}

