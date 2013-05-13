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
#include "protocol.h"
#include <stdio.h>

string smime_types = "a=application/octet-stream\n\
aab=application/x-authorware-bin\n\
aam=application/x-authorware-map\n\
aas=application/x-authorware-seg\n\
ai=application/postscript\n\
aif=audio/x-aiff\n\
aifc=audio/x-aiff\n\
aiff=audio/x-aiff\n\
asc=text/plain\n\
asf=video/x-ms-asf\n\
asx=video/x-ms-asf\n\
au=audio/basic\n\
avi=video/x-msvideo\n\
bcpio=application/x-bcpio\n\
bin=application/octet-stream\n\
bmp=image/bmp\n\
cdf=application/x-netcdf\n\
class=application/x-java-vm\n\
cpio=application/x-cpio\n\
cpt=application/mac-compactpro\n\
crl=application/x-pkcs7-crl\n\
crt=application/x-x509-ca-cert\n\
csh=application/x-csh\n\
css=text/css\n\
dcr=application/x-director\n\
dir=application/x-director\n\
djv=image/vnd.djvu\n\
djvu=image/vnd.djvu\n\
dll=application/octet-stream\n\
dms=application/octet-stream\n\
doc=application/msword\n\
dtd=text/xml\n\
dump=application/octet-stream\n\
dvi=application/x-dvi\n\
dxr=application/x-director\n\
eps=application/postscript\n\
etx=text/x-setext\n\
exe=application/octet-stream\n\
ez=application/andrew-inset\n\
fgd=application/x-director\n\
fh=image/x-freehand\n\
fh4=image/x-freehand\n\
fh5=image/x-freehand\n\
fh7=image/x-freehand\n\
fhc=image/x-freehand\n\
gif=image/gif\n\
gtar=application/x-gtar\n\
hdf=application/x-hdf\n\
hqx=application/mac-binhex40\n\
ice=x-conference/x-cooltalk\n\
ief=image/ief\n\
iges=model/iges\n\
igs=model/iges\n\
iv=application/x-inventor\n\
jar=application/x-java-archive\n\
jfif=image/jpeg\n\
jpe=image/jpeg\n\
jpeg=image/jpeg\n\
jpg=image/jpeg\n\
js=application/x-javascript\n\
kar=audio/midi\n\
latex=application/x-latex\n\
lha=application/octet-stream\n\
lzh=application/octet-stream\n\
m3u=audio/x-mpegurl\n\
man=application/x-troff-man\n\
mathml=application/mathml+xml\n\
me=application/x-troff-me\n\
mesh=model/mesh\n\
mid=audio/midi\n\
midi=audio/midi\n\
mif=application/vnd.mif\n\
mime=message/rfc822\n\
mml=application/mathml+xml\n\
mov=video/quicktime\n\
movie=video/x-sgi-movie\n\
mp2=audio/mpeg\n\
mp3=audio/mpeg\n\
mp4=video/mp4\n\
mpe=video/mpeg\n\
mpeg=video/mpeg\n\
mpg=video/mpeg\n\
mpga=audio/mpeg\n\
ms=application/x-troff-ms\n\
msh=model/mesh\n\
mv=video/x-sgi-movie\n\
mxu=video/vnd.mpegurl\n\
nc=application/x-netcdf\n\
o=application/octet-stream\n\
oda=application/oda\n\
ogg=application/x-ogg\n\
pac=application/x-ns-proxy-autoconfig\n\
pbm=image/x-portable-bitmap\n\
pdb=chemical/x-pdb\n\
pdf=application/pdf\n\
pgm=image/x-portable-graymap\n\
pgn=application/x-chess-pgn\n\
png=image/png\n\
pnm=image/x-portable-anymap\n\
ppm=image/x-portable-pixmap\n\
ppt=application/vnd.ms-powerpoint\n\
ps=application/postscript\n\
qt=video/quicktime\n\
ra=audio/x-realaudio\n\
ram=audio/x-pn-realaudio\n\
ras=image/x-cmu-raster\n\
rdf=application/rdf+xml\n\
rgb=image/x-rgb\n\
rm=audio/x-pn-realaudio\n\
roff=application/x-troff\n\
rpm=audio/x-pn-realaudio-plugin\n\
rss=application/rss+xml\n\
rtf=text/rtf\n\
rtx=text/richtext\n\
sgm=text/sgml\n\
sgml=text/sgml\n\
sh=application/x-sh\n\
shar=application/x-shar\n\
silo=model/mesh\n\
sit=application/x-stuffit\n\
skd=application/x-koan\n\
skm=application/x-koan\n\
skp=application/x-koan\n\
skt=application/x-koan\n\
smi=application/smil\n\
smil=application/smil\n\
snd=audio/basic\n\
so=application/octet-stream\n\
spl=application/x-futuresplash\n\
src=application/x-wais-source\n\
stc=application/vnd.sun.xml.calc.template\n\
std=application/vnd.sun.xml.draw.template\n\
sti=application/vnd.sun.xml.impress.template\n\
stw=application/vnd.sun.xml.writer.template\n\
sv4cpio=application/x-sv4cpio\n\
sv4crc=application/x-sv4crc\n\
svg=image/svg+xml\n\
svgz=image/svg+xml\n\
swf=application/x-shockwave-flash\n\
sxc=application/vnd.sun.xml.calc\n\
sxd=application/vnd.sun.xml.draw\n\
sxg=application/vnd.sun.xml.writer.global\n\
sxi=application/vnd.sun.xml.impress\n\
sxm=application/vnd.sun.xml.math\n\
sxw=application/vnd.sun.xml.writer\n\
t=application/x-troff\n\
tar=application/x-tar\n\
tcl=application/x-tcl\n\
tex=application/x-tex\n\
texi=application/x-texinfo\n\
texinfo=application/x-texinfo\n\
tif=image/tiff\n\
tiff=image/tiff\n\
tr=application/x-troff\n\
tsp=application/dsptype\n\
tsv=text/tab-separated-values\n\
ustar=application/x-ustar\n\
vcd=application/x-cdlink\n\
vrml=model/vrml\n\
vx=video/x-rad-screenplay\n\
wav=audio/x-wav\n\
wax=audio/x-ms-wax\n\
wbmp=image/vnd.wap.wbmp\n\
wbxml=application/vnd.wap.wbxml\n\
wm=video/x-ms-wm\n\
wma=audio/x-ms-wma\n\
wmd=application/x-ms-wmd\n\
wml=text/vnd.wap.wml\n\
wmlc=application/vnd.wap.wmlc\n\
wmls=text/vnd.wap.wmlscript\n\
wmlsc=application/vnd.wap.wmlscriptc\n\
wmv=video/x-ms-wmv\n\
wmx=video/x-ms-wmx\n\
wmz=application/x-ms-wmz\n\
wrl=model/vrml\n\
wsrc=application/x-wais-source\n\
wvx=video/x-ms-wvx\n\
xbm=image/x-xbitmap\n\
xht=application/xhtml+xml\n\
xhtml=application/xhtml+xml\n\
xls=application/vnd.ms-excel\n\
xml=text/xml\n\
xpm=image/x-xpixmap\n\
xsl=text/xml\n\
xwd=image/x-xwindowdump\n\
xyz=chemical/x-xyz\n\
zip=application/zip\n";

string smime_types_charset = "txt=text/plain; charset=%s\n\
htm=text/html; charset=%s\n\
html=text/html; charset=%s\n";

string default_mime;

mimec::mimec()
{
	nativehttp::data::superstring ts(smime_types_charset);
	ts.change("%s", charset);

	nativehttp::data::superstring mp(ts.str + smime_types);
	int n = mp.count('\n');
	mp.pos = 0;
	for (int i = 0; i < n; i++)
	{
		string tnam = mp.to("=");
		string tmi = mp.to("\n");
		mimes[tnam] = tmi;
	}

}


string mimec::get_ctype(string fn)
{
	nativehttp::data::superstring fnm(fn);
	fnm.pos = fn.size() - 1;
	string fext = fnm.back_to(".");
	string tm = mimes[fext];
	if (!tm.empty())
	{
		string rt = "Content-Type: ";
		rt += tm;
		return rt;
	}

	return default_mime;
}

