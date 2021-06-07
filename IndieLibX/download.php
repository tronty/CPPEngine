<html>
 <head>
  <title>IndieLibX downloads</title>
 </head>
 <body>
	<h1>IndieLibX downloads</h1>
 <table>


    <TR><TD>File:</TD><TD>Size in bytes:</TD><TD>Last Updated:</TD><TD>Comments:</TD></TR>


<?php

$filename = './README.html';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD><P></P></TD><TD><P></P></TD></TR>";
}

$filename = './install_.sh';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD><P></P></TD><TD><P></P></TD></TR>";
}

$filename = './IndieLibX.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD><P>3D models loaded with assimp-5.0.1 static library. Build depends on SDL2-2.0.14.</P></TD><TD><P></P></TD></TR>";
}

$filename = './IndieLib_resources.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD><P></P></TD></TR>";
}

$filename = './IndieLibX_AllLibs_LNX32.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD>Compiles. Rendering (GLSL1_1: OpenGL 3.1 / GLSL 1.40) fails. Some textures colored incorrectly as white or black.</TD><TD></TD></TR>";
}

$filename = './IndieLibX_AllLibs_LNX64.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD>Compiles. Rendering (GLSL1_1: OpenGL 4.30 / GLSL 4.3) succeeds.</TD><TD></TD></TR>";
}

$filename = './IndieLibX_AllLibs_NDK.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD>Compiles. Rendering (GLES2) not tested.</TD><TD></TD></TR>";
}

$filename = './IndieLibX_AllLibs_OSX.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD>Compiles. Rendering (GLSL1_1: OpenGL 2.1 / GLSL 1.2) succeeds.</TD><TD></TD></TR>";
}

$filename = './IndieLibX_AllLibs_iOS.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD>Compiles. Rendering (GLES2) not tested.</TD><TD></TD></TR>";
}

$filename = './__IndieLibX_AllLibs_WIN64__.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD>Compiles. Rendering (OpenGL 4.6.0 / GLSL 4.60) partially succeeds.</TD><TD></TD></TR>";
}

$filename = './IndieLibX_AllLibs_WIN64.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD>Compiles. Rendering (D3D11: DirectX11) fails.</TD><TD></TD></TR>";
}

$filename = './HLSL330toGLSL4.py';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}

$filename = './native-activity.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}

$filename = './Lesson01.zip';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}

$filename = './conv3ds.txt';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}

$filename = './conv3ds.exe';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}

$filename = './screenshots/README.html';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}

$filename = './screenshots/NOTES1.html';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}
$filename = './screenshots/NOTES2.html';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}
$filename = './screenshots/NOTES3.html';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}
$filename = './screenshots/NOTES4.html';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}

$filename = './NOTES.txt';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}
$filename = './Projects.xml';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}

$filename = './download.sh';
if (file_exists($filename)) {
    echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/".$filename."'>".$filename."</A></TD><TD>".filesize($filename)."</TD><TD>".date("F d Y H:i:s", filemtime($filename))."</TD><TD></TD><TD></TD></TR>";
}

echo "</table>";
echo "<table>";

echo "<TR><TD><A TARGET='_blank' HREF='Nonaxisymmetric_Magnetic_Structures_in_RS_Canum_Venaticorum--type_Stars.pdf'>Nonaxisymmetric_Magnetic_Structures_in_RS_Canum_Venaticorum--type_Stars.pdf</A></TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/Procedural World: Procedural Architecture.html'>Procedural World: Procedural Architecture.html</A></TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/Procedural modeling of buildings (Pavel Reichl).pdf'>Procedural modeling of buildings (Pavel Reichl).pdf</A></TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/2006.SG.Mueller.ProceduralModelingOfBuildings.final.pdf'>2006.SG.Mueller.ProceduralModelingOfBuildings.final.pdf</TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/Lars Rinde o Alexander Dahl-Procedural Generation of Indoor Environments.pdf'>Lars Rinde o Alexander Dahl-Procedural Generation of Indoor Environments.pdf</A></TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/Automatically Generating Roof Models from Building Footprints.pdf'>Automatically Generating Roof Models from Building Footprints.pdf</TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/Automatic Generation of 3D Building Models with Multiple Roofs.pdf'>Automatic Generation of 3D Building Models with Multiple Roofs.pdf</TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/SevenWaysToUseTurtle-PyCon2009.pdf'>SevenWaysToUseTurtle-PyCon2009.pdf</TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/abop.pdf'>The Algorithmic Beauty of Plants.pdf</TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/CONSTRUCTING TESSELLATIONS AND CREATING HYPERBOLIC ART Raymond F. Tennant.pdf'>CONSTRUCTING TESSELLATIONS AND CREATING HYPERBOLIC ART Raymond F. Tennant.pdf</TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/CreatingRegularRepeatingHyperbolicPatterns.pdf'>CreatingRegularRepeatingHyperbolicPatterns.pdf</TD></TR>";

echo "<TR><TD><A TARGET='_blank' HREF='http://tommironty.fi/Procedural modeling of buildings/dunisam07_AnAlgorithmtoGenerateRepeatingHyperbolicPatterns.pdf'>dunisam07_AnAlgorithmtoGenerateRepeatingHyperbolicPatterns.pdf</TD></TR>";

?>

 </table>
 </body>
</html>
