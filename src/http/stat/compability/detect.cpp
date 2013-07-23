/*
Copyright (c) 2013 Lukasz Magiera

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/
#include "../../stat.h"
#include "compability.h"
#include "nativehttp.h"

namespace http
{
	namespace statdata
	{

        namespace compability
        {
            bool tryload(FILE *sd)
            {
                nativehttp::server::log("detect.cpp@stat-compability", "Detecting version");
                fseek(sd,3,SEEK_SET);
                uint16_t tfv = 0x0000;
				fread(&tfv, 2, 1, sd);

				switch(tfv)
				{
                    case 0x0002:
                        nativehttp::server::log("detect.cpp@stat-compability", "FILE: v2 - Native Endian");
                        return load_0x0002(sd);
                        break;
                    default:
                        fclose(sd);
				}


                return false;
            }
        }

	}

}
