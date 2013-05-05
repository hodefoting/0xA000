#include <glib.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SCALE 512

#define OVERLAP_SOLID 1

typedef struct _Mapping Mapping;

  enum {
    C_BLANK=0,
    C_SOLID,
    C_DARK,
    C_BRIGHT,
    C_CNW,
    C_CNE,
    C_CSW,
    C_CSE,
    C_LNW,
    C_LNE,
    C_LSW,
    C_LSE,
    C_VE,
    C_VW,
    C_VN,
    C_VS
  };

  struct _Mapping {
    gchar   ascii;
    gchar  *name;
    int     type;
  };
  char *mem_read (char *start,
                  char *linebuf,
                  int  *len);

  static Mapping map[256]={{0,},};
static int     mappings=0;

static int map_pix (char pix)
{
  int i;
  for (i = 0; i < mappings; i++)
    if (map[i].ascii == pix)
      return map[i].type;
  return C_BLANK;
}


static const char *font_name = NULL;
static const char *font_type = NULL;
static int   y_shift = 0;

static char *asc_source = NULL;

int rw, rh;
unsigned int *fb;
int stride;

static char ufo_path[2048];

const char *glyphs = "";

gunichar *uglyphs = NULL;
glong n_glyphs;

GString *contents_plist = NULL;
GString *str = NULL;

int glyph_height;

GString *ascii_font = NULL;

void gen_glyph (int glyph_no, int x0, int y0, int x1, int y1)
{
  GString *str;
  gchar utf8_chr[8]={0,0,0,0,};
  int x, y;
  x0++;
  x1++;


  if (glyph_no >= n_glyphs)
    return;

  if (y1 - y0 > glyph_height)
    glyph_height = y1 - y0 - 1;

  g_unichar_to_utf8 (uglyphs[glyph_no], utf8_chr);
  str = g_string_new ("");

  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%X\" format=\"1\">\n", uglyphs[glyph_no]);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", (x1-x0+1) * SCALE);
  g_string_append_printf (str, "  <unicode hex=\"%04X\"/>\n", uglyphs[glyph_no]);
  g_string_append_printf (str, "  <outline>\n");

#if OVERLAP_SOLID
  for (y = y0; y <= y1; y++)
    for (x = x0; x <= x1; x++)
      {
        int *pix = &fb[stride * y+x];
        int u = x - x0;
        int v = y1 - y -1 + y_shift;
        if (*pix == C_SOLID)
          {
            if (y <y1 && fb[stride *(y+1)+x] == C_SOLID)
              {
          g_string_append_printf (str, "  <component base=\"solidv\" xOffset=\"%d\" yOffset=\"%d\"/>\n", u * SCALE, v * SCALE); 
          if (x <x1 && fb[stride *(y)+(x+1)] == C_SOLID)
            g_string_append_printf (str, "  <component base=\"solidh\" xOffset=\"%d\" yOffset=\"%d\"/>\n", u * SCALE, v * SCALE); 
              }
            else
              {
          if (x <x1 && fb[stride *(y)+(x+1)] == C_SOLID)
            g_string_append_printf (str, "  <component base=\"solidh\" xOffset=\"%d\" yOffset=\"%d\"/>\n", u * SCALE, v * SCALE); 
          else
            g_string_append_printf (str, "  <component base=\"solid\" xOffset=\"%d\" yOffset=\"%d\"/>\n", u * SCALE, v * SCALE); 
              }
          }
      }
#endif

  for (y = y0; y <= y1; y++)
    for (x = x0; x <= x1; x++)
      {
        int *pix = &fb[stride * y+x];
        int u = x - x0;
        int v = y1 - y -1 + y_shift;
        const char *component = NULL;

        switch (*pix)
          {
            case C_SOLID: 
#if !OVERLAP_SOLID
              component = "solid";
#endif
            break;

            case C_CNE: component = "cne"; break;
            case C_CNW: component = "cnw"; break;
            case C_CSE: component = "cse"; break;
            case C_CSW: component = "csw"; break;
            case C_LNE: component = "lne"; break;
            case C_LNW: component = "lnw"; break;
            case C_LSE: component = "lse"; break;
            case C_LSW: component = "lsw"; break;
            case C_VE:  component = "ve"; break;
            case C_VW:  component = "vw"; break;
            case C_VS:  component = "vs"; break;
            case C_VN:  component = "vn"; break;
            case C_DARK:  component = "dark"; break;
            case C_BRIGHT: component = "bright"; break;
            case C_BLANK: component = NULL;
          }
        if (component)
          g_string_append_printf (str, "  <component base=\"%s\" xOffset=\"%d\" yOffset=\"%d\"/>\n", component, u * SCALE, v * SCALE);
      }
 g_string_append_printf (str, "  </outline>\n");
 g_string_append_printf (str, "</glyph>\n");
 char buf[1024];
 sprintf (buf, "%s/glyphs/%X.glif", ufo_path, uglyphs[glyph_no]);
 g_file_set_contents (buf, str->str, str->len, NULL);
 g_string_free (str, TRUE);

 g_string_append_printf (contents_plist, "<key>%X</key><string>%X.glif</string>\n", uglyphs[glyph_no], uglyphs[glyph_no]);
}

void gen_blocks ();
void gen_fontinfo (int glyph_height);

void import_includes (char **asc_source)
{
  GString *new = g_string_new ("");
  char linebuf[1024];
  int len;
  char *p = *asc_source;

  do {
    p = mem_read (p, linebuf, &len);
    if (p)
      {
        if (g_str_has_prefix (linebuf, "include "))
        {
          if (1)
          {
          char *read = NULL;
          g_file_get_contents (&linebuf[strlen("include ")], &read, NULL, NULL);
          if (read)
            {
              g_string_append_printf (new, "%s", read);
              g_free (read);
            }
          }
        }
        else
        {
          g_string_append_printf (new, "%s\n", linebuf);
        }
      }
  } while (p);

  *asc_source = new->str;
  g_string_free (new, FALSE);
}

int main (int argc, char **argv)
{
  int y0 = 0, y1 = 0;

  if (argc != 5)
    {
      fprintf (stderr, "Usage: %s <fontimage.png> <outputfontname> <yshift>\n", argv[0]);
      return -1;
    }

  uglyphs = NULL;

  font_name = argv[2];
  font_type = argv[3];
  glyphs    = NULL;
  y_shift = atoi(argv[4]);

  ascii_font = g_string_new ("");

  g_string_append_printf (ascii_font,
      ". blank    # period\n"
      "8 solid    # eight\n"
      "7 dark     # seven\n"
      "+ bright   # plus\n"
      "\n");

  sprintf (ufo_path, "%s.ufo", font_name);
  char buf[2048];
  sprintf (buf, "mkdir %s > /dev/null 2>&1", ufo_path); system (buf);
  sprintf (buf, "mkdir %s/glyphs > /dev/null 2>&1", ufo_path); system (buf);
  sprintf (buf, "%s/metainfo.plist", ufo_path);

  g_file_set_contents (buf,
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
"<plist version=\"1.0\"> <dict> <key>creator</key> <string>org.gimp.pippin</string> <key>formatVersion</key> <integer>2</integer> </dict> </plist>", -1, NULL);


#if 0
  fb = stbi_load (argv[1], &rw, &rh, NULL, 4);
  if (!fb)
    return 0;
#else
  if (!g_file_get_contents (argv[1], &asc_source, NULL, NULL))
    {
      fprintf (stderr, "failed to load ascii font\n");
      return -1;
    }
#endif

/* import includes */
  import_includes (&asc_source);

  int y, x0;
  int glyph_no = 0;

  contents_plist = g_string_new (
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\"\n"
"\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
"<plist version=\"1.0\">\n"
"<dict>\n");

  /* read mapping table */

  {
    char linebuf[1024];
    int len;
    char *p = asc_source;

    do {
      p = mem_read (p, linebuf, &len);
      if (len)
        {
          if (linebuf[0] == '!' && linebuf[1] == '!')
            goto mappings_done;
          map[mappings].ascii = linebuf[0];
          if (strchr (&linebuf[2], ' '))
            *strchr (&linebuf[2], ' ')=0;
          map[mappings].name = strdup (&linebuf[2]);
          if (!strcmp (&linebuf[2], "blank"))
            map[mappings].type = C_BLANK;
          else if (!strcmp (&linebuf[2], "solid"))
            map[mappings].type = C_SOLID;
          else if (!strcmp (&linebuf[2], "bright"))
            map[mappings].type = C_BRIGHT;
          else if (!strcmp (&linebuf[2], "dark"))
            map[mappings].type = C_DARK;
          else if (!strcmp (&linebuf[2], "cne"))
            map[mappings].type = C_CNE;
          else if (!strcmp (&linebuf[2], "cnw"))
            map[mappings].type = C_CNW;
          else if (!strcmp (&linebuf[2], "cse"))
            map[mappings].type = C_CSE;
          else if (!strcmp (&linebuf[2], "csw"))
            map[mappings].type = C_CSW;
          else if (!strcmp (&linebuf[2], "lne"))
            map[mappings].type = C_LNE;
          else if (!strcmp (&linebuf[2], "lnw"))
            map[mappings].type = C_LNW;
          else if (!strcmp (&linebuf[2], "lse"))
            map[mappings].type = C_LSE;
          else if (!strcmp (&linebuf[2], "lsw"))
            map[mappings].type = C_LSW;
          else if (!strcmp (&linebuf[2], "ve"))
            map[mappings].type = C_VE;
          else if (!strcmp (&linebuf[2], "vw"))
            map[mappings].type = C_VW;
          else if (!strcmp (&linebuf[2], "vn"))
            map[mappings].type = C_VN;
          else if (!strcmp (&linebuf[2], "vs"))
            map[mappings].type = C_VS;
          mappings++;
        }
    } while (p);

  mappings_done:
    if (0);

    int lineno = 0;
    int gotone = 0;

    int maxx = 0;
    int maxy = 0;

    do {
      p = mem_read (p, linebuf, &len);
      if (len)
        {
          if (linebuf[0] == '(' && linebuf[1] == ' ')
            {
              if (maxy>0)
                {
                  gen_glyph (0, 0, 0, maxx, maxy-1);
                }
              maxx = 0;
              maxy = 0;
              glyph_no = 0;
              lineno = 0;
              glyphs = &linebuf[2];
              if (uglyphs)
                free (uglyphs);
  uglyphs = g_utf8_to_ucs4 (glyphs, -1, &n_glyphs, NULL, NULL);
              if (fb)
                free (fb);
              fb = g_malloc0 (256*256 * sizeof(int));
              stride = 256;
            }
          else
            {
              int x;
              for (x = 0; linebuf[x]; x ++)
                {
                  fb[maxy * stride + x + 1] = map_pix (linebuf[x]);
                  if (x>maxx)
                    maxx=x;
                }
              maxy++;
            }
        }
    } while (p);
    if (maxy>0)
      {
        gen_glyph (0, 0, 0, maxx, maxy-1);
      }
  }

  gen_blocks ();
 
  g_string_append (contents_plist, "</dict>\n</plist>\n");
  sprintf (buf, "%s/glyphs/contents.plist", ufo_path);
  g_file_set_contents (buf, contents_plist->str, contents_plist->len, NULL);

  gen_fontinfo (glyph_height);

  g_file_set_contents ("font.asc", ascii_font->str, ascii_font->len, NULL);

  return 0;
}


void gen_corner_block ()
{
 char buf[1024];
 const char *name;

  GString *str;
  str = g_string_new ("");
  name = "lne";
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);

  name = "lnw";
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);

  name = "lsw";
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);

  name = "lse";
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);



 /*******/

  str = g_string_new ("");
  name = "cne";
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);

  g_string_append_printf (str, "    <point x='%d' y='%d'/>\n", (int)(SCALE * 0.5523), SCALE * 1);
  g_string_append_printf (str, "    <point x='%d' y='%d'/>\n", SCALE * 1, (int)(SCALE * 0.5423));
  
  g_string_append_printf (str, "    <point type='curve' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);

  name = "cnw";
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line'  x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    <point x='%d' y='%d'/>\n", 0,(int)(SCALE*(1-0.5523)));
  g_string_append_printf (str, "    <point x='%d' y='%d'/>\n", (int)(SCALE*(1-0.5523)),SCALE);
  g_string_append_printf (str, "    <point type='curve' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line'  x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);

  name = "csw";
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);

  g_string_append_printf (str, "    <point x='%d' y='%d'/>\n", (int)(SCALE * (1.0-0.5523)),    0);
  g_string_append_printf (str, "    <point x='%d' y='%d'/>\n", 0,    (int)(SCALE * (1.0-0.5523)));
  g_string_append_printf (str, "    <point type='curve' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);

  //g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);


  name = "cse";
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  //g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  //
  g_string_append_printf (str, "    <point x='%d' y='%d'/>\n", (int)(SCALE * 1), (int)(SCALE * 0.5523));
  g_string_append_printf (str, "    <point x='%d' y='%d'/>\n", (int)(SCALE * 0.5523), 0);
  //
  g_string_append_printf (str, "    <point type='curve' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);


  name = "ve";
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE / 2, SCALE / 2 );
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);

  name = "vw";
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE / 2, SCALE / 2 );
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);


  name = "vn";
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE / 2, SCALE / 2 );
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);


  name = "vs";
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE / 2, SCALE / 2 );
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);
}

void gen_solid_block ()
{
 char buf[1024];
  GString *str;
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"solid\" format=\"1\">\n");
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, "solid");
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);


  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"solidv\" format=\"1\">\n");
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 1, (int)(SCALE * -1));
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, (int)(SCALE * -1));
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, "solidv");
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);

  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"solidh\" format=\"1\">\n");
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  g_string_append_printf (str, "    <contour>\n");
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 2, SCALE * 1);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 2, SCALE * 0);
  g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", SCALE * 0, SCALE * 0);
  g_string_append_printf (str, "    </contour>\n");
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, "solidh");
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);

  g_string_append_printf (contents_plist, "<key>solid</key><string>solid.glif</string>\n");
  g_string_append_printf (contents_plist, "<key>solidv</key><string>solidv.glif</string>\n");
  g_string_append_printf (contents_plist, "<key>solidh</key><string>solidh.glif</string>\n");
}

void gen_gray (GString *str, int mod)
{
  int i;
  int no = 0;

#define GO 4
#define NSCALE  (SCALE + GO * 2)


  for (i = 0; i < NSCALE * 2; i++)
  {

    no ++;
    if (no % mod == 0)
    {
      g_string_append_printf (str, "    <contour>\n");
        if (i < NSCALE)
          {
            g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", i - GO,   0 - GO);
            g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", 0 - GO,   i - GO);
          }
        else
          {
            g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", NSCALE - GO, i - NSCALE - GO);
            g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", i - NSCALE - GO, NSCALE - GO);
          }

        if (i+7 < NSCALE)
          {
            g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", 0 - GO,   i+7 - GO);
            g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", i+7 - GO, 0 - GO);
          }
        else
          {
            g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", i+7 - NSCALE - GO, NSCALE - GO);
            g_string_append_printf (str, "    <point type='line' x='%d' y='%d'/>\n", NSCALE - GO, i+7 - NSCALE - GO);
          }
      g_string_append_printf (str, "    </contour>\n");
    }
  }
}

void gen_dia_grays ()
{
  char buf[1024];
  GString *str;
  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"bright\" format=\"1\">\n");
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  gen_gray (str, 21);

  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, "bright");
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);

  str = g_string_new ("");
  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"dark\" format=\"1\">\n");
  g_string_append_printf (str, "  <advance width=\"%i\"/>\n", SCALE);
  g_string_append_printf (str, "  <outline>\n");
  gen_gray (str, 11);
  g_string_append_printf (str, "  </outline>\n");
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, "dark");
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);

  g_string_append_printf (contents_plist, "<key>dark</key><string>dark.glif</string>\n");
  g_string_append_printf (contents_plist, "<key>bright</key><string>bright.glif</string>\n");
}

void gen_blocks ()
{
  gen_solid_block ();
  gen_corner_block ();
  gen_dia_grays ();
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
  g_string_append_printf (str, "	<key>copyright</key>\n");
  g_string_append_printf (str, "	<string>OEyvind Kolaas pippin@gimp.org</string>\n");
  g_string_append_printf (str, "	<key>unitsPerEm</key>\n");
  g_string_append_printf (str, "	<integer>%i</integer>\n", SCALE * glyph_height);
  g_string_append_printf (str, "	<key>openTypeNameLicenseURL</key>\n");
  g_string_append_printf (str, "	<string>http://scripts.sil.org/OFL</string>\n");
  g_string_append_printf (str, "	<key>openTypeNameVersion</key>\n");
  g_string_append_printf (str, "	<string>Version 0.1</string>\n");
  g_string_append_printf (str, "	<key>postscriptFontName</key>\n");
  g_string_append_printf (str, "	<string>%s</string>\n", font_name);
  g_string_append_printf (str, "	<key>postscriptFullName</key>\n");
  g_string_append_printf (str, "	<string>%s</string>\n", font_name);
  g_string_append_printf (str, "	<key>postscriptWeightName</key>\n");
  g_string_append_printf (str, "	<string>%s</string>\n", font_type);
  g_string_append_printf (str, "    </dict>\n");
  g_string_append_printf (str, "</plist>\n");

  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
}



char *mem_read (char *start,
                char *linebuf,
                int  *len)
{
    int  int_len;
    char *p = start;
    if (!len)
      len = &int_len;
    *len = 0;
    for (p = start; *p ;p++)
      {
        switch (*p)
         {
           case '#':
            while (*p && *p!='\n')p++;
            if (*p == '\n') p--;
             break;
           case '\n':
             {
               linebuf[*len]=0;
               return p+1;
             }
             break;
           case '\\':
             p++;
             if (*p)
               {
                 linebuf[(*len)++]=*p;
               }
             else
               p--;
             break;
           default:
             linebuf[(*len)++]=*p;
             break;
         }
      }
  return NULL;
}
