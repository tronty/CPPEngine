#!/usr/bin/perl
#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
package a_01_Installing;

sub init
{
	my $width = shift;
	my $height = shift;
	IndieLib->CIndieLib->Instance();
	IndieLib->CIndieLib->Instance()->Init("a_01_Installing");
}

sub render
{
	IndieLib->CIndieLib->Instance()->Input->Update ();
	IndieLib->CIndieLib->Instance()->Render->ClearViewPort (60, 60, 60);
	IndieLib->CIndieLib->Instance()->Render->BeginScene ();
	IndieLib->CIndieLib->Instance()->Render->EndScene ();
	#IndieLib->CIndieLib->Instance()->Render->ShowFpsInWindowTitle();
	return 0;
}

sub run
{
	init(0,0);
	while(1)
	{
		if(IndieLib->CIndieLib->Instance()->Input->IsKeyUp(IndieLib->STX_KEY_ESCAPE))
			return;
		self.render();
	}
}

