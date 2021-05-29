rem
rem Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
rem Licensed under The GNU Lesser General Public License, version 2.1:
rem     http://opensource.org/licenses/LGPL-2.1
rem
rem	cs_4_0 cs_4_1 cs_5_0 ds_5_0 fx_2_0 fx_4_0 fx_4_1 fx_5_0 gs_4_0 
rem      gs_4_1 gs_5_0 hs_5_0 ps_2_0 ps_2_a ps_2_b ps_2_sw ps_3_0 ps_3_sw ps_4_0 
rem      ps_4_0_level_9_1 ps_4_0_level_9_3 ps_4_0_level_9_0 ps_4_1 ps_5_0 tx_1_0 
rem      vs_1_1 vs_2_0 vs_2_a vs_2_sw vs_3_0 vs_3_sw vs_4_0 vs_4_0_level_9_1 
rem      vs_4_0_level_9_3 vs_4_0_level_9_0 vs_4_1 vs_5_0

cd fxc
fxc_x86.exe /E RenderSceneVS /T vs_3_0 /Fo BasicHLSL9.vs.o /Fc BasicHLSL9.vs.asm BasicHLSL9.fx
fxc_x86.exe /E RenderScenePS /T ps_3_0 /Fo BasicHLSL9.ps.o /Fc BasicHLSL9.ps.asm BasicHLSL9.fx
fxc_x86.exe /E RenderSceneVS /T vs_3_0 /Fo BasicHLSL10.vs.o /Fc BasicHLSL10.vs.asm BasicHLSL10.fx
fxc_x86.exe /E RenderScenePS /T ps_3_0 /Fo BasicHLSL10.ps.o /Fc BasicHLSL10.ps.asm BasicHLSL10.fx
fxc_x86.exe /E RenderSceneVS /T vs_3_0 /Fo BasicHLSL11.vs.o /Fc BasicHLSL11.vs.asm BasicHLSL11.fx
fxc_x86.exe /E RenderScenePS /T ps_3_0 /Fo BasicHLSL11.ps.o /Fc BasicHLSL11.ps.asm BasicHLSL11.fx
cd ..

