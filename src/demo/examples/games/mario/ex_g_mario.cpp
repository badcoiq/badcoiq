/*
BSD 2-Clause License

Copyright (c) 2024, badcoiq
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "../../../DemoApp.h"


ExampleGameMario::ExampleGameMario(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleGameMario::~ExampleGameMario()
{
}


bool ExampleGameMario::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(3.f, 3.f, 3.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(36.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::PerspectiveLookAt);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->m_projectionMatrix * m_camera->m_viewMatrix;

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	// генерация всей графики
	// Думаю всё можно уместить в одну текстуру
	bqImage img;
	img.Create(320, 320);
	img.Fill(bqImageFillType::Solid, bq::ColorTransparent, bq::ColorTransparent);
	
	_initColorMap();

	// ground block
	{
		uint8_t pic[] =
		{
			2, 3, 3, 3, 3, 3, 3, 3, 3, 1, 2, 3, 3, 3, 3, 2,
			3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 1,
			3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 1,
			3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 1,
			3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 2, 2, 2, 1,
			3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 2,
			3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 3, 3, 3, 3, 1,
			3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 1,
			3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 1,
			3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 1,
			1, 1, 2, 2, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 2, 1,
			3, 3, 1, 1, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 2, 1,
			3, 2, 3, 3, 1, 1, 1, 1, 3, 2, 2, 2, 2, 2, 2, 1,
			3, 2, 2, 2, 3, 3, 3, 1, 3, 2, 2, 2, 2, 2, 2, 1,
			3, 2, 2, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 2, 1, 1,
			2, 1, 1, 1, 1, 1, 1, 2, 3, 1, 1, 1, 1, 1, 1, 2,
		};
		img.Fill(m_palette, pic, 16, 16, 0, 0);
	}
	
	
	bqStringA stra;
	stra.reserve(256 * 256);

	// empty box
	{
		//uint8_t pic[] =
		//{
			//4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1,
			//1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
			//4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,
		//};
		//img.Fill(palette, pic, 16, 16, 16, 0);
		stra.clear();
		stra += " `````````````` ";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%`%%%%%%%%%%`%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += "`%`%%%%%%%%%%`%`";
		stra += "`%%%%%%%%%%%%%%`";
		stra += " `````````````` ";
		_imageFill(&img, &stra, 16, 16, 16, 0);
	}

	// box[0]
	{
		stra.clear();
		stra += " %%%%%%%%%%%%%% ";
		stra += "%@@@@@@@@@@@@@@`";
		stra += "%@`@@@@@@@@@@`@`";
		stra += "%@@@@%%%%%@@@@@`";
		stra += "%@@@%%```%%@@@@`";
		stra += "%@@@%%`@@%%`@@@`";
		stra += "%@@@@``@@%%`@@@`";
		stra += "%@@@@@@@%%%`@@@`";
		stra += "%@@@@@@%%```@@@`";
		stra += "%@@@@@@%%`@@@@@`";
		stra += "%@@@@@@@``@@@@@`";
		stra += "%@@@@@@%%@@@@@@`";
		stra += "%@@@@@@%%`@@@@@`";
		stra += "%@`@@@@@``@@@`@`";
		stra += "%@@@@@@@@@@@@@@`";
		stra += "````````````````";
		_imageFill(&img, &stra, 16, 16, 32, 0);
	}
	// box[1]
	{
		stra.clear();
		stra += " %%%%%%%%%%%%%% ";
		stra += "%%%%%%%%%%%%%%%`";
		stra += "%%`%%%%%%%%%%`%`";
		stra += "%%%%%%%%%%%%%%%`";
		stra += "%%%%%%```%%%%%%`";
		stra += "%%%%%%`%%%%`%%%`";
		stra += "%%%%%%`%%%%`%%%`";
		stra += "%%%%%``%%%%`%%%`";
		stra += "%%%%%%%%%```%%%`";
		stra += "%%%%%%%%%`%%%%%`";
		stra += "%%%%%%%%``%%%%%`";
		stra += "%%%%%%%%%%%%%%%`";
		stra += "%%%%%%%%%`%%%%%`";
		stra += "%%`%%%%%``%%%`%`";
		stra += "%%%%%%%%%%%%%%%`";
		stra += "````````````````";
		_imageFill(&img, &stra, 16, 16, 48, 0);
	}
	// box[2]
	{
		stra.clear();
		stra += " %%%%%%%%%%%%%% ";
		stra += "%**************`";
		stra += "%*`**********`*`";
		stra += "%****%%%%%*****`";
		stra += "%***%%```%%****`";
		stra += "%***%%`**%%`***`";
		stra += "%***%%`**%%`***`";
		stra += "%****``*%%%`***`";
		stra += "%******%%```***`";
		stra += "%******%%`*****`";
		stra += "%*******``*****`";
		stra += "%******%%******`";
		stra += "%******%%`*****`";
		stra += "%*`*****``***`*`";
		stra += "%**************`";
		stra += "````````````````";
		_imageFill(&img, &stra, 16, 16, 64, 0);
	}
	// bricks
	{
		stra.clear();
		stra += "^^^^^^^^^^^^^^^^";
		stra += "%%%%%%%`%%%%%%%`";
		stra += "%%%%%%%`%%%%%%%`";
		stra += "````````````````";
		stra += "%%%`%%%%%%%`%%%%";
		stra += "%%%`%%%%%%%`%%%%";
		stra += "%%%`%%%%%%%`%%%%";
		stra += "````````````````";
		stra += "%%%%%%%`%%%%%%%`";
		stra += "%%%%%%%`%%%%%%%`";
		stra += "%%%%%%%`%%%%%%%`";
		stra += "````````````````";
		stra += "%%%`%%%%%%%`%%%%";
		stra += "%%%`%%%%%%%`%%%%";
		stra += "%%%`%%%%%%%`%%%%";
		stra += "````````````````";
		_imageFill(&img, &stra, 16, 16, 80, 0);
	}

	// metal?
	{
		stra.clear();
		stra += "%^^^^^^^^^^^^^^`";
		stra += "^%^^^^^^^^^^^^``";
		stra += "^^%^^^^^^^^^^```";
		stra += "^^^%^^^^^^^^````";
		stra += "^^^^%%%%%%%%````";
		stra += "^^^^%%%%%%%%````";
		stra += "^^^^%%%%%%%%````";
		stra += "^^^^%%%%%%%%````";
		stra += "^^^^%%%%%%%%````";
		stra += "^^^^%%%%%%%%````";
		stra += "^^^^%%%%%%%%````";
		stra += "^^^^%%%%%%%%````";
		stra += "^^^`````````%```";
		stra += "^^```````````%``";
		stra += "^`````````````%`";
		stra += "```````````````%";
		_imageFill(&img, &stra, 16, 16, 96, 0);
	}

	// pipe up
	{
		stra.clear();
		stra += "````````````````````````````````";
		stra += "`XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX`";
		stra += "`+++++XXXXXX+++++++++++++++++++`";
		stra += "`XXX++XXXXXX+XX++++++++++X+X+XX`";
		stra += "`XXX++XXXXXX+XX+++++++++++X+XXX`";
		stra += "`XXX++XXXXXX+XX++++++++++X+X+XX`";
		stra += "`XXX++XXXXXX+XX+++++++++++X+XXX`";
		stra += "`XXX++XXXXXX+XX++++++++++X+X+XX`";
		stra += "`XXX++XXXXXX+XX+++++++++++X+XXX`";
		stra += "`XXX++XXXXXX+XX++++++++++X+X+XX`";
		stra += "`XXX++XXXXXX+XX+++++++++++X+XXX`";
		stra += "`XXX++XXXXXX+XX++++++++++X+X+XX`";
		stra += "`XXX++XXXXXX+XX+++++++++++X+XXX`";
		stra += "`XXX++XXXXXX+XX++++++++++X+X+XX`";
		stra += "````````````````````````````````";
		stra += "  ````````````````````````````  ";
		
		_imageFill(&img, &stra, 32, 16, 112, 0);
	}

	// pipe
	{
		stra.clear();
		stra += "  `XXX++XXXXX+XX++++++++X+XXX`  ";
		stra += "  `XXX++XXXXX+XX+++++++++X+XX`  ";
		stra += "  `XXX++XXXXX+XX++++++++X+XXX`  ";
		stra += "  `XXX++XXXXX+XX+++++++++X+XX`  ";
		stra += "  `XXX++XXXXX+XX++++++++X+XXX`  ";
		stra += "  `XXX++XXXXX+XX+++++++++X+XX`  ";
		stra += "  `XXX++XXXXX+XX++++++++X+XXX`  ";
		stra += "  `XXX++XXXXX+XX+++++++++X+XX`  ";
		stra += "  `XXX++XXXXX+XX++++++++X+XXX`  ";
		stra += "  `XXX++XXXXX+XX+++++++++X+XX`  ";
		stra += "  `XXX++XXXXX+XX++++++++X+XXX`  ";
		stra += "  `XXX++XXXXX+XX+++++++++X+XX`  ";
		stra += "  `XXX++XXXXX+XX++++++++X+XXX`  ";
		stra += "  `XXX++XXXXX+XX+++++++++X+XX`  ";
		stra += "  `XXX++XXXXX+XX++++++++X+XXX`  ";
		stra += "  `XXX++XXXXX+XX+++++++++X+XX`  ";
		
		_imageFill(&img, &stra, 32, 16, 144, 0);

	}

	// coin 1/4
	{
		stra.clear();
		stra += "   @@   ";
		stra += "  @@@@  ";
		stra += " @@@@@@ ";
		stra += " @@#E@@ ";
		stra += "@@#@@E@@";
		stra += "@@#@@E@@";
		stra += "@@#@@E@@";
		stra += "@@#@@E@@";
		stra += "@@#@@E@@";
		stra += "@@#@@E@@";
		stra += " @@#E@@ ";
		stra += " @@@@@@ ";
		stra += "  @@@@  ";
		stra += "   @@   ";
		_imageFill(&img, &stra, 8, 14, 176, 0);
	}

	// coin 2/4
	{
		stra.clear();
		stra += "   @E   ";
		stra += "   @E   ";
		stra += "  @EEE  ";
		stra += "  @EEE  ";
		stra += "  @EEE  ";
		stra += "  @EEE  ";
		stra += "  #EEE  ";
		stra += "  #EEE  ";
		stra += "  @EEE  ";
		stra += "  @EEE  ";
		stra += "  @EEE  ";
		stra += "  @EEE  ";
		stra += "   @E   ";
		stra += "   @E   ";
		_imageFill(&img, &stra, 8, 14, 184, 0);
	}

	// coin 3/4
	{
		stra.clear();
		stra += "   @    ";
		stra += "   @    ";
		stra += "   @    ";
		stra += "   @    ";
		stra += "   @    ";
		stra += "   @    ";
		stra += "   #    ";
		stra += "   #    ";
		stra += "   @    ";
		stra += "   @    ";
		stra += "   @    ";
		stra += "   @    ";
		stra += "   @    ";
		stra += "   @    ";
		_imageFill(&img, &stra, 8, 14, 192, 0);
	}

	// coin 4/4
	{
		stra.clear();
		stra += "   #E   ";
		stra += "   #E   ";
		stra += "  ###E  ";
		stra += "  ###E  ";
		stra += "  ###E  ";
		stra += "  ###E  ";
		stra += "  ###E  ";
		stra += "  ###E  ";
		stra += "  ###E  ";
		stra += "  ###E  ";
		stra += "  ###E  ";
		stra += "  ###E  ";
		stra += "   #E   ";
		stra += "   #E   ";
		_imageFill(&img, &stra, 8, 14, 200, 0);
	}

	// Mario small stand
	{
		stra.clear();
		stra += "     EEEEE      ";
		stra += "    EEEEEEEEE   ";
		stra += "    ccc@@c@     ";
		stra += "   c@c@@@c@@@   ";
		stra += "   c@cc@@@c@@@  ";
		stra += "   cc@@@@cccc   ";
		stra += "     @@@@@@@    ";
		stra += "    ccEccc      ";
		stra += "   cccEccEccc   ";
		stra += "  ccccEEEEcccc  ";
		stra += "  @@cE@EE@Ec@@  ";
		stra += "  @@@EEEEEE@@@  ";
		stra += "  @@EEEEEEEE@@  ";
		stra += "    EEE  EEE    ";
		stra += "   ccc    ccc   ";
		stra += "  cccc    cccc  ";
		_imageFill(&img, &stra, 16, 16, 208, 0);
	}

	// Mario small walk 1/3
	{
		stra.clear();
		stra += "                ";
		stra += "      EEEEE     ";
		stra += "     EEEEEEEEE  ";
		stra += "     ccc@@c@    ";
		stra += "    c@c@@@c@@@  ";
		stra += "    c@cc@@@c@@@ ";
		stra += "    cc@@@@cccc  ";
		stra += "      @@@@@@@   ";
		stra += "     ccccEc @   ";
		stra += "    @cccccc@@@  ";
		stra += "   @@Eccccc@@   ";
		stra += "   ccEEEEEEE    ";
		stra += "   cEEEEEEEE    ";
		stra += "  ccEEE EEE     ";
		stra += "  c    ccc      ";
		stra += "       cccc     ";
		_imageFill(&img, &stra, 16, 16, 224, 0);
	}

	// Mario small walk 2/3
	{
		stra.clear();
		stra += "     EEEEE      ";
		stra += "    EEEEEEEEE   ";
		stra += "    ccc@@c@     ";
		stra += "   c@c@@@c@@@   ";
		stra += "   c@cc@@@c@@@  ";
		stra += "   cc@@@@cccc   ";
		stra += "     @@@@@@@    ";
		stra += "    ccEccc      ";
		stra += "   ccccEEcc     ";
		stra += "   cccEE@EE@    ";
		stra += "   ccccEEEEE    ";
		stra += "   Ecc@@@EEE    ";
		stra += "    Ec@@EEE     ";
		stra += "     EEEccc     ";
		stra += "     ccccccc    ";
		stra += "     cccc       ";
		_imageFill(&img, &stra, 16, 16, 240, 0);
	}

	// Mario small walk 3/3
	{
		stra.clear();
		stra += "      EEEEE     ";
		stra += "     EEEEEEEEE  ";
		stra += "     ccc@@c@    ";
		stra += "    c@c@@@c@@@  ";
		stra += "    c@cc@@@c@@@ ";
		stra += "    cc@@@@cccc  ";
		stra += "      @@@@@@@   ";
		stra += "   ccccEEcc     ";
		stra += " @@ccccEEEccc@@@";
		stra += " @@@ ccE@EEEcc@@";
		stra += " @@  EEEEEEE  c ";
		stra += "    EEEEEEEEEcc ";
		stra += "   EEEEEEEEEEcc ";
		stra += "  ccEEE   EEEcc ";
		stra += "  ccc           ";
		stra += "   ccc          ";
		_imageFill(&img, &stra, 16, 16, 256, 0);
	}

	// Mario small jump
	{
		stra.clear();
		stra += "             @@@";
		stra += "      EEEEE  @@@";
		stra += "     EEEEEEEEE@@";
		stra += "     ccc@@c@ ccc";
		stra += "    c@c@@@c@@ccc";
		stra += "    c@cc@@@c@@@c";
		stra += "    cc@@@@ccccc ";
		stra += "      @@@@@@@c  ";
		stra += "  cccccEcccEc   ";
		stra += " cccccccEcccE  c";
		stra += "@@ccccccEEEEE  c";
		stra += "@@@ EEcEE@EE@Ecc";
		stra += " @ cEEEEEEEEEEcc";
		stra += "  cccEEEEEEEEEcc";
		stra += " cccEEEEEEE     ";
		stra += " c  EEEE        ";
		_imageFill(&img, &stra, 16, 16, 272, 0);
	}

	// Mario cry
	{
		stra.clear();
		stra += "                ";
		stra += "      EEEE      ";
		stra += "   @ EEEEEE @   ";
		stra += " @@@c@c@@c@c@@@ ";
		stra += " @@cc@c@@c@cc@@ ";
		stra += " @@ccc@@@@ccc@@ ";
		stra += "   cccc@@cccc   ";
		stra += "    c@cccc@c    ";
		stra += "    c@@@@@@c    ";
		stra += "   EEE@@@@EEE   ";
		stra += "  ccEEccccEEcc  ";
		stra += "  cccEEccEEccc  ";
		stra += "  cccE@EE@Eccc  ";
		stra += "  cccEEEEEEccc  ";
		stra += "   ccEEEEEEcc   ";
		stra += "                ";
		_imageFill(&img, &stra, 16, 16, 288, 0);
	}

	// Mario break
	{
		stra.clear();
		stra += "      EEEEE     ";
		stra += "    cEEEEEEEE   ";
		stra += "   cccccc@c@    ";
		stra += "  @@c@@c@@@@@@  ";
		stra += "  @@c@@cc@@cc@@ ";
		stra += "   @@c@@@@@@cc  ";
		stra += "    EEEcccE@@   ";
		stra += "   EE@@@cEEccc  ";
		stra += "   Ec@@@cccccc  ";
		stra += "   EEE@@cccccc  ";
		stra += "    EEEEEcccc   ";
		stra += "    EcccEEEE    ";
		stra += "     ccccEEE    ";
		stra += "  c cEEcccE     ";
		stra += "  cccccE        ";
		stra += "   cccc         ";
		_imageFill(&img, &stra, 16, 16, 304, 0);
	}

	// cloud1
	{
		stra.clear();
		stra += "              ````              ";
		stra += "             `####`             ";
		stra += "           ``######`            ";
		stra += "          `########` `          ";
		stra += "          `#########`#`         ";
		stra += "          `######[#####`        ";
		stra += "         `###[[###[####`        ";
		stra += "        `###[##########`        ";
		stra += "     ```################`  `    ";
		stra += "    `###################` `#`   ";
		stra += "   `#####################`##`   ";
		stra += "   #########################` ` ";
		stra += " ``##########################`#`";
		stra += "`##############################`";
		stra += "`##############################`";
		stra += " `############################` ";
		stra += "  `##[###########[###########`  ";
		stra += "   `##[##[######[#############` ";
		stra += "    `##[[[[###[[[[####[########`";
		stra += "    `#####[[[[[[#[[[[[######### ";
		stra += "     ```####[[####[[[########`` ";
		stra += "        `######`########`##``   ";
		stra += "         ``###` ``####`` ``     ";
		stra += "           ```    ````          ";

		_imageFill(&img, &stra, 32, 24, 0, 16);
	}

	// cloud2
	{
		stra.clear();    //             //
		stra += "              ````            ````              ";
		stra += "             `####`          `####`             ";
		stra += "           ``######`       ``######`            ";
		stra += "          `########` `    `########` `          ";
		stra += "          `#########`#`   `#########`#`         ";
		stra += "          `######[#####`  `######[#####`        ";
		stra += "         `###[[###[####` `###[[###[####`        ";
		stra += "        `###[##########``###[##########`        ";
		stra += "     ```################################`  `    ";
		stra += "    `###################################` `#`   ";
		stra += "   `#####################################`##`   ";
		stra += "   #########################################` ` ";
		stra += " ``##########################################`#`";
		stra += "`##############################################`";
		stra += "`##############################################`";
		stra += " `############################################` ";
		stra += "  `##[###########[###############[###########`  ";
		stra += "   `##[##[######[########[######[#############` ";
		stra += "    `##[[[[###[[[[####[#[[[###[[[[####[########`";
		stra += "    `#####[[[[[[#[[[[[####[[[[[[#[[[[[######### ";
		stra += "     ```####[[####[[[#######[[####[[[########`` ";
		stra += "        `######`########`######`########`##``   ";
		stra += "         ``###` ``####`` ``###` ``####`` ``     ";
		stra += "           ```    ````     ```    ````          ";

		_imageFill(&img, &stra, 48, 24, 32, 16);
	}

	// cloud3
	{
		stra.clear();    //             //
		stra += "              ````            ````            ````              ";
		stra += "             `####`          `####`          `####`             ";
		stra += "           ``######`       ``######`       ``######`            ";
		stra += "          `########` `    `########` `    `########` `          ";
		stra += "          `#########`#`   `#########`#`   `#########`#`         ";
		stra += "          `######[#####`  `######[#####`  `######[#####`        ";
		stra += "         `###[[###[####` `###[[###[####` `###[[###[####`        ";
		stra += "        `###[##########``###[##########``###[##########`        ";
		stra += "     ```################################################`  `    ";
		stra += "    `###################################################` `#`   ";
		stra += "   `#####################################################`##`   ";
		stra += "   #########################################################` ` ";
		stra += " ``##########################################################`#`";
		stra += "`##############################################################`";
		stra += "`##############################################################`";
		stra += " `############################################################` ";
		stra += "  `##[###########[###############[###############[###########`  ";
		stra += "   `##[##[######[########[######[########[######[#############` ";
		stra += "    `##[[[[###[[[[####[#[[[###[[[[####[#[[[###[[[[####[########`";
		stra += "    `#####[[[[[[#[[[[[####[[[[[[#[[[[[####[[[[[[#[[[[[######### ";
		stra += "     ```####[[####[[[#######[[####[[[#######[[####[[[########`` ";
		stra += "        `######`########`######`########`######`########`##``   ";
		stra += "         ``###` ``####`` ``###` ``####`` ``###` ``####`` ``     ";
		stra += "           ```    ````     ```    ````     ```    ````          ";

		_imageFill(&img, &stra, 64, 24, 80, 16);
	}

	// Gumba walk 1/2
	{
		stra.clear();
		stra += "      %%%%      ";
		stra += "     %%%%%%     ";
		stra += "    %%%%%%%%    ";
		stra += "   %%%%%%%%%%   ";
		stra += "  %``%%%%%%``%  ";
		stra += " %%%^`%%%%`^%%% ";
		stra += " %%%^``````^%%% ";
		stra += "%%%%^`^%%^`^%%%%";
		stra += "%%%%^^^%%^^^%%%%";
		stra += "%%%%%%%%%%%%%%%%";
		stra += " %%%%^^^^^^%%%% ";
		stra += "    ^^^^^^^^    ";
		stra += "    ^^^^^^^^``  ";
		stra += "   ``^^^^^````` ";
		stra += "   ```^^^`````` ";
		stra += "    ```^^`````  ";
		_imageFill(&img, &stra, 16, 16, 144, 16);
	}

	// Gumba walk 2/2
	{
		stra.clear();
		stra += "      %%%%      ";
		stra += "     %%%%%%     ";
		stra += "    %%%%%%%%    ";
		stra += "   %%%%%%%%%%   ";
		stra += "  %``%%%%%%``%  ";
		stra += " %%%^`%%%%`^%%% ";
		stra += " %%%^``````^%%% ";
		stra += "%%%%^`^%%^`^%%%%";
		stra += "%%%%^^^%%^^^%%%%";
		stra += "%%%%%%%%%%%%%%%%";
		stra += " %%%%^^^^^^%%%% ";
		stra += "    ^^^^^^^^    ";
		stra += "  ``^^^^^^^^    ";
		stra += " `````^^^^^``   ";
		stra += " ``````^^^```   ";
		stra += "  `````^^```    ";
		_imageFill(&img, &stra, 16, 16, 160, 16);
	}

	// Gumba ground
	{
		stra.clear();
		stra += "                ";
		stra += "                ";
		stra += "                ";
		stra += "                ";
		stra += "                ";
		stra += "                ";
		stra += "                ";
		stra += "                ";
		stra += "      %%%%      ";
		stra += "   %%%%%%%%%%   ";
		stra += " %%```%%%%```%% ";
		stra += "%%^^^^````^^^^%%";
		stra += "%%%%%%%%%%%%%%%%";
		stra += "   ^^^^^^^^^^   ";
		stra += "  ^^^^^^^^^^^^  ";
		stra += " `````    ````` ";
		_imageFill(&img, &stra, 16, 16, 176, 16);
	}

	// hill
	{
		stra.clear();
		stra += "                                     ``````                                     ";
		stra += "                                  ```++++++```                                  ";
		stra += "                                ``++++++++++++``                                ";
		stra += "                               `+++++++++++++`++`                               ";
		stra += "                              `+++++++++++++```++`                              ";
		stra += "                             `++++++++++++++```+++`                             ";
		stra += "                            `+++++++++++++++```++++`                            ";
		stra += "                           `+++++++++++++``+```+++++`                           ";
		stra += "                          `++++++++++++++``++`+++++++`                          ";
		stra += "                         `+++++++++++++++``+++++++++++`                         ";
		stra += "                        `++++++++++++++++``++++++++++++`                        ";
		stra += "                       `++++++++++++++++++++++++++++++++`                       ";
		stra += "                      `++++++++++++++++++++++++++++++++++`                      ";
		stra += "                     `++++++++++++++++++++++++++++++++++++`                     ";
		stra += "                    `++++++++++++++++++++++++++++++++++++++`                    ";
		stra += "                   `++++++++++++++++++++++++++++++++++++++++`                   ";
		stra += "                  `++++++++++++++++++++++++++++++++++++++++++`                  ";
		stra += "                 `++++++++++++++++++++++++++++++++++++++++++++`                 ";
		stra += "                `++++++++++++++++++++++++++++++++++++++++++++++`                ";
		stra += "               `+++++++++++++`+++++++++++++++++++++++`++++++++++`               ";
		stra += "              `+++++++++++++```+++++++++++++++++++++```++++++++++`              ";
		stra += "             `++++++++++++++```+++++++++++++++++++++```+++++++++++`             ";
		stra += "            `+++++++++++++++```+++++++++++++++++++++```++++++++++++`            ";
		stra += "           `+++++++++++++``+```++++++++++++++++++``+```+++++++++++++`           ";
		stra += "          `++++++++++++++``++`+++++++++++++++++++``++`+++++++++++++++`          ";
		stra += "         `+++++++++++++++``++++++++++++++++++++++``+++++++++++++++++++`         ";
		stra += "        `++++++++++++++++``++++++++++++++++++++++``++++++++++++++++++++`        ";
		stra += "       `++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++`       ";
		stra += "      `++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++`      ";
		stra += "     `++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++`     ";
		stra += "    `++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++`    ";
		stra += "   `++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++`   ";
		stra += "  `++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++`  ";
		stra += " `++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++` ";
		stra += "`++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++`";

		_imageFill(&img, &stra, 80, 35, 0, 40);
	}

	m_texture = m_gs->SummonTexture(&img);

	return true;
}

void ExampleGameMario::Shutdown()
{
	BQ_SAFEDESTROY(m_texture);
	BQ_SAFEDESTROY(m_camera);
}

void ExampleGameMario::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->m_projectionMatrix * m_camera->m_viewMatrix;
	
	m_gs->BeginGUI();
	m_gs->DrawGUIRectangle(bqVec4f(10.f, 10.f, 512.f, 512.f), bq::ColorWhite, bq::ColorWhite, m_texture, 0);
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

void ExampleGameMario::_imageFill(
	bqImage* img,
	bqStringA* str, 
	uint32_t w,
	uint32_t h,
	uint32_t whereX,
	uint32_t whereY)
{
	static bqStringA strToImg;
	strToImg.clear();
	for (size_t i = 0; i < str->size(); ++i)
	{
		char ch = str->data()[i];
		strToImg.push_back(m_colorMap[ch]);
	}

	img->Fill(m_palette, (uint8_t*)strToImg.data(), w, h, whereX, whereY);
}
