#include <glib.h>
#include <math.h>
#include "stb_image.inc"

#define SCALE 16

static const char *font_name = NULL;


int rw, rh;
unsigned char *fb;
int stride;

static char ufo_path[2048];

const char *glyphs = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

GString *contents_plist = NULL;

void gen_glyph (int glyph_no, int x0, int y0, int x1, int y1)
{
  GString *str = g_string_new ("");
  int x, y;
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%X\" format=\"1\">\n", glyphs[glyph_no]);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", (x1-x0+1) * SCALE);
  g_string_append_printf (str, "  <unicode hex=\"%04X\"/>\n", glyphs[glyph_no]);
  g_string_append_printf (str, "  <outline>\n");
  for (y = y0; y <= y1; y++)
    {
      for (x = x0; x <= x1; x++)
        {
          int u, v;
          u = x - x0;
          v = y1 - y -1;
          unsigned char *pix = &fb[stride * y+x*4];
          if (*pix < 32)
            g_string_append_printf (str, "  <component base=\"solid\" xOffset=\"%d\" yOffset=\"%d\"/>\n", u * SCALE, v * SCALE);
          else if (*pix < 120)
            g_string_append_printf (str, "  <component base=\"dark\" xOffset=\"%d\" yOffset=\"%d\"/>\n", u * SCALE, v * SCALE);
          else if (*pix < 240)
            g_string_append_printf (str, "  <component base=\"bright\" xOffset=\"%d\" yOffset=\"%d\"/>\n", u * SCALE, v * SCALE);
          else
            ;
        }
    }
 g_string_append_printf (str, "  </outline>\n");
 g_string_append_printf (str, "</glyph>\n");
 char buf[1024];
 sprintf (buf, "%s/glyphs/%X.glif", ufo_path, glyphs[glyph_no]);
 g_file_set_contents (buf, str->str, str->len, NULL);
 g_string_free (str, TRUE);

 g_string_append_printf (contents_plist, "<key>%X</key><string>%X.glif</string>\n", glyphs[glyph_no], glyphs[glyph_no]);
}

void gen_blocks ();
void gen_fontinfo (int glyph_height);

int main (int argc, char **argv)
{
  int y0 = 0, y1 = 0;

  if (argc != 3)
    {
      fprintf (stderr, "Usage: %s <fontimage.png> <outputfontname>\n", argv[0]);
      return -1;
    }

  font_name = argv[2];
  sprintf (ufo_path, "%s.ufo", font_name);
  char buf[2048];
  sprintf (buf, "mkdir %s > /dev/null 2>&1", ufo_path); system (buf);
  sprintf (buf, "mkdir %s/glyphs > /dev/null 2>&1", ufo_path); system (buf);
  sprintf (buf, "%s/metainfo.plist", ufo_path);

  g_file_set_contents (buf,
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
"<plist version=\"1.0\"> <dict> <key>creator</key> <string>org.gimp.pippin</string> <key>formatVersion</key> <integer>2</integer> </dict> </plist>", -1, NULL);


  fb = stbi_load (argv[1], &rw, &rh, NULL, 4);
  if (!fb)
    return 0;
  int y, x0;
  int glyph_no = 0;

  stride = rw * 4;

  contents_plist = g_string_new (
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\"\n"
"\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
"<plist version=\"1.0\">\n"
"<dict>\n");

  /* determine glyph height */
  for (y0 = 0; y0 < rh && fb[rw * 4 * y0]==255; y0++);
  for (y1 = y0; y1 < rh && fb[rw * 4 * y0]<32; y1++);
  y1--;

  printf ("rw: %d rh: %d y0: %d y1: %d\n", rw, rh, y0, y1);

  /* iterate through glyphs as delimited on first scanline */
  for (x0 = 0; x0 < rw; x0++)
    {
      if (fb[x0 * 4] < 255)
        {
          int x1;
          for (x1 = x0+1; fb[x1 * 4] == 255; x1++);
          x1--;
          if (x1 > rw)
            break;
          gen_glyph (glyph_no, x0, y0, x1, y1);
          glyph_no++;
        }
    }
  gen_blocks ();
 
  g_string_append (contents_plist, "</dict>\n</plist>\n");
  sprintf (buf, "%s/glyphs/contents.plist", ufo_path);
  g_file_set_contents (buf, contents_plist->str, contents_plist->len, NULL);

  gen_fontinfo (y1-y0);

  return 0;
}


void gen_blocks ()
{
 char buf[1024];
  GString *str;
  str = g_string_new ("");

  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"solid\" format=\"1\">\n");
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <unicode hex=\"0703\"/>\n");
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1 + 2);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1 + 2, SCALE * 1 + 2);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1 + 2, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, "solid");
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);

  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"bright\" format=\"1\">\n");
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <unicode hex=\"0121\"/>\n");
  g_string_append_printf (str, "  <outline>\n");
  int i;
  for (i = 0; i < SCALE; i++)
  {
    if (i % 3 == 0)
    {
      g_string_append_printf (str, "    <contour>\n");
        g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, i+1);
        g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, i+1);
        g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, i);
        g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, i);
      g_string_append_printf (str, "    </contour>\n");
    }
  }

  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, "bright");
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);

  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"dark\" format=\"1\">\n");
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <unicode hex=\"0122\"/>\n");
  g_string_append_printf (str, "  <outline>\n");
  for (i = 0; i < SCALE; i++)
  {
    if (i % 2)
    {
      g_string_append_printf (str, "    <contour>\n");
        g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, i+1);
        g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, i+1);
        g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, i);
        g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, i);
      g_string_append_printf (str, "    </contour>\n");
    }
  }
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, "dark");
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);

  g_string_append_printf (contents_plist, "<key>solid</key><string>solid.glif</string>\n");
  g_string_append_printf (contents_plist, "<key>dark</key><string>dark.glif</string>\n");
  g_string_append_printf (contents_plist, "<key>bright</key><string>bright.glif</string>\n");

}

void gen_fontinfo (int glyph_height)
{
  char buf[2048];
  sprintf (buf, "%s/fontinfo.plist", ufo_path);
  GString *str = g_string_new ("");


  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n");
  g_string_append_printf (str, "<plist version=\"1.0\">\n");
  g_string_append_printf (str, "    <dict>\n");
  g_string_append_printf (str, "	<key>familyName</key>\n");
  g_string_append_printf (str, "	<string>%s</string>\n", font_name);
  g_string_append_printf (str, "	<key>styleName</key>\n");
  g_string_append_printf (str, "	<string>AA</string>\n");
  g_string_append_printf (str, "	<key>copyright</key>\n");
  g_string_append_printf (str, "	<string>OEyvind Kolaas pippin@gimp.org</string>\n");
  g_string_append_printf (str, "	<key>unitsPerEm</key>\n");
  g_string_append_printf (str, "	<integer>%i</integer>\n", SCALE * glyph_height);
  g_string_append_printf (str, "	<key>xHeight</key>\n");
  g_string_append_printf (str, "	<integer>192</integer>\n");
  g_string_append_printf (str, "	<key>capHeight</key>\n");
  g_string_append_printf (str, "	<integer>320</integer>\n");
  g_string_append_printf (str, "	<key>ascender</key>\n");
  g_string_append_printf (str, "	<integer>800</integer>\n");
  g_string_append_printf (str, "	<key>descender</key>\n");
  g_string_append_printf (str, "	<integer>-200</integer>\n");
  g_string_append_printf (str, "	<key>italicAngle</key>\n");
  g_string_append_printf (str, "	<real>0</real>\n");
  g_string_append_printf (str, "	<key>note</key>\n");
  g_string_append_printf (str, "	<string></string>\n");
  g_string_append_printf (str, "	<key>openTypeHeadCreated</key>\n");
  g_string_append_printf (str, "	<string>2013/03/29 02:59:45</string>\n");
  g_string_append_printf (str, "	<key>openTypeNameLicenseURL</key>\n");
  g_string_append_printf (str, "	<string>http://scripts.sil.org/OFL</string>\n");
  g_string_append_printf (str, "	<key>openTypeNameVersion</key>\n");
  g_string_append_printf (str, "	<string>Version 01.000</string>\n");
  g_string_append_printf (str, "	<key>openTypeOS2Panose</key>\n");
  g_string_append_printf (str, "	<array>\n");
  g_string_append_printf (str, "		<integer>2</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>6</integer>\n");
  g_string_append_printf (str, "		<integer>3</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "	</array>\n");
  g_string_append_printf (str, "	<key>openTypeOS2WidthClass</key>\n");
  g_string_append_printf (str, "	<integer>5</integer>\n");
  g_string_append_printf (str, "	<key>openTypeOS2WeightClass</key>\n");
  g_string_append_printf (str, "	<integer>500</integer>\n");
  g_string_append_printf (str, "	<key>openTypeOS2VendorID</key>\n");
  g_string_append_printf (str, "	<string>PfEd</string>\n");
  g_string_append_printf (str, "	<key>openTypeOS2FamilyClass</key>\n");
  g_string_append_printf (str, "	<array>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "	</array>\n");
  g_string_append_printf (str, "	<key>openTypeOS2Type</key>\n");
  g_string_append_printf (str, "	<array>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>1</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "		<integer>0</integer>\n");
  g_string_append_printf (str, "	</array>\n");
  g_string_append_printf (str, "	<key>postscriptFontName</key>\n");
  g_string_append_printf (str, "	<string>%s</string>\n", font_name);
  g_string_append_printf (str, "	<key>postscriptFullName</key>\n");
  g_string_append_printf (str, "	<string>%s</string>\n", font_name);
  g_string_append_printf (str, "	<key>postscriptWeightName</key>\n");
  g_string_append_printf (str, "	<string>Regular</string>\n");
  g_string_append_printf (str, "    </dict>\n");
  g_string_append_printf (str, "</plist>\n");


  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
}
