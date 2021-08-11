/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __CgReflection__
#define __CgReflection__
#if 1//def _LINUX

#include <stdio.h>
#ifdef QT_BUILD
#include <string>
#else
#include <string.h>
#endif
#include <Framework3/IRenderer.h>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

//extern CGcontext g_CgContext;
//};
struct CgReflection
{
	CgReflection()
	{
		tui=0;
		m_uniformCount=0;
	}
	virtual ~CgReflection(){clear();}
CGbool interestingCgProgramParameter(CGparameter parameter)
{
  return cgIsParameterReferenced(parameter);
}
CGbool interestingCgProgramParameters(CGprogram program, CGenum nameSpace)
{
  CGparameter parameter;
  for (parameter = cgGetFirstParameter(program, nameSpace);
    parameter;
    parameter = cgGetNextParameter(parameter)) {
      if (interestingCgProgramParameter(parameter)) {
        return CG_TRUE;
      }
    }
    return CG_FALSE;
}
bool IsParameterValid(CGparameter parameter)
{
        bool r=true;
  CGerror error;
  const char *string = cgGetLastErrorString(&error);
  if (error == CG_INVALID_PARAMETER_HANDLE_ERROR) r=false;
        return r;
}
void saveCgParameterInfo(CGparameter parameter, int unabridged/*,
                                                                        std::vector<CGparameter>& v,
                                                                        std::vector<std::string>& s,
                                                                        std::vector<int>& i,
  std::vector<int>& tu,
  int& tui*/
                                                                        )
{
  const char *semantic = cgGetParameterSemantic(parameter);
  const char* cVariability=cgGetEnumString(cgGetParameterVariability(parameter));
  const char* cDirection=cgGetEnumString(cgGetParameterDirection(parameter));
  CGtype type=cgGetParameterType(parameter);
  const char* cType=cgGetTypeString(type);

  CGparameterclass paramclass = cgGetParameterClass(parameter);

  if(
(strcmp(cVariability,"CG_UNIFORM")==0)//???
  &&(strcmp(cDirection,"CG_IN")==0)
  )//???
	{std::string x(cgGetParameterName(parameter));
	if(std::find(n.begin(), n.end(), x) == n.end())
  {
	  
	  t.push_back(type);
	  ts.push_back(cType);
	  c.push_back(paramclass);
	  p.push_back(parameter);
 //LOG_PRINT("pn=%s\n",cgGetParameterName(parameter));

		rows.push_back(cgGetParameterRows(parameter));

		columns.push_back(cgGetParameterColumns(parameter));
// //LOG_PRINT("t=%s\n",type);

	  n.push_back(std::string(cgGetParameterName(parameter)));
		

	 
        if((strcmp(cType,"sampler")==0)||
                (strcmp(cType,"sampler1D")==0)||
                (strcmp(cType,"sampler2D")==0)||
                (strcmp(cType,"sampler3D")==0)||
                (strcmp(cType,"samplerCUBE")==0))
        {
                //i.push_back(1);
                tu.push_back(tui++);
        }
                else
                {
                //i.push_back(0);
                tu.push_back(-1);
				m_uniformCount++;
        }
}}
}
void saveCgProgramParameter(CGparameter parameter, CGbool unabridged/*,
                                                                        std::vector<CGparameter>& v,
                                                                        std::vector<std::string>& s,
                                                                        std::vector<int>& i,
                                                                        std::vector<int>& tu,
                                                                        int& tui
                                                                        */){
  if (cgIsParameterReferenced(parameter)&&(IsParameterValid(parameter))) {
    ////LOG_PRINT("          Valid Parameter %s (0x%p):\n",
//      cgGetParameterName(parameter), parameter);
    saveCgParameterInfo(parameter, unabridged);//,v,s,i,tu, tui);
  }
}
void saveCgProgramParameters(CGprogram m_Program,
                                    CGenum nameSpace,
                                    char const *nameSpaceName,
                                    CGbool unabridged/*,
                                                                        std::vector<CGparameter>& v,
                                                                        std::vector<std::string>& s,
                                                                        std::vector<int>& i,
  std::vector<int>& tu,
  int& tui                                             */       
  )
{
        //IMPLEMENT;
        //return;//???ei toimi
  CGparameter parameter;
  ////LOG_PRINT("  %s:\n", nameSpaceName);
  parameter = cgGetFirstParameter(m_Program, nameSpace);
  saveCgProgramParameter(parameter, unabridged);//,v,s,i,tu,tui);
  for (parameter = cgGetFirstParameter(m_Program, nameSpace);
    parameter;
    parameter = cgGetNextParameter(parameter)) {
                saveCgProgramParameter(parameter, unabridged);//,v,s,i,tu,tui);
    }
    CGerror error;
  const char *string = cgGetLastErrorString(&error);
  if (error == CG_INVALID_PARAMETER_HANDLE_ERROR) {LOG_PRINT("CG_INVALID_PARAMETER_HANDLE_ERROR\n");}
  else if (error != CG_NO_ERROR) {LOG_PRINT("CG_ERROR\n");}
  //else //LOG_PRINT("CG_NO_ERROR\n");
}
void saveCgProgram(CGprogram program, CGbool unabridged/*,
                                                                        std::vector<CGparameter>& v,
                                                                        std::vector<std::string>& s,
                                                                        std::vector<int>& i,
  std::vector<int>& tu,
  int& tui                                                      */
  )
{
    ////LOG_PRINT("  Profile: %s\n", cgGetProfileString(cgGetProgramProfile(program)));
    if (unabridged || interestingCgProgramParameters(program, CG_GLOBAL)) {
        saveCgProgramParameters(program, CG_GLOBAL, "Global Parameters", unabridged);//,v,s,i,tu,tui);
    }
    if (unabridged || interestingCgProgramParameters(program, CG_PROGRAM)) {
        saveCgProgramParameters(program, CG_PROGRAM, "Program Parameters", unabridged);//,v,s,i,tu,tui);
    }
}
void saveCgPrograms(CGprogram program,CGcontext context, CGbool unabridged/*,
                                                                        std::vector<CGparameter>& v,
                                                                        std::vector<std::string>& s,
                                                                        std::vector<int>& i,
  std::vector<int>& tu,
  int& tui                                                                      */
  )
{
    if (cgIsProgramCompiled(program)) {
      ////LOG_PRINT("Program 0x%p (%d):\n", program, 0);
      saveCgProgram(program, unabridged);//,v,s,i,tu,tui);
    } else {
      if (unabridged) {
        ////LOG_PRINT("__uncompiled Program 0x%p (%d):\n", program, 0);
        saveCgProgram(program, unabridged);//,v,s,i,tu,tui);
      }
    }
}
void saveCgProgramParameters(CGprogram program)
{
        CGbool unabridged=true;//???

        CGcontext CgContext=RendererCgGL_1_1::GetCgContext();

        saveCgPrograms(program, CgContext,
                unabridged);/*,
                FProgramParameters,FProgramParameterNames,FProgramParameterTypes,FProgramParameterTU,ftui);*/
}
void parse(CGprogram program, GLint& uniformCount)
{
        tui=0;
		m_uniformCount=0;
	//void saveCgVProgramParameters();
		saveCgProgramParameters(program);
		int tmp=m_uniformCount;
		m_uniformCount=0;
		tui=0;
		saveCgProgramParameters(program);
		uniformCount=tmp+m_uniformCount;
}
int tui;
int m_uniformCount;
std::vector<CGparameter> p;
std::vector<std::string> n;
std::vector<std::string> ts;
std::vector<CGtype> t;
std::vector<CGparameterclass> c;
std::vector<int> tu;
std::vector<int> rows;
std::vector<int> columns;
void clear()
{
	p.clear();
	p.empty();
	n.clear();
	n.empty();
	ts.clear();
	ts.empty();
	t.clear();
	t.empty();
	c.clear();
	c.empty();
	tu.clear();
	tu.empty();
	rows.clear();
	rows.empty();
	columns.clear();
	columns.empty();
}
};


/* cgfx_dump.c */


struct CgDump
{
void dumpCgStateAssignment(CGstateassignment sa, CGbool unabridged)
{
  CGstate state = cgGetStateAssignmentState(sa);
  CGtype type = cgGetStateType(state);
  int numDependents = cgGetNumDependentStateAssignmentParameters(sa);
  CGprogram program;
  const float *fvalues;
  const int *ivalues;
  const CGbool *bvalues;
  int numValues;
  int i;

  LOG_PRINT("        StateAssignment 0x%p:\n", sa);
  LOG_PRINT("          State Name: %s\n",
    cgGetStateName(state));
  LOG_PRINT("          State Type: %s\n",
    cgGetTypeString(type));
  switch (type) {
  case CG_PROGRAM_TYPE:
    program = cgGetProgramStateAssignmentValue(sa);
    LOG_PRINT("          State Assignment Value: 0x%p\n", program);
    break;
  case CG_FLOAT:
  case CG_FLOAT1:
  case CG_FLOAT2:
  case CG_FLOAT3:
  case CG_FLOAT4:
    fvalues = cgGetFloatStateAssignmentValues(sa, &numValues);
    LOG_PRINT("          State Assignment Value:");
    for (i=0; i<numValues; i++) {
      LOG_PRINT(" %f", fvalues[i]);
    }
    LOG_PRINT("\n");
    break;
  case CG_INT:
  case CG_INT1:
  case CG_INT2:
  case CG_INT3:
  case CG_INT4:
    ivalues = cgGetIntStateAssignmentValues(sa, &numValues);
    LOG_PRINT("          State Assignment Value:");
    for (i=0; i<numValues; i++) {
      LOG_PRINT(" %d (0x%x)", ivalues[i], ivalues[i]);
    }
    LOG_PRINT("\n");
    break;
  case CG_BOOL:
  case CG_BOOL1:
  case CG_BOOL2:
  case CG_BOOL3:
  case CG_BOOL4:
    bvalues = cgGetBoolStateAssignmentValues(sa, &numValues);
    LOG_PRINT("          State Assignment Value:");
    for (i=0; i<numValues; i++) {
      LOG_PRINT(" %s", bvalues[i] ? "TRUE" : "false");
    }
    LOG_PRINT("\n");
    break;
  case CG_STRING:
    LOG_PRINT("          State Assignment Value: %s\n", cgGetStringStateAssignmentValue(sa));
    break;
  case CG_TEXTURE:
    LOG_PRINT("          State Assignment Value: 0x%p\n", cgGetTextureStateAssignmentValue(sa));
    break;
  case CG_SAMPLER1D:
  case CG_SAMPLER2D:
  case CG_SAMPLER3D:
  case CG_SAMPLERCUBE:
  case CG_SAMPLERRECT:
    LOG_PRINT("          State Assignment Value: 0x%p\n", cgGetSamplerStateAssignmentValue(sa));
    break;
  default:
    LOG_PRINT("UNEXPECTED CASE: 0x%x (%d)\n", type, type);
    break;
  }
  for (i=0; i<numDependents; i++) {
    CGparameter parameter = cgGetDependentStateAssignmentParameter(sa, i);

    LOG_PRINT("            Dependent on Parameter %s (0x%p)\n",
      cgGetParameterName(parameter), parameter);
  }
}

void dumpCgSamplerStateAssignment(CGstateassignment sa, CGbool unabridged)
{
  CGstate state = cgGetSamplerStateAssignmentState(sa);
  CGtype type = cgGetStateType(state);
  int numDependents = cgGetNumDependentStateAssignmentParameters(sa);
  CGprogram program;
  const float *fvalues;
  const int *ivalues;
  const CGbool *bvalues;
  int numValues;
  int i;

  LOG_PRINT("            SamplerStateAssignment 0x%p:\n", sa);
  LOG_PRINT("              State Name: %s\n",
    cgGetStateName(state));
  LOG_PRINT("              State Type: %s\n",
    cgGetTypeString(type));
  switch (type) {
  case CG_PROGRAM_TYPE:
    program = cgGetProgramStateAssignmentValue(sa);
    LOG_PRINT("              State Assignment Value: 0x%p\n", program);
    break;
  case CG_FLOAT:
  case CG_FLOAT1:
  case CG_FLOAT2:
  case CG_FLOAT3:
  case CG_FLOAT4:
    fvalues = cgGetFloatStateAssignmentValues(sa, &numValues);
    LOG_PRINT("              State Assignment Value:");
    for (i=0; i<numValues; i++) {
      LOG_PRINT(" %f", fvalues[i]);
    }
    LOG_PRINT("\n");
    break;
  case CG_INT:
  case CG_INT1:
  case CG_INT2:
  case CG_INT3:
  case CG_INT4:
    ivalues = cgGetIntStateAssignmentValues(sa, &numValues);
    LOG_PRINT("              State Assignment Value:");
    for (i=0; i<numValues; i++) {
      LOG_PRINT(" %d (0x%x)", ivalues[i], ivalues[i]);
    }
    LOG_PRINT("\n");
    break;
  case CG_BOOL:
  case CG_BOOL1:
  case CG_BOOL2:
  case CG_BOOL3:
  case CG_BOOL4:
    bvalues = cgGetBoolStateAssignmentValues(sa, &numValues);
    LOG_PRINT("              State Assignment Value:");
    for (i=0; i<numValues; i++) {
      LOG_PRINT(" %s", bvalues[i] ? "TRUE" : "false");
    }
    LOG_PRINT("\n");
    break;
  case CG_STRING:
    LOG_PRINT("              State Assignment Value: %s\n", cgGetStringStateAssignmentValue(sa));
    break;
  case CG_TEXTURE:
    LOG_PRINT("              State Assignment Value: 0x%p\n", cgGetTextureStateAssignmentValue(sa));
    break;
  case CG_SAMPLER1D:
  case CG_SAMPLER2D:
  case CG_SAMPLER3D:
  case CG_SAMPLERCUBE:
  case CG_SAMPLERRECT:
    LOG_PRINT("              State Assignment Value: 0x%p\n", cgGetSamplerStateAssignmentValue(sa));
    break;
  default:
    LOG_PRINT("UNEXPECTED CASE: 0x%x (%d)\n", type, type);
    break;
  }
  for (i=0; i<numDependents; i++) {
    CGparameter parameter = cgGetDependentStateAssignmentParameter(sa, i);

    LOG_PRINT("                Dependent on Parameter %s (0x%p)\n",
      cgGetParameterName(parameter), parameter);
  }
}

const char *parameterClassString(CGparameterclass pc)
{
  switch (pc) {
  case CG_PARAMETERCLASS_UNKNOWN:
    return "unknown";
  case CG_PARAMETERCLASS_SCALAR:
    return "scalar";
  case CG_PARAMETERCLASS_VECTOR:
    return "vector";
  case CG_PARAMETERCLASS_MATRIX:
    return "matrix";
  case CG_PARAMETERCLASS_STRUCT:
    return "struct";
  case CG_PARAMETERCLASS_ARRAY:
    return "array";
  case CG_PARAMETERCLASS_OBJECT:
    return "object";
  default:
    return "invalid";
  }
}

void dumpCgParameterInfo(CGparameter parameter, int unabridged)
{
  const char *semantic = cgGetParameterSemantic(parameter);
  CGparameterclass paramclass = cgGetParameterClass(parameter);

  LOG_PRINT("              Class: %s\n", parameterClassString(paramclass));
  LOG_PRINT("              Type: %s\n", cgGetTypeString(cgGetParameterType(parameter)));
  LOG_PRINT("              Variability: %s\n", cgGetEnumString(cgGetParameterVariability(parameter)));
  if (semantic && strlen(semantic)) {
    LOG_PRINT("              Semantic: %s\n", semantic);
  }
  LOG_PRINT("              Resource: %s:%ld (base %s)\n",
    cgGetResourceString(cgGetParameterResource(parameter)),
    cgGetParameterResourceIndex(parameter),
    cgGetResourceString(cgGetParameterBaseResource(parameter)));
  LOG_PRINT("              Direction: %s\n", cgGetEnumString(cgGetParameterDirection(parameter)));
}

CGbool interestingCgProgramParameter(CGparameter parameter)
{
  return cgIsParameterReferenced(parameter);
}

void dumpCgProgramParameter(CGparameter parameter, CGbool unabridged)
{
  if (cgIsParameterReferenced(parameter)) {
    LOG_PRINT("            Parameter %s (0x%p):\n",
      cgGetParameterName(parameter), parameter);
    dumpCgParameterInfo(parameter, unabridged);
  } else {
    if (unabridged) {
      LOG_PRINT("            __unreferenced Parameter %s (0x%p):\n",
        cgGetParameterName(parameter), parameter);
      dumpCgParameterInfo(parameter, unabridged);
    }
  }
}

CGbool interestingCgConnectedParameter(CGparameter parameter)
{
  if (cgIsParameterReferenced(parameter)) {
    CGprogram program = cgGetParameterProgram(parameter);
    CGbool compiled = cgIsProgramCompiled(program);

    return compiled;
  }
  return CG_FALSE;
}

void dumpCgConnectedParameter(CGparameter parameter, CGbool unabridged)
{
  CGprogram program = cgGetParameterProgram(parameter);

  if (cgIsParameterReferenced(parameter)) {
    CGbool compiled = cgIsProgramCompiled(program);
    if (unabridged || compiled) {
      LOG_PRINT("            Connected to Parameter %s (0x%p) of\n",
        cgGetParameterName(parameter), parameter);
      if (compiled) {
        LOG_PRINT("              Program 0x%p\n",
          program);
      } else {
        LOG_PRINT("              __uncompiled Program 0x%p\n",
          program);
      }
    }
  } else {
    if (unabridged) {
      LOG_PRINT("            __unreferenced Connected to Parameter %s (0x%p) of\n",
        cgGetParameterName(parameter), parameter);
      if (cgIsProgramCompiled(program)) {
        LOG_PRINT("              __uncompiled Program 0x%p\n",
          program);
      } else {
        LOG_PRINT("              Program 0x%p\n",
          program);
      }
    }
  }
}

CGbool interestingCgEffectParameter(CGparameter parameter)
{
  int numConnected = cgGetNumConnectedToParameters(parameter);
  int i;

  for (i=0; i<numConnected; i++) {
    CGbool interesting = interestingCgConnectedParameter(cgGetConnectedToParameter(parameter, i));

    if (interesting) {
      return CG_TRUE;
    }
  }
  return CG_FALSE;
}

void dumpCgEffectParameterInfo(CGparameter parameter, int numConnected, CGbool unabridged)
{
  const char *semantic = cgGetParameterSemantic(parameter);
  int i;

  if (semantic && strlen(semantic)) {
    LOG_PRINT("            Semantic: %s\n", semantic);
  }
  if (cgGetParameterClass(parameter) == CG_PARAMETERCLASS_SAMPLER) {
    CGstateassignment sa;

    for (sa = cgGetFirstSamplerStateAssignment(parameter); sa; sa = cgGetNextStateAssignment(sa)) {
      dumpCgSamplerStateAssignment(sa, unabridged);
    }
  }
  for (i=0; i<numConnected; i++) {
    dumpCgConnectedParameter(cgGetConnectedToParameter(parameter, i), unabridged);
  }
}

void dumpCgEffectParameter(CGparameter parameter, CGbool unabridged)
{
  if (unabridged || interestingCgEffectParameter(parameter)) {
    int numConnected = cgGetNumConnectedToParameters(parameter);

    if (numConnected) {
      const char *name = cgGetParameterName(parameter);


      LOG_PRINT("          Parameter %s (0x%p) for effect:\n", name, parameter);
      dumpCgEffectParameterInfo(parameter, numConnected, unabridged);
    } else {
      if (unabridged) {
        const char *name = cgGetParameterName(parameter);

        LOG_PRINT("          __unconnected Parameter %s (0x%p) for effect:\n",
          name, parameter);
        dumpCgEffectParameterInfo(parameter, 0, unabridged);
      }
    }
  }
}

void dumpCgEffectParameters(CGeffect effect, CGbool unabridged)
{
  CGparameter parameter;

  for (parameter = cgGetFirstEffectParameter(effect);
    parameter;
    parameter = cgGetNextParameter(parameter)) {

      dumpCgEffectParameter(parameter, unabridged);
    }
}

CGbool interestingCgProgramParameters(CGprogram program, CGenum nameSpace)
{
  CGparameter parameter;

  for (parameter = cgGetFirstParameter(program, nameSpace);
    parameter;
    parameter = cgGetNextParameter(parameter)) {
      if (interestingCgProgramParameter(parameter)) {
        return CG_TRUE;
      }
    }
    return CG_FALSE;
}

void dumpCgProgramParameters(CGprogram program, 
                                    CGenum nameSpace, 
                                    char const *nameSpaceName, 
                                    CGbool unabridged)
{
  CGparameter parameter;

  LOG_PRINT("  %s:\n", nameSpaceName);
  for (parameter = cgGetFirstParameter(program, nameSpace);
    parameter;
    parameter = cgGetNextParameter(parameter)) {
      dumpCgProgramParameter(parameter, unabridged);
    }
}

void dumpCgProgram(CGprogram program, CGbool unabridged)
{
    LOG_PRINT("  Profile: %s\n", cgGetProfileString(cgGetProgramProfile(program)));
    if (unabridged || interestingCgProgramParameters(program, CG_GLOBAL)) {
        dumpCgProgramParameters(program, CG_GLOBAL, "Global Parameters", unabridged);
    }
    if (unabridged || interestingCgProgramParameters(program, CG_PROGRAM)) {
        dumpCgProgramParameters(program, CG_PROGRAM, "Program Parameters", unabridged);
    }
}

void dumpCgPrograms(CGcontext context, CGbool unabridged)
{
  CGprogram program;
  int programNumber = 0;

  for (program = cgGetFirstProgram(context);
       program;
       program = cgGetNextProgram(program), programNumber++) {
    if (cgIsProgramCompiled(program)) {
      LOG_PRINT("Program 0x%p (%d):\n", program, programNumber);
      dumpCgProgram(program, unabridged);
    } else {
      if (unabridged) {
        LOG_PRINT("__uncompiled Program 0x%p (%d):\n", program, programNumber);
        dumpCgProgram(program, unabridged);
      }
    }
  }
}

void dumpCgStateAssignments(CGpass pass, CGbool unabridged)
{
  CGstateassignment sa;

  for (sa = cgGetFirstStateAssignment(pass);
       sa;
       sa = cgGetNextStateAssignment(sa)) {

      dumpCgStateAssignment(sa, unabridged);
    }
}

void dumpCgPasses(CGtechnique technique, CGbool unabridged)
{
  CGpass pass;
  int passNumber = 0;

  for (pass = cgGetFirstPass(technique);
    pass;
    pass = cgGetNextPass(pass), passNumber++) {
    LOG_PRINT("      Pass %s (0x%p, %d):\n",
      cgGetPassName(pass), pass, passNumber);
    dumpCgStateAssignments(pass, unabridged);
  }
}

void dumpCgTechniques(CGeffect effect, CGbool unabridged)
{
  CGtechnique technique;
  int techniqueNumber = 0;

  for (technique = cgGetFirstTechnique(effect);
       technique;
       technique = cgGetNextTechnique(technique), techniqueNumber++) {
    if (cgIsTechniqueValidated(technique)) {
      LOG_PRINT("    Technique %s (0x%p, %d):\n", cgGetTechniqueName(technique), technique, techniqueNumber);
      dumpCgPasses(technique, unabridged);
    } else {
      if (unabridged) {
        LOG_PRINT("    __unvalidated Technique %s (%d):\n", cgGetTechniqueName(technique), techniqueNumber);
        dumpCgPasses(technique, unabridged);
      }
    }
  }
}

void dumpCgEffects(CGcontext context, CGbool unabridged)
{
  CGeffect effect;
  int effectNumber = 0;

  for (effect = cgGetFirstEffect(context);
       effect;
       effect = cgGetNextEffect(effect), effectNumber++) {
    LOG_PRINT("Effect 0x%p (%d):\n", effect, effectNumber);
    LOG_PRINT("  Effect Parameters:\n");
    dumpCgEffectParameters(effect, unabridged);
    LOG_PRINT("  Techniques:\n");
    dumpCgTechniques(effect, unabridged);
  }
}

void dumpCgContext(CGcontext context, CGbool unabridged)
{
  dumpCgPrograms(context, unabridged);
  dumpCgEffects(context, unabridged);
}
};
#endif
#endif

