/*
 * Copyright (c) 2013 Øyvind Kolås <pippin@gimp.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <glib.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int SCALE=512;
static int overlap_solid=1;
static int author_mode = 0;
static int y_shift = 0;
static int rbearing_reduce = 0;
static int inline_components = 0;

static int got_blank = -1; /* if this is non 0 we have a blank glyph! */

/* we expect to find these, in this order at the beginning of the
 * palette
 */
enum {
  C_BLANK=0,
  C_SOLID
};

typedef struct Mapping {
  gchar  *name;
  int     type;
  gchar   ascii;
} Mapping;

static Mapping catalog[512]={{0,},};
static int     n_catalog = 0;

const char *mapping2str (int type)
{
  if (type < n_catalog)
    return catalog[type].name;
  return NULL;
}

int resolve_mapping_str (const char *str)
{
  int i;
  for (i = 0; i < n_catalog; i++)
    if (!strcmp (str, catalog[i].name))
      return i;
  return 0;
}

int catalog_add (const gchar *name)
{
  int i;
  for (i = 0; i < n_catalog; i++)
    if (!strcmp (name, catalog[i].name))
      {
        fprintf (stderr, "%s already exists!\n");
        return i;
      }
  catalog[n_catalog++].name = g_strdup (name);
  return i;
}

static Mapping map[256]={{0,},};
static int     mappings=0;

gchar *component_name_tmp = NULL;
GString *component_str = NULL;

void write_component (const char *name, const char *curve_xml);

void finalize_component (void)
{
  if (!component_name_tmp)
    return;
  g_string_append_printf (component_str, "</contour>\n");

  write_component (component_name_tmp, component_str->str);
  g_free (component_name_tmp);
  g_string_free (component_str, TRUE);
  component_name_tmp = NULL;
  component_str = NULL;
}

void add_component (const char *component_name)
{
  finalize_component ();
  component_name_tmp = g_strdup (component_name);
  component_str = g_string_new ("<contour>\n");
}

void add_point (char type, float x, float y)
{
  g_assert (component_str);
  switch (type)
    {
      case 'L':
        g_string_append_printf (component_str,
            "    <point type='line' x='%d' y='%d'/>\n",
            (int)(SCALE * x), (int)(SCALE * y));
      break;
      case 'c':
        g_string_append_printf (component_str,
            "    <point type='offcurve' x='%d' y='%d'/>\n",
            (int)(SCALE * x), (int)(SCALE * y));
      break;
      case 'C':
        g_string_append_printf (component_str,
            "    <point type='curve' x='%d' y='%d'/>\n",
            (int)(SCALE * x), (int)(SCALE * y));
      break;
        break;
    }
}

void add_subpath (void)
{
  g_string_append_printf (component_str, "</contour>\n<contour>\n");
}

void add_gray_block (float fill_ratio, float paramA, float paramB);

char *mem_read (char *start,
                char *linebuf,
                int  *len);


static int map_pix (char pix)
{
  int i;
  for (i = 0; i < mappings; i++)
    if (map[i].ascii == pix)
      return map[i].type;
  return C_BLANK;
}


static const char *font_name = NULL;
static const char *font_variant = NULL;

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

void write_glyph (const char *name, int advance,
                  signed long unicode,
                  const char *inner_outline)
{
  GString *str;
  char buf[1024];
  str = g_string_new ("");

  g_string_append_printf (str, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  g_string_append_printf (str, "<glyph name=\"%s\" format=\"1\">\n", name);
  g_string_append_printf (str, "  <advance width=\"%d\"/>\n", advance - (rbearing_reduce?((int)(SCALE / rbearing_reduce)):0));
  if (unicode>= 0)
    g_string_append_printf (str, "  <unicode hex=\"%04X\"/>\n", unicode);
  g_string_append_printf (str, "  <outline>\n%s</outline>\n", inner_outline);
  g_string_append_printf (str, "</glyph>\n");
  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_set_contents (buf, str->str, str->len, NULL);
  g_string_free (str, TRUE);
}

char *load_component_outline (const char *name, int xoffset, int yoffset)
{
  GString *str = g_string_new ("");
  char *data = NULL;
  char buf[1024];
  int outline_count = 0;
  int len;

  sprintf (buf, "%s/glyphs/%s.glif", ufo_path, name);
  g_file_get_contents (buf, &data, NULL, NULL);

  char *p = data;

  do {
    p = mem_read (p, buf, &len);
    if (p)
      {
        if (strstr (buf, "outline"))
          outline_count++;
        else if (outline_count == 1)
          {
            if (strstr (buf, "contour"))
              g_string_append_printf (str, "%s\n", buf);
            else if (strstr (buf, "point"))
              {
                char type[64];
                int x;
                int y;
                sscanf (buf, "    <point type='%s x='%d' y='%d'/>", &type, &x, &y);
                type[strlen(type)-1]=0;
                g_string_append_printf (str, "    <point type='%s' x='%d' y='%d'/>\n",  type, x + xoffset * SCALE, y + yoffset * SCALE);
              }
          }
      }
  } while (p);

  return g_string_free (str, FALSE);
}

void gen_ref_glyph (Mapping *mapping, int xw, int xh)
{
  GString *str;
  char name[8];
  sprintf (name, "%04X", mapping->ascii);
  str = g_string_new ("");
  g_string_append_printf (str, "<component base=\"%s\" xOffset=\"0\" yOffset=\"%d\" xScale=\"%d\" yScale=\"%d\"/>\n",
       mapping->name, -SCALE, xw, xh);
  write_glyph (name, (xw) * SCALE, mapping->ascii, str->str);
  g_string_free (str, TRUE);
}

static void glyph_add_component (GString *str, const char *name, int x, int y)
{
  if (!strcmp (name, "blank") && got_blank == 0)
    return;
  if (inline_components)
    {
      char *component = load_component_outline (name, x, y);
      g_assert (component);
      g_string_append_printf (str, "%s", component);
      g_free (component);
    }
  else
    g_string_append_printf (str,
        "  <component base=\"%s\" xOffset=\"%d\" yOffset=\"%d\"/>\n",
        name, x * SCALE, y * SCALE); 
}

void gen_glyph (int glyph_no, int x0, int y0, int x1, int y1)
{
  GString *str;
  gchar utf8_chr[8]={0,0,0,0,};
  int x, y;
  x0++;
  x1++;

  if (glyph_no >= n_glyphs)
    return;
  char name[8];
  sprintf (name, "%04X", uglyphs[glyph_no]);

  if (y1 - y0 - 1> glyph_height)
    {
      fprintf (stderr, "%s %i\n", name, glyph_height);
      glyph_height = y1 - y0 + 1;
    }

  g_unichar_to_utf8 (uglyphs[glyph_no], utf8_chr);
  str = g_string_new ("");

  if (overlap_solid)
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
                glyph_add_component (str, "solidv", u, v);
                if (x <x1 && fb[stride *(y)+(x+1)] == C_SOLID)
                  glyph_add_component (str, "solidh", u, v);
              }
            else
              {
                if (x <x1 && fb[stride *(y)+(x+1)] == C_SOLID)
                  glyph_add_component (str, "solidh", u, v);
                else
                  glyph_add_component (str, "solid", u, v);
              }
          }
      }

  for (y = y0; y <= y1; y++)
    for (x = x0; x <= x1; x++)
      {
        int *pix = &fb[stride * y+x];
        int u = x - x0;
        int v = y1 - y -1 + y_shift;
        const char *component = NULL;

        if (overlap_solid && *pix == C_SOLID)
          component = NULL;
        else
          component = mapping2str (*pix);

        if (component)
          glyph_add_component (str, component, u, v);
      }

  write_glyph (name, (x1-x0+1) * SCALE, uglyphs[glyph_no], str->str);

  {
    char buf[1024];
    sprintf (buf, "<key>%s</key>", name);
    if (!strstr (contents_plist->str, buf))
    g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);
  }

  g_string_free (str, TRUE);
}

void gen_fontinfo (int glyph_height);

static void str_chomp (char *string)
{
  char *p;
  
  /* trim comment */
  p = strchr (string, '#');
  if (p)
   *p = '\0';

  /* trim trailing spaces, */
  p = &string[strlen(string)-1];
  p--;
  while (p && p>string && *p == ' ')
    {
      *p = '\0';
      p--;
    }
}

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
          char *read = NULL;

          str_chomp (linebuf);

          g_file_get_contents (&linebuf[strlen("include ")], &read, NULL, NULL);
          if (read)
            {
              import_includes (&read);
              g_string_append_printf (new, "%s", read);
              g_free (read);
            }
        }
        else if (g_str_has_prefix (linebuf, "authormode"))
          author_mode = 1;

#define PARSE_INT(var, prefix) \
        else if (g_str_has_prefix (linebuf, prefix)) \
          var = atoi (&linebuf[strlen(prefix)]);
#define PARSE_STRING(var, prefix) \
        else if (g_str_has_prefix (linebuf, prefix)) \
          var = g_strdup (&linebuf[strlen(prefix)]);

        PARSE_INT (inline_components, "inline_components ")
        PARSE_INT (y_shift,           "y_shift ")
        PARSE_INT (rbearing_reduce,           "rbearing_reduce ")
        PARSE_INT (overlap_solid,     "overlap_solid ")
        PARSE_INT (SCALE,             "scale")
        PARSE_STRING (font_variant,   "variant ")
        PARSE_STRING (font_name,      "fontname ")

#undef PARSE_INT
#undef PARSE_STRING
        else
        {
          g_string_append_printf (new, "%s\n", linebuf);
        }
      }
  } while (p);

  *asc_source = new->str;
  g_string_free (new, FALSE);
}

void write_component (const char *name, const char *curve_xml)
{
  write_glyph (name, SCALE, -1, curve_xml);
  g_string_append_printf (contents_plist, "<key>%s</key><string>%s.glif</string>\n", name, name);
  catalog_add (name);
  if (!strcmp (name, "blank"))
    got_blank ++;
}

void gen_components ()
{
  write_component ("blank", "");
  add_component ("solid");
  add_point ('L', 0, 1);
  add_point ('L', 1, 1);
  add_point ('L', 1, 0);
  add_point ('L', 0, 0);

  add_component ("solidv");
  add_point ('L', 0, 1);
  add_point ('L', 1, 1);
  add_point ('L', 1, -1);
  add_point ('L', 0, -1);

  add_component ("solidh");
  add_point ('L', 0, 1);
  add_point ('L', 2, 1);
  add_point ('L', 2, 0);
  add_point ('L', 0, 0);
  finalize_component ();
}


int main (int argc, char **argv)
{
  int y0 = 0, y1 = 0;

  if (argc != 2)
    {
      fprintf (stderr, "Usage: %s <fontsource.asc>\n", argv[0]);
      return -1;
    }
  if (!g_file_get_contents (argv[1], &asc_source, NULL, NULL))
    {
      fprintf (stderr, "failed to load ascii font\n");
      return -1;
    }

  import_includes (&asc_source);

  uglyphs = NULL;
  glyphs    = NULL;

  sprintf (ufo_path, "%s.ufo", font_name);
  char buf[2048];
  sprintf (buf, "mkdir %s > /dev/null 2>&1", ufo_path); system (buf);
  sprintf (buf, "mkdir %s/glyphs > /dev/null 2>&1", ufo_path); system (buf);
  sprintf (buf, "%s/metainfo.plist", ufo_path);

  g_file_set_contents (buf,
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
"<plist version=\"1.0\"> <dict> <key>creator</key> <string>org.gimp.pippin</string> <key>formatVersion</key> <integer>2</integer> </dict> </plist>", -1, NULL);


  int y, x0;
  int glyph_no = 0;

  contents_plist = g_string_new (
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\"\n"
"\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
"<plist version=\"1.0\">\n"
"<dict>\n");

  gen_components (); /* add components which always are present */
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

          switch (linebuf[0])
          {
            case '{': /* new component definition */
              {
                add_component (&linebuf[2]);
                break;
              }
            case 'L': /* line-to */
            case 'c': /* curve-to */
            case 'C': /* curve-to */
              {
                int type;
                float x, y;
                sscanf (&linebuf[0], "%c %f %f", &type, &x, &y);
                add_point (type, x, y);
                break;
              }
            case 'Z': /* new sub-path */
              add_subpath ();
              break;
            case 'G':
              {
                float arg1;
                float arg2;
                float ratio;
                sscanf (&linebuf[2], "%f %f", &ratio, &arg1, &arg2);
                add_gray_block (ratio, arg1, arg2);
                break;
              }
              break;

            default:
              finalize_component ();
              map[mappings].ascii = linebuf[0];
              if (strchr (&linebuf[2], ' '))
                *strchr (&linebuf[2], ' ')=0;
              map[mappings].name = strdup (&linebuf[2]);
              map[mappings].type = resolve_mapping_str (&linebuf[2]);
              mappings++;
          }
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
      //str_chomp (linebuf);
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


  if (author_mode)
    {
      int i;
      for (i = 0; map[i].ascii; i++)
        {
          gen_ref_glyph (&map[i], maxy-2, maxy+1);
        }
    }
  }
 
  g_string_append (contents_plist, "</dict>\n</plist>\n");
  sprintf (buf, "%s/glyphs/contents.plist", ufo_path);
  g_file_set_contents (buf, contents_plist->str, contents_plist->len, NULL);
  gen_fontinfo (glyph_height);

  return 0;
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
  g_string_append_printf (str, "	<string>Copyright (c) 2013 OEyvind Kolaas (pippin@gimp.org)</string>\n");
  g_string_append_printf (str, "	<key>unitsPerEm</key>\n");
  g_string_append_printf (str, "	<integer>%i</integer>\n", SCALE * glyph_height);
#if 0
  g_string_append_printf (str, "	  <key>openTypeHheaLineGap</key> <integer>0</integer>\n");
  g_string_append_printf (str, "	  <key>openTypeOS2TypoLineGap</key> <integer>0</integer>\n");

  g_string_append_printf (str, "	  <key>openTypeHheaDescender</key> <integer>0</integer>\n");
  g_string_append_printf (str, "	  <key>openTypeOS2WinDescent</key> <integer>0</integer>\n");
  g_string_append_printf (str, "	  <key>descender</key> <integer>0</integer>\n");

  g_string_append_printf (str, "	  <key>openTypeHheaAscender</key> <integer>%i</integer>\n", SCALE * (glyph_height+1));
  g_string_append_printf (str, "	  <key>openTypeOS2WinAscent</key> <integer>%i</integer>\n", SCALE * (glyph_height+1));
  g_string_append_printf (str, "	  <key>ascender</key> <integer>%i</integer>\n", SCALE * (glyph_height+1));
#endif


  g_string_append_printf (str, "	<key>openTypeNameLicenseURL</key>\n");
  g_string_append_printf (str, "	<string>http://scripts.sil.org/OFL</string>\n");
  g_string_append_printf (str, "	<key>openTypeNameLicense</key>\n");
  g_string_append_printf (str, "	<string>Licenced under the SIL Open Font License, Version 1.1, available with a FAQ at: http://scripts.sil.org/OFL</string>\n");
  g_string_append_printf (str, "	<key>openTypeNameVersion</key>\n");
  g_string_append_printf (str, "	<string>Version 0.1</string>\n");
  g_string_append_printf (str, "	<key>postscriptFontName</key>\n");
  g_string_append_printf (str, "	<string>%s</string>\n", font_name);
  g_string_append_printf (str, "	<key>postscriptFullName</key>\n");
  g_string_append_printf (str, "	<string>%s</string>\n", font_name);
  g_string_append_printf (str, "	<key>postscriptWeightName</key>\n");
  g_string_append_printf (str, "	<string>%s</string>\n", font_variant);
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


void add_gray_block_gray (float fill_ratio, float paramA, float paramB)
{
  int step = 7;
  fill_ratio = 1.0 - fill_ratio;
  int mod = step + step * 3.0*fill_ratio;
  float scale = SCALE;
  int i;
  int no = 0;
#define GO 1
#define NSCALE  (SCALE + GO * 2)
  int again = 0;
  for (i = 0; i < NSCALE * 2; i++)
  {
    no ++;
    if (no % mod == 0)
    {
        if (again)
          add_subpath ();
        again = 1;
        if (i < NSCALE)
          {
            add_point ('L', (i - GO)/scale,  (0 - GO)/scale);
            add_point ('L', (0 - GO)/scale,  (i - GO)/scale);
          }
        else
          {
            add_point ('L', (NSCALE - GO)/scale, (i - NSCALE - GO)/scale);
            add_point ('L', (i - NSCALE - GO)/scale, (NSCALE - GO)/scale);
          }

        if (i+step < NSCALE)
          {
            add_point ('L', (0 - GO)/scale,   (i+step - GO)/scale);
            add_point ('L', (i+step - GO)/scale, (0 - GO)/scale);
          }
        else
          {
            add_point ('L', (i+step - NSCALE - GO)/scale, (NSCALE - GO)/scale);
            add_point ('L', (NSCALE - GO)/scale, (i+step - NSCALE - GO)/scale);
          }
    }
  }
#undef GO
#undef NSCALE
}

void add_scaled_point (int type, float x, float y, float scale)
{
  x -= 0.5;
  y -= 0.5;
  scale *= 1.5;
  x *= scale;
  y *= scale;

  x += 0.5;
  y += 0.5;

  add_point (type,
      x, 
      y);
}

void add_gray_block_circle (float fill_ratio, float paramA, float paramB)
{
  float x, y;
  add_scaled_point ('c', 0.11, 0.29, fill_ratio);
  add_scaled_point ('c', 0.29, 0.11, fill_ratio);
  add_scaled_point ('C', 0.50, 0.11, fill_ratio);
  add_scaled_point ('c', 0.71, 0.11, fill_ratio);
  add_scaled_point ('c', 0.89, 0.29, fill_ratio);
  add_scaled_point ('C', 0.89, 0.50, fill_ratio);
  add_scaled_point ('c', 0.89, 0.71, fill_ratio);
  add_scaled_point ('c', 0.71, 0.89, fill_ratio);
  add_scaled_point ('C', 0.50, 0.89, fill_ratio);
  add_scaled_point ('c', 0.29, 0.89, fill_ratio);
  add_scaled_point ('c', 0.11, 0.71, fill_ratio);
  add_scaled_point ('C', 0.11, 0.50, fill_ratio);

}

void add_gray_block (float fill_ratio, float paramA, float paramB)
{
  int type = paramA;
  switch (type)
    {
      case 1: add_gray_block_gray (fill_ratio, paramA, paramB);return;
      case 2: add_gray_block_circle (fill_ratio, paramA, paramB);return;
    }
}
